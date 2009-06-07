/*
 * $Id; xtar_util.h $
 *
 * Author: Tomi Ollila -- too ät iki piste fi
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Sun 07 Jun 2009 22:04:14 EEST too
 * Last modified: Sun 07 Jun 2009 22:12:56 EEST too
 */

#ifndef XTAR_UTIL_H
#define XTAR_UTIL_H

#include <stdarg.h>

#define d0(x) do {} while (0)

void vwarn(const char * format, va_list ap);
void die(const char * format, ...);
void warn(const char * format, ...);

void doparents(const char * name);

void writefully(int fd, char * buf, int len);

#endif /* XTAR_UTIL_H */

/*
 * Local variables:
 * mode: c
 * c-file-style: "stroustrup"
 * tab-width: 8
 * End:
 * vi: set sw=8 ts=8:
 */
