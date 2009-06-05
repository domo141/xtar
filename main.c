/*
 * $Id; main.c $
 *
 * Author: Tomi Ollila -- too ät iki piste fi
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Fri 05 Jun 2009 16:56:03 EEST too
 * Last modified: Fri 05 Jun 2009 19:11:54 EEST too
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "archive_platform.h"
#include "archive.h"
#include "archive_entry.h"

#define null ((void *)0)

struct {
    const char * progname;
    const char * filename;
    const char * xdir;
    const char * namefile;
    const char * linkfile;
    FILE * namefh;
    FILE * linkfh;
} G;

init_G(const char * pn)
{
    memset(&G, 0, sizeof G);
    G.progname = pn;
    //G.namefd = G.linkfd = -1;
}

// examples/untar.c
static void extract(const char *filename, int do_extract, int flags);

static void usage(const char * format, ...)
{
    if (format) {
	va_list ap;
	fprintf(stderr, "\n%s: ", G.progname);
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fputc('\n', stderr);
    }
    fprintf(stderr, "\nUsage: %s [-C <dir>] [-n <file>] [-l <file>]\n\n",
	    G.progname);
    exit(1);
}

#define d0(x) do {} while (0)

static char * get_next_arg(int * c, char *** v, const char * umsg)
{
    if ((*v)[1] == null)
	usage(umsg);

    (*v)++; (*c)--;

    d0(("nxt arg: %s, left %d", **v, *c));

    return **v;
}

static void handle_args(int argc, char ** argv)
{
    while (argv[0] && argv[0][0] == '-'
	   && argv[0][1] != '\0'
	   && (argv[0][1] == '-' || argv[0][2] == '\0'))
    {
	d0(("arg: %s", *argvp[0]));

	switch (argv[0][1])
	{
#define ARGREQ(o) "option requires an argument -- " o
	case 'C': G.xdir = get_next_arg(&argc, &argv, ARGREQ("C")); break;
	case 'n': G.namefile = get_next_arg(&argc, &argv, ARGREQ("n")); break;
	case 'l': G.linkfile = get_next_arg(&argc, &argv, ARGREQ("l")); break;
	default:
	    usage("%s: unknown option.", argv[0]);
	}
	argc--; argv++;
    }
    G.filename = argv[0];
    if (G.filename == null)
	usage("filename missing");

    if (argc > 1)
	usage("Too many arguments");
}

int main(int argc, char * argv[])
{
    init_G(argv[0]);
    argc--; argv++;

    handle_args(argc, argv);

    extract(G.filename, 1, 0);
    return 0;
}

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

	archive_read_support_compression_gzip(a);
	archive_read_support_compression_bzip2(a);

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
