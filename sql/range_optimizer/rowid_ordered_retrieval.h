/* Copyright (c) 2000, 2021, Oracle and/or its affiliates.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef SQL_RANGE_OPTIMIZER_ROWID_ORDERED_RETRIEVAL_H_
#define SQL_RANGE_OPTIMIZER_ROWID_ORDERED_RETRIEVAL_H_

#include <assert.h>
#include <sys/types.h>
#include <vector>

#include "my_alloc.h"
#include "my_inttypes.h"
#include "priority_queue.h"
#include "sql/handler.h"
#include "sql/malloc_allocator.h"
#include "sql/range_optimizer/range_optimizer.h"
#include "sql/range_optimizer/range_scan.h"
#include "sql/sql_list.h"
#include "sql/table.h"

class String;
class THD;
struct MY_BITMAP;

/*
  Rowid-Ordered Retrieval (ROR) index intersection quick select.
  This quick select produces intersection of row sequences returned
  by several QUICK_RANGE_SELECTs it "merges".

  All merged QUICK_RANGE_SELECTs must return rowids in rowid order.
  QUICK_ROR_INTERSECT_SELECT will return rows in rowid order, too.

  All merged quick selects retrieve {rowid, covered_fields} tuples (not full
  table records).
  QUICK_ROR_INTERSECT_SELECT retrieves full records if it is not being used
  by QUICK_ROR_INTERSECT_SELECT and all merged quick selects together don't
  cover needed all fields.

  If one of the merged quick selects is a Clustered PK range scan, it is
  used only to filter rowid sequence produced by other merged quick selects.
*/

class QUICK_ROR_INTERSECT_SELECT : public RowIDCapableRowIterator {
 public:
  QUICK_ROR_INTERSECT_SELECT(THD *thd, TABLE *table_arg,
                             bool retrieve_full_rows,
                             bool need_rows_in_rowid_order,
                             MEM_ROOT *return_mem_root);
  ~QUICK_ROR_INTERSECT_SELECT() override;

  bool Init() override;
  int Read() override;
  uchar *last_rowid() const override {
    assert(need_rows_in_rowid_order);
    return m_last_rowid;
  }

  bool push_quick_back(QUICK_RANGE_SELECT *quick_sel_range);

  /*
    Range quick selects this intersection consists of, not including
    cpk_quick.
  */
  List<QUICK_RANGE_SELECT> quick_selects;

  MEM_ROOT *mem_root; /* Memory pool for this and merged quick selects data. */
  /*
    Merged quick select that uses Clustered PK, if there is one. This quick
    select is not used for row retrieval, it is used for row retrieval.
  */
  QUICK_RANGE_SELECT *cpk_quick;

  bool retrieve_full_rows; /* if true, do retrieve full table records. */
  /* in top-level quick select, true if merged scans where initialized */
  bool scans_inited;

 private:
  const bool need_rows_in_rowid_order;
  uchar *m_last_rowid;
  bool inited = false;

  bool init_ror_merged_scan();
};

/*
  Comparison function to be used QUICK_ROR_UNION_SELECT::queue priority
  queue.
*/
struct Quick_ror_union_less {
  explicit Quick_ror_union_less(const handler *file) : m_file(file) {}
  bool operator()(RowIDCapableRowIterator *a, RowIDCapableRowIterator *b) {
    return m_file->cmp_ref(a->last_rowid(), b->last_rowid()) > 0;
  }
  const handler *m_file;
};

/*
  Rowid-Ordered Retrieval index union select.
  This quick select produces union of row sequences returned by several
  quick select it "merges".

  All merged quick selects must return rowids in rowid order.
  QUICK_ROR_UNION_SELECT will return rows in rowid order, too.

  All merged quick selects are set not to retrieve full table records.
  ROR-union quick select always retrieves full records.

*/

class QUICK_ROR_UNION_SELECT : public TableRowIterator {
 public:
  QUICK_ROR_UNION_SELECT(MEM_ROOT *return_mem_root, THD *thd, TABLE *table);
  ~QUICK_ROR_UNION_SELECT() override;

  bool Init() override;
  int Read() override;

  bool push_quick_back(RowIDCapableRowIterator *quick_sel_range);

  List<RowIDCapableRowIterator> quick_selects; /* Merged quick selects */

  Priority_queue<RowIDCapableRowIterator *,
                 std::vector<RowIDCapableRowIterator *,
                             Malloc_allocator<RowIDCapableRowIterator *>>,
                 Quick_ror_union_less>
      queue; /* Priority queue for merge operation */

  MEM_ROOT *mem_root; /* Memory pool for this and merged quick selects data. */
  uchar *cur_rowid;   /* buffer used in Read() */
  uchar *prev_rowid;  /* rowid of last row returned by Read() */
  bool have_prev_rowid; /* true if prev_rowid has valid data */
  uint rowid_length;    /* table rowid length */

 private:
  bool scans_inited;
  bool inited = false;
};

#endif  // SQL_RANGE_OPTIMIZER_ROWID_ORDERED_RETRIEVAL_H_
