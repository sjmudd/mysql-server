/* Copyright (c) 2023, 2024, Oracle and/or its affiliates. */

#include "sql/statement/statement_runnable.h"

#include "mysql/psi/mysql_ps.h"  // MYSQL_EXECUTE_PS
#include "sql/sp_head.h"
#include "sql/sp_rcontext.h"
#include "sql/sql_class.h"
#include "sql/sql_digest_stream.h"
#include "sql/sql_lex.h"
#include "sql/sql_parse.h"  // sql_command_flags
#include "sql/statement/utils.h"

Server_runnable::~Server_runnable() = default;

Statement_runnable::Statement_runnable(LEX_STRING sql_text)
    : m_sql_text(sql_text) {}

/**
  Parse and execute a statement. Does not prepare the query.

  Allows to execute a statement from within another statement.
  Supports even executing a statement from within stored program.
  The main property of the implementation is that it does not
  affect the environment -- i.e. you  can run many
  executions without having to cleanup/reset THD in between.
*/

bool Statement_runnable::execute_server_code(THD *thd) {
  sql_digest_state *parent_digest;
  PSI_statement_locker *parent_locker;

  if (alloc_query(thd, m_sql_text.str, m_sql_text.length)) return true;

  Parser_state parser_state;
  if (parser_state.init(thd, thd->query().str, thd->query().length)) {
    // OOM
    return true; /* purecov: inspected */
  }

  parser_state.m_lip.multi_statements = false;
  lex_start(thd);

  const bool executing_statement_under_stored_program =
      (thd->sp_runtime_ctx != nullptr);

  if (executing_statement_under_stored_program) {
    thd->lex->sphead = thd->sp_runtime_ctx->sp;

    /*
      Make sure that we are not here while parsing a statement under stored
      program. In other words, Execute_regular_statement::execute() and
      Ed_connection::execute_direct() should not be invoked while parsing
      another SP statement.
    */
    assert(thd->lex->get_sp_current_parsing_ctx() == nullptr);
  }

  parent_digest = thd->m_digest;
  parent_locker = thd->m_statement_psi;
  thd->m_digest = nullptr;
  thd->m_statement_psi = nullptr;
  bool error = parse_sql(thd, &parser_state, nullptr) || thd->is_error();
  thd->m_digest = parent_digest;
  thd->m_statement_psi = parent_locker;

  if (error) goto end;

  /*
    Make sure that no new stored program is created while executed statement
    under stored program. CREATE/ALTER/DROP SP under stored program are
    restricted.
  */
  assert(!executing_statement_under_stored_program ||
         thd->lex->sphead == thd->sp_runtime_ctx->sp);

  // Set multi-result state if statement belongs to SP.
  if (executing_statement_under_stored_program) {
    error = set_sp_multi_result_state(thd, thd->lex);
    if (error) goto end;
  }

  thd->lex->set_trg_event_type_for_tables();

  parent_locker = thd->m_statement_psi;
  thd->m_statement_psi = nullptr;

  /*
    Rewrite first (if needed); execution might replace passwords
    with hashes in situ without flagging it, and then we'd make
    a hash of that hash.
  */
  rewrite_query_if_needed(thd);
  log_execute_line(thd);

  set_query_for_display(thd);

  error = mysql_execute_command(thd);
  thd->m_statement_psi = parent_locker;

end:
  /*
    lex_end() would free sphead. Don't free sphead which invoked this
    statement.
  */
  if (executing_statement_under_stored_program) {
    assert(thd->lex->sphead == thd->sp_runtime_ctx->sp);
    thd->lex->sphead = nullptr;
  }

  lex_end(thd->lex);

  return error;
}
