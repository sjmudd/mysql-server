/*
   Copyright (c) 2010, 2021, Oracle and/or its affiliates.

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
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef SYNC_HPP
#define SYNC_HPP

#define JAM_FILE_ID 31


struct SyncReq
{
  Uint32 senderRef;
  Uint32 senderData;
  Uint32 prio;

  STATIC_CONST( SignalLength = 3 );
};

struct SyncRef
{
  Uint32 senderRef;
  Uint32 senderData;
  Uint32 errorCode;

  STATIC_CONST( SignalLength = 3 );

  enum ErrorCode
  {
    SR_OK = 0,
    SR_OUT_OF_MEMORY = 1
  };
};

struct SyncConf
{
  Uint32 senderRef;
  Uint32 senderData;

  STATIC_CONST( SignalLength = 3 );
};

struct SyncPathReq
{
  Uint32 senderData;
  Uint32 prio;
  Uint32 count;
  Uint32 pathlen;
  Uint32 path[1];

  STATIC_CONST( SignalLength = 4 );
  STATIC_CONST( MaxPathLen = 25 - SignalLength );
};

struct SyncPathConf
{
  Uint32 senderData;
  Uint32 count;

  STATIC_CONST( SignalLength = 2 );
};


#undef JAM_FILE_ID

#endif
