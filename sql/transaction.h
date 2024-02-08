/* Copyright (c) 2008, 2023, Oracle and/or its affiliates.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is designed to work with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have either included with
   the program or referenced in the documentation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <my_global.h>
#include <m_string.h>

class THD;

bool trans_check_state(THD *thd);
void trans_reset_one_shot_chistics(THD *thd);
void trans_track_end_trx(THD *thd);

bool trans_begin(THD *thd, uint flags= 0);
bool trans_commit(THD *thd);
bool trans_commit_implicit(THD *thd);
bool trans_rollback(THD *thd);
bool trans_rollback_implicit(THD *thd);

bool trans_commit_stmt(THD *thd);
bool trans_rollback_stmt(THD *thd);
bool trans_commit_attachable(THD *thd);

bool trans_savepoint(THD *thd, LEX_STRING name);
bool trans_rollback_to_savepoint(THD *thd, LEX_STRING name);
bool trans_release_savepoint(THD *thd, LEX_STRING name);

#endif /* TRANSACTION_H */
