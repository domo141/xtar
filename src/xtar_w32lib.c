/*
 * $Id; xtar_w32lib.c $
 *
 * Author: Tomi Ollila -- too ät iki piste fi
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Mon 24 Aug 2009 19:43:56 EEST too
 * Last modified: Wed 20 Oct 2010 15:30:17 EEST too
 */

#include <stdio.h>

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

void w32vwarn(int error, const char * format, va_list ap)
{
    vfprintf(stderr, format, ap);
    if (format[strlen(format) - 1] == ':') {

	char * msgbuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		      FORMAT_MESSAGE_FROM_SYSTEM |
		      FORMAT_MESSAGE_IGNORE_INSERTS,
		      null, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		      &msgbuf, 0, null);
	fprintf(stderr, " %s\n", msgbuf);
    }
    else
	fputs("\n", stderr);
}

void w32die(int error, const char * format, ...)
{
    va_list ap;

    va_start(ap, format);
    w32vwarn(error, format, ap);
    va_end(ap);
    exit(1);
}
