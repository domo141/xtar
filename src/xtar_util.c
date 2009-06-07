/*
 * $Id; xtar_util.c $
 *
 * Author: Tomi Ollila -- too ät iki piste fi
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Sun 07 Jun 2009 21:44:50 EEST too
 * Last modified: Sun 07 Jun 2009 22:18:39 EEST too
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#include "xtar_util.h"

void vwarn(const char * format, va_list ap)
{
    int error = errno; /* XXX is this too late ? */

    vfprintf(stderr, format, ap);
    if (format[strlen(format) - 1] == ':')
	fprintf(stderr, " %s\n", strerror(error));
    else
	fputs("\n", stderr);
//    fflush(stderr);
}

void die(const char * format, ...)
{
    va_list ap;

    va_start(ap, format);
    vwarn(format, ap);
    va_end(ap);
    exit(1);
}

void warn(const char * format, ...)
{
    va_list ap;

    va_start(ap, format);
    vwarn(format, ap);
    va_end(ap);
}

void doparents(const char * name)
{
    // single element cache for last path...
    return;
}

void writefully(int fd, char * buf, int len)
{
    write(fd, buf, len); // XXX
}


/*
 * Local variables:
 * mode: c
 * c-file-style: "stroustrup"
 * tab-width: 8
 * End:
 * vi: set sw=8 ts=8:
 */
