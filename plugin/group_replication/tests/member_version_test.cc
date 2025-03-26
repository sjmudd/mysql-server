/* Copyright (c) 2025, Simon J Mudd <sjmudd@pobox.com>.

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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

void test_gr_primary_min_equivalent_version() {
    // Test 8.0.x versions
    assert(gr_primary_min_equivalent_version(Member_version(0x080022)) == 0x080022);  // 8.0.34
    assert(gr_primary_min_equivalent_version(Member_version(0x080023)) == 0x080023);  // 8.0.35
    assert(gr_primary_min_equivalent_version(Member_version(0x080024)) == 0x080023);  // 8.0.36 -> 8.0.35
    assert(gr_primary_min_equivalent_version(Member_version(0x080028)) == 0x080023);  // 8.0.40 -> 8.0.35

    // Test 8.4.x versions
    assert(gr_primary_min_equivalent_version(Member_version(0x080400)) == 0x080400);  // 8.4.0
    assert(gr_primary_min_equivalent_version(Member_version(0x080401)) == 0x080400);  // 8.4.1 -> 8.4.0
    assert(gr_primary_min_equivalent_version(Member_version(0x08040A)) == 0x080400);  // 8.4.10 -> 8.4.0

    // Test 9.x.x versions
    assert(gr_primary_min_equivalent_version(Member_version(0x090000)) == 0x090000);  // 9.0.0
    assert(gr_primary_min_equivalent_version(Member_version(0x090100)) == 0x090100);  // 9.1.0
    assert(gr_primary_min_equivalent_version(Member_version(0x090101)) == 0x090100);  // 9.1.1 -> 9.1.0
    
    // Test boundary cases
    assert(gr_primary_min_equivalent_version(Member_version(0x050709)) == 0x050709);  // 5.7.9
    assert(gr_primary_min_equivalent_version(Member_version(0x070000)) == 0x070000);  // 7.0.0
    assert(gr_primary_min_equivalent_version(Member_version(0x0A0000)) == 0x0A0000);  // 10.0.0
}
