/* Copyright (c) 2018, 2023, Oracle and/or its affiliates.

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

#ifndef LOGIN_SESSION_VARIABLES_INCLUDED
#define LOGIN_SESSION_VARIABLES_INCLUDED

// Forward declarations
class Login_session_variables {
};

// FIXME: MOVE DEFINITION TO .cc file
// FIXME: not authentication related so move to a different directory?

/*
  Handle the updating of login session variables with those from the user definition
*/

static void handle_login_session_variables(THD *thd) {
    // for debugging log something like reading in variables from mysql.user.User_attributes

// +----------------------------------------------------------------------+
// | User_attributes                                                      |
// +----------------------------------------------------------------------+
// | {"metadata": {"created_by": "badmin", "account_type": "dba"}}        |
// | {"metadata": {"created_by": "badmin"}}                               |
// | {"metadata": {"created_by": "GrantsAPI", "created_for": "rotation"}} |
// | {"metadata": {"created_by": "GrantsAPI"}}                            |
// sample configuration setting
// | {"Login_session_variables": {"wait_timeout": 600, "system_time_timezone": "UTC", "time_zone": "+00:00", "sql_require_primary_key": "OFF", "max_execution_time": 10}}, |
// +----------------------------------------------------------------------+
//
// see: bugs.mysql.com/105006
// We add "Login_session_variables"
// - sample values could be:
// "Login_session_variables": {   // upper case to match say "Password_locking"
//   "wait_timeout": 600,
//   "system_time_timezone": "UTC", // requires time zone data to be loaded into MySQL
//   "time_zone": "+00:00",
//   "sql_require_primary_key": "OFF",
//   "max_execution_time", 10,
// }
//
// CODE:
// ... goes here ...
//
// - logic structure is:
// - check if "login_session_variables" exists as key to User_attributes
//   - it not -> no handling
//   - iterate over each key / value
//     - check key is a string, if not skip and log the error
//     - DEBUG: log "key" / "value"
//     - try to set logically "key=value", handling "string", integer values. Other values?
//     - if the intent to update this fails log an issue, but continue
//
// - for error cases:
//   - consider logging being optional
//   - consider a counter of errors
//   - consider a counter of issues by username/key and only log once
}


#endif /* LOGIN_SESSION_VARIABLES_INCLUDED */
