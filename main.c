/*
 * $Id; main.c $
 *
 * Author: Tomi Ollila <>
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Fri 05 Jun 2009 16:16:03 EEST too
 * Last modified: Fri 05 Jun 2009 16:50:31 EEST too
 */

#include <string.h>
#include <stdlib.h>

#include "archive_platform.h"
#include "archive.h"
#include "archive_entry.h"

// examples/untar.c
static void extract(const char *filename, int do_extract, int flags);

int main(int argc, char * argv[])
{
    (void)argc; (void)argv;
    //extract("foo.tar", 1, 0);
    extract("bzip2-1.0.5.tar.gz", 1, 0);
    return 0;
}

#if 0
int archive_read_close(struct archive *a)
{
    (void)a;
    return 0; //((a->vtable->archive_close)(a)); // XXX
}
#endif


// taken examples/untar.c, then stripped & modified

#define errmsg puts
#define verbose 1
#define msg puts

static void
extract(const char *filename, int do_extract, int flags)
{
	struct archive *a;
	struct archive_entry *entry;
	int r;

	(void) flags; // XXX to be removed ?

	a = archive_read_new();

	archive_read_support_format_tar(a);
	/*
	 * On my system, enabling other archive formats adds 20k-30k
	 * each.  Enabling gzip decompression adds about 20k.
	 * Enabling bzip2 is more expensive because the libbz2 library
	 * isn't very well factored.
	 */
	if (filename != NULL && strcmp(filename, "-") == 0)
		filename = NULL;
	if ((r = archive_read_open_file(a, filename, 10240))) {
		errmsg(archive_error_string(a));
		errmsg("\n");
		exit(r);
	}
	for (;;) {
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r != ARCHIVE_OK) {
			errmsg(archive_error_string(a));
			errmsg("\n");
			exit(1);
		}
		if (verbose && do_extract)
			msg("x ");
		if (verbose || !do_extract)
			msg(archive_entry_pathname(entry));
#if 0
		if (do_extract) {
			r = archive_write_header(ext, entry);
			if (r != ARCHIVE_OK)
				errmsg(archive_error_string(a));
			else
				copy_data(a, ext);
		}
#endif
		if (verbose || !do_extract)
			msg("\n");
	}
	archive_read_close(a);
	archive_read_finish(a);
	exit(0);
}
