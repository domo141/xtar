/*
 * $Id; xtar_w32lib.c $
 *
 * Author: Tomi Ollila -- too ät iki piste fi
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Mon 24 Aug 2009 19:43:56 EEST too
 * Last modified: Wed 26 Aug 2009 10:32:03 EEST too
 */

#include "xtar_w32lib.h"
#include "xtar_util.h"

#ifndef null
#define null 0
#endif

static HMODULE kernel32dll = null;
static FARPROC CreateHardLink = null;

static void _loadw32dll(void)
{
    kernel32dll = LoadLibrary("KERNEL32");
    if (kernel32dll) return;
    die("kernel32 dll not found");
}
static inline void loadw32dll(void) {
    if (! kernel32dll) _loadw32dll();
}

static void _getCreateHardLink(void)
{
    CreateHardLink = GetProcAddress(kernel32dll, "CreateHardLinkA");
    if (CreateHardLink) return;
    die("CreateHardLink() function not found");
}
static inline void getCreateHardLink(void) {
    if (! CreateHardLink) _getCreateHardLink();
}

DWORD link_w32 (const char * from, const char * to)
{
  loadw32dll();
  getCreateHardLink();
  if (CreateHardLink(to, from, null) == 0)
      return GetLastError();
  return 0;
}
