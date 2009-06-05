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
 * $Id; main.c $
 *
 * Author: Tomi Ollila <>
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	Copyright (c) 2009
 *	    All rights reserved
 *
 * Created: Fri 05 Jun 2009 15:46:03 EEST too
 * Last modified: Fri 05 Jun 2009 15:53:40 EEST too
 */

#include "archive_platform.h"
#include "archive.h"


int main(int argc, char * argv[])
{
    return 0;
}


int archive_read_close(struct archive *a)
{
    return 0; //((a->vtable->archive_close)(a)); // XXX
}


