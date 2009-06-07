/*
 * $Id; archive_platform.h $
 *
 * Author: Tomi Ollila <>
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	Copyright (c) 2009
 *	    All rights reserved
 *
 * Created: Wed 27 May 2009 20:48:29 EEST too
 * Last modified: Sun 07 Jun 2009 10:34:15 EEST too
 */

/* stuff from original archive_platform.h, with extras and tunes */

#ifndef ARCHIVE_PLATFORM_H
#define ARCHIVE_PLATFORM_H

#include <inttypes.h>
#include <stdint.h>

#include <limits.h> /* for INT_MAX */

#define HAVE_ERRNO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_WCHAR_H 1
#define HAVE_FCNTL_H 1

#define HAVE_WCSCPY 1
#define HAVE_WCSLEN 1
#define HAVE_WMEMCMP 1
#define HAVE_WMEMCPY 1


#define HAVE_ZLIB_H 1
#define HAVE_BZLIB_H 1


#define ARCHIVE_ERRNO_MISC 1
#define ARCHIVE_ERRNO_FILE_FORMAT 2
#define ARCHIVE_ERRNO_PROGRAMMER 3

/* archive.h and archive_entry.h require this. */
#define __LIBARCHIVE_BUILD 1

/* Just leaving this macro replacement empty leads to a dangling semicolon. */
#define __FBSDID(a) struct _undefined_hack


/* file type macros... */

#define AE_ISREG(m) (((m) & AE_IFMT) == AE_IFREG)
#define AE_ISLNK(m) (((m) & AE_IFMT) == AE_IFLNK)
#define AE_ISDIR(m) (((m) & AE_IFMT) == AE_IFDIR)
//#define AE_IS...(m) (((m) & AE_IFMT) == AE_IF...)

/* hard link */
#define AEX_ISHL(m) (((m) & AE_IFMT) == 0)

#if WIN32
/* w32 haxes & tunes */

typedef unsigned int uid_t;
typedef unsigned int gid_t;

/* Replacement for major/minor/makedev. */  // from archive_windows.h
#define major(x) ((int)(0x00ff & ((x) >> 8)))
#define minor(x) ((int)(0xffff00ff & (x)))
#define mkdev(maj,min) ((0xff00 & ((maj)<<8))|(0xffff00ff & (min)))

#endif /* WIN32 */



#endif /* ARCHIVE_PLATFORM_H */
