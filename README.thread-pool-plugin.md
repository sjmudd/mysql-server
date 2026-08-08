# thread_pool plugin backport: 8.4 branch, ol10 container build

`plugin/thread_pool` is open-source on trunk but absent from the `8.4` branch
(MySQL 8.4.11), where thread pool was enterprise-only. Everything below was
copied/ported from `trunk` at commit `06a5c1c99c377fc41b2eba1ea244e8b220bdc3c8`,
tag `mysql-26.7.0`. Result:
`plugin/thread_pool/thread_pool.so` builds cleanly against `8.4` in an
oraclelinux:10 container. **Compile/link feasibility only** — the plugin has
not been loaded into a running mysqld or exercised functionally.

## Repro

```sh
docker run --rm --network=host \
  -v /data1/other/src/mysql-server:/data/git \
  -w /data/git oraclelinux:10 bash -c '
set -e
yum install -y oracle-epel-release-el10
yum install -y "dnf-command(config-manager)"
yum config-manager --set-enabled ol10_codeready_builder
yum install -y git cmake libtirpc-devel rpcgen annobin-annocheck \
  annobin-plugin-gcc binutils dwz gcc-c++ gcc libfido2-devel \
  libquadmath-devel libudev-devel bison elfutils patchelf perl time \
  libaio-devel libcurl-devel krb5-devel ncurses-devel numactl-devel \
  openssl-devel zlib-devel pkgconfig cyrus-sasl-devel openldap-devel
cd bin && cmake .. && cmake --build . --target thread_pool -j"$(nproc)"
'
```

Note: Repo enable order matters as shown.

`bin/` is a plain out-of-tree cmake build dir at the repo root (`mkdir bin;
cd bin; cmake ..`), bind-mounted from the host so it survives across `--rm`
container runs.

## Source changes (branch 8.4, relative to a plain checkout)

1. **Copy `plugin/thread_pool/` from trunk as `plugin/thread_pool/`**.
   The plugin's own sources
   `#include "plugin/thread_pool/src/..."` as project-root-relative paths, so
   the on-disk directory name must match trunk's exactly.

2. **`components/library_mysys/my_system.cc` + `my_system_api/{my_system_api.h,
   my_system_api_common.cc, my_system_api_cgroup.cc, my_system_api_linux.cc}`**
   — copied from trunk (Linux-only backend, apple/freebsd/solaris/win skipped).
   Provides `my_num_vcpus()`, declared in
   `include/mysql/components/library_mysys/my_system.h` (byte-identical to
   trunk). Wired into `components/library_mysys/CMakeLists.txt` via a new
   `MY_SYSTEM_SOURCES` list (`IF(UNIX)`/`IF(LINUX)` gated, mirroring trunk's
   structure minus the non-Linux backends).

   **Trimmed vs trunk**: dropped `my_physical_memory()` /
   `init_my_physical_memory()` entirely from both `my_system.h` and
   `my_system.cc`. 8.4 already has a different, incompatible
   `my_physical_memory()` in `mysys/my_system.cc` (declared in
   `include/my_sys.h`, used by `sql/sys_vars.cc` and
   `storage/innobase/handler/ha_innodb.cc`) — same name, different return type
   (`unsigned long long` vs `uint64_t`). Both get pulled into the same
   translation unit transitively via `mysql/plugin.h`, which is a hard compile
   error (`ambiguating new declaration`). thread_pool never calls
   `my_physical_memory()`, only `my_num_vcpus()`, so dropping it avoided
   touching `mysys/my_system.cc` / `include/my_sys.h` / `sys_vars.cc` /
   `ha_innodb.cc` — zero core-file edits for this issue.

3. **`share/messages_to_error_log.txt`**: added 14 error message entries
   (13 genuinely missing from trunk's 31 thread_pool-related codes, plus
   `ER_THREAD_POOL_LOW_LEVEL_INIT_FAILED`). Initially assumed this last one was
   a straight rename of 8.4's existing `ER_TRHEAD_POOL_LOW_LEVEL_INIT_FAILED`
   ("TRHEAD" typo) — it isn't: `thread_pool.cc` uses both, at three different
   call sites, for two distinct conditions (`ER_TRHEAD_POOL_LOW_LEVEL_INIT_FAILED`
   = "tp_group_low_level_init() failed", the group-level init path;
   `ER_THREAD_POOL_LOW_LEVEL_INIT_FAILED` = "tp_client_low_level_init() failed",
   the per-client path). The typo'd name is a real, actively-shipped 8.4 error
   code (not obsolete, not dead) — left as-is; the two are added side by side,
   not one replacing the other. **18 of trunk's 31 thread_pool codes already
   existed in 8.4** — leftover from when the enterprise (closed-source) plugin
   shipped against this same open error catalog. Don't assume a missing symbol
   means missing infrastructure; check for a differently-named or
   differently-spelled survivor first.

   The 14 added entries use the exact same numeric error codes as trunk, not
   auto-assigned ones — pinned via `start-error-number` pragmas, following the
   convention already established in this file's "Error numbers backported
   from 9.7 to 8.4" section ("Make sure the error numbers are identical
   between versions"). The real numbers were obtained directly, not scraped
   from docs: `git show trunk:share/messages_to_error_log.txt` +
   `git show trunk:share/messages_to_clients.txt`, then run through the
   already-built local `comp_err` binary
   (`bin/runtime_output_directory/comp_err`) to generate trunk's actual
   `mysqld_error.h` and read the real values off it. Placement matters: the
   11 non-`WARN_` entries (15509-15519) sit right after 8.4's existing
   `ER_AUTH_INITIAL_PLUGIN_OVERRIDE` (15148), in the unused numeric gap before
   the "backported from 9.7" section's own `start-error-number 15604` pragma;
   the 3 `ER_WARN_THREAD_POOL_*` entries (15643-15645) had to go *inside*
   that backported section instead (after `ER_AUDIT_LOG_OFFLOAD_ERROR`=15610,
   before `start-error-number 15694`), since 8.4's own sequential numbering
   by the end of the "8.1-8.4" section has already passed 15643 — `comp_err`
   enforces `start-error-number` as forward-only, which is exactly what
   caught this (first attempt failed with "start-error-number may only
   increase the index"). Verified by running the local `comp_err` against the
   edited file and diffing the generated numbers against trunk's.

4. **3 small functions added to match `include/mysql/thread_pool_priv.h`**
   (that header was otherwise already ~99% identical to trunk's):
   - `thd_get_mysql_socket(THD*)` — one-liner in `sql/sql_thd_api.cc`
     (`thd->get_protocol_classic()->get_vio()->mysql_socket`), same idiom used
     elsewhere in the tree already.
   - `get_aborted_connects()` — wrapper in `sql/sql_thd_api.cc` around
     `Connection_handler_manager::aborted_connects()`, which **already
     existed** in `sql/conn_handler/connection_handler_manager.h` (just wasn't
     exposed as a free function).
   - `get_incoming_connects()` — genuinely new: added a
     `static std::atomic_ulong incoming_connection_count` member +
     `get_incoming_connects()` accessor to `Connection_handler_manager`
     (`connection_handler_manager.h`/`.cc`), incremented in
     `check_and_incr_conn_count()`. 8.4's `Connections` status var is tracked
     differently (`show_thread_id_count`), not via a dedicated atomic counter,
     so this one couldn't be reused as-is. Trunk's version of this same
     function also takes an `internal_session` param (skipped — that's
     unrelated new infra in `Connection_handler_manager`, not present on 8.4
     and not needed here).

5. **Dropped, not ported**: `plugin/thread_pool/src/option_usage.cc` (deleted,
   along with `option_usage.h`) and its one caller site in `thread_pool.cc`
   (`++opt_option_tracker_usage_thread_pool_plugin;`), plus the init/deinit
   calls and status-var entry in `thread_pool_plugin.cc`. This is Oracle
   feature-usage telemetry (`mysql_option_tracker` component service), which
   doesn't exist on 8.4 at all and isn't core thread-pool functionality.

6. **Stubbed, not ported**: `get_system_variable_value()` in
   `thread_pool_plugin.cc` (used once, to read a new `container_aware` sysvar
   via the `mysql_system_variable_reader` component service — also entirely
   absent from 8.4). Replaced with a function that always returns `"OFF"`
   instead of actually querying the service, so `init_container_aware()` still
   gets called with a valid value. **This means `container_aware` mode is
   effectively hardcoded off** — fine for a compile/link test, not fine for
   real cgroup-aware behavior if this ever needs to actually run.

## Known gaps / not done

- `mysql_system_variable_reader` component service (`sql/server_component/
  mysql_system_variable_reader_imp.{cc,h}` on trunk) was never ported — see
  item 6 above. Porting it for real would let `container_aware` be
  configurable again instead of hardcoded off.
- Not tested: loading `thread_pool.so` into a running mysqld, `INSTALL
  PLUGIN`, `SET GLOBAL thread_handling=...`, or any functional behavior.
  Given the version gap (8.4.11 vs trunk 26.7), further runtime-only issues
  (missing PFS table registrations, status/sysvar wiring, ABI mismatches)
  are plausible and unverified.
- `include/mysql/components/library_mysys/my_system.h`/`.cc` now diverge from
  trunk (physical-memory API removed) — fine for this local experiment, but
  not a drop-in match if trunk's version is ever ported wholesale later.
