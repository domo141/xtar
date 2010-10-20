#if 0 /* -*- mode: c; c-file-style: "stroustrup"; tab-width: 8; -*-
 set -e; TRG=`basename $0 .c`; rm -f "$TRG"
 WARN="-Wall -Wno-long-long -Wstrict-prototypes -pedantic"
 WARN="$WARN -Wcast-align -Wpointer-arith " # -Wfloat-equal #-Werror
 WARN="$WARN -W -Wwrite-strings -Wcast-qual -Wshadow" # -Wconversion
 case $1 in '') set x -O2 ### set x -ggdb;
	shift ;; esac;
 set -x; ${CC:-gcc} $WARN "$@" -o "$TRG" "$0" -DCDATE="\"`date`\""
 exit $?
 */
#endif
/*
 * $Id; xtar_w32lib.h $
 *
 * Author: Tomi Ollila  -- too ät iki piste fi
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Tue 25 Aug 2009 19:00:20 EEST too
 * Last modified: Wed 20 Oct 2010 15:19:58 EEST too
 */

#ifndef XTAR_W32LIB_H
#define XTAR_W32LIB_H

#include <windows.h>

DWORD link_w32 (const char * from, const char * to);

void w32vwarn(int error, const char * format, va_list ap);
void w32die(int error, const char * format, ...);



#endif /* XTAR_W32LIB_H */
