/*
   Copyright (c) 2007, 2023, Oracle and/or its affiliates.

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

#ifndef MY_BIT_INCLUDED
#define MY_BIT_INCLUDED

/**
  @file include/my_bit.h
  Some useful bit functions.
*/

#include <sys/types.h>

#include "my_config.h"
#include "my_inttypes.h"

extern const char _my_bits_nbits[256];
extern const uchar _my_bits_reverse_table[256];

static inline uint my_count_bits(ulonglong v) {
#if SIZEOF_LONG_LONG > 4
  /* The following code is a bit faster on 16 bit machines than if we would
     only shift v */
  const ulong v2 = (ulong)(v >> 32);
  return (uint)(uchar)(
      _my_bits_nbits[(uchar)v] + _my_bits_nbits[(uchar)(v >> 8)] +
      _my_bits_nbits[(uchar)(v >> 16)] + _my_bits_nbits[(uchar)(v >> 24)] +
      _my_bits_nbits[(uchar)(v2)] + _my_bits_nbits[(uchar)(v2 >> 8)] +
      _my_bits_nbits[(uchar)(v2 >> 16)] + _my_bits_nbits[(uchar)(v2 >> 24)]);
#else
  return (uint)(uchar)(
      _my_bits_nbits[(uchar)v] + _my_bits_nbits[(uchar)(v >> 8)] +
      _my_bits_nbits[(uchar)(v >> 16)] + _my_bits_nbits[(uchar)(v >> 24)]);
#endif
}

static inline uint my_count_bits_uint32(uint32 v) {
  return (uint)(uchar)(
      _my_bits_nbits[(uchar)v] + _my_bits_nbits[(uchar)(v >> 8)] +
      _my_bits_nbits[(uchar)(v >> 16)] + _my_bits_nbits[(uchar)(v >> 24)]);
}

static inline uint32 my_clear_highest_bit(uint32 v) {
  uint32 w = v >> 1;
  w |= w >> 1;
  w |= w >> 2;
  w |= w >> 4;
  w |= w >> 8;
  w |= w >> 16;
  return v & w;
}

static inline uint32 my_reverse_bits(uint32 key) {
  return (_my_bits_reverse_table[key & 255] << 24) |
         (_my_bits_reverse_table[(key >> 8) & 255] << 16) |
         (_my_bits_reverse_table[(key >> 16) & 255] << 8) |
         _my_bits_reverse_table[(key >> 24)];
}

#endif /* MY_BIT_INCLUDED */
