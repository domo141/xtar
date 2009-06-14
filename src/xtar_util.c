/*
 * $Id; xtar_util.c $
 *
 * Author: Tomi Ollila -- too ät iki piste fi
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Sun 07 Jun 2009 21:44:50 EEST too
 * Last modified: Sun 14 Jun 2009 18:18:25 EEST too
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#include "xtar_util.h"

#define null ((void*)0)

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

/* based on create_dir() in archive_write_disk.c */
static void _create_dir(char * path)
{
    struct stat st;
    char * slash, * base;

    /* Check for special names and just skip them. */
    slash = strrchr(path, '/');
    if (slash == NULL)
	base = path;
    else
	base = slash + 1;

    if (base[0] == '\0' ||
	(base[0] == '.' && base[1] == '\0') ||
	(base[0] == '.' && base[1] == '.' && base[2] == '\0')) {
	/* Don't bother trying to create null path, '.', or '..'. */
	if (slash != NULL) {
	    *slash = '\0';
	    _create_dir(path);
	    *slash = '/';
	    return;
	}
	return;
    }

    /*
     * Yes, this should be stat() and not lstat().  Using lstat()
     * here loses the ability to extract through symlinks.  Also note
     * that this should not use the a->st cache.
     */
    if (stat(path, &st) == 0) {
	if (S_ISDIR(st.st_mode))
	    return;
	if (unlink(path) != 0)
	    die("Can not unlink '%s':", path);
    }
    else if (errno != ENOENT && errno != ENOTDIR) {
	/* Stat failed? */
	die("Can not test directory '%s':");
    } else if (slash != NULL) {
	*slash = '\0';
	_create_dir(path);
	*slash = '/';
    }

#if 0
    /*
     * Mode we want for the final restored directory.  Per POSIX,
     * implicitly-created dirs must be created obeying the umask.
     * There's no mention whether this is different for privileged
     * restores (which the rest of this code handles by pretending
     * umask=0).  I've chosen here to always obey the user's umask for
     * implicit dirs, even if _EXTRACT_PERM was specified.
     */
    mode_final = DEFAULT_DIR_MODE & ~a->user_umask;
    /* Mode we want on disk during the restore process. */
    mode = mode_final;
    mode |= MINIMUM_DIR_MODE;
    mode &= MAXIMUM_DIR_MODE;
    if (mkdir(path, mode) == 0) {
	if (mode != mode_final) {
	    le = new_fixup(a, path);
	    le->fixup |=TODO_MODE_BASE;
	    le->mode = mode_final;
	}
    }
#else
#if WIN32
#define mkdir(path) mkdir(path)
#else
#define mkdir(path) mkdir(path, 0755)
#endif
    if (mkdir(path) == 0)
	return;
#endif

    /*
     * Without the following check, a/b/../b/c/d fails at the
     * second visit to 'b', so 'd' can't be created.  Note that we
     * don't add it to the fixup list here, as it's already been
     * added.
     */
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
	return;

    die("Failed to create dir '%s':", path);
}

void doparents(const char * name)
{
    static char * lastpath; // also current;
    static int lastpathlen = 0;
    int pathlen;
    char * slash = strrchr(name, '/');

    if (slash == null)
	return;

    pathlen = slash - name;
    if (lastpathlen > pathlen && lastpath[pathlen] == '\0'
	&& memcmp(name, lastpath, pathlen) == 0)
	return;
    if (lastpathlen <= pathlen) {
	lastpath = (char *)realloc(lastpath, pathlen + 1);
	if (lastpath == null)
	    die("Out of Memory\n");
	lastpathlen = pathlen + 1;
    }
    memcpy(lastpath, name, pathlen);
    lastpath[pathlen] = 0;

    _create_dir(lastpath);
}

void writefully(int fd, char * buf, int len)
{
    int l;

    /* XXX should this check EINTR ??? */
    while ((l = write(fd, buf, len)) > 0) {
	if (l == len)
	    return;
	len -= l;
	buf += len;
    }
    if (len)
	die("write error:");
}

/*
 * Local variables:
 * mode: c
 * c-file-style: "stroustrup"
 * tab-width: 8
 * End:
 * vi: set sw=8 ts=8:
 */
