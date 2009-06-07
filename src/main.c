/*
 * $Id; main.c $
 *
 * Author: Tomi Ollila -- too ät iki piste fi
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Fri 05 Jun 2009 15:56:03 EEST too
 * Last modified: Sun 07 Jun 2009 13:24:04 EEST too
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "archive_platform.h"
#include "archive.h"
#include "archive_entry.h"

enum { false = 0, true = 1 };
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

void init_G(const char * pn)
{
    memset(&G, 0, sizeof G);
    G.progname = pn;
    //G.namefd = G.linkfd = -1;
}

// examples/untar.c
static void extract(const char *filename);

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

static void vwarn(const char * format, va_list ap)
{
    int error = errno; /* XXX is this too late ? */

    if (memcmp(format, "%C:", 3) == 0) {
	fputs(G.progname, stderr);
	format += 2;
    }
    vfprintf(stderr, format, ap);
    if (format[strlen(format) - 1] == ':')
	fprintf(stderr, " %s\n", strerror(error));
    else
	fputs("\n", stderr);
//    fflush(stderr);
}

static void die(const char * format, ...)
{
    va_list ap;

    va_start(ap, format);
    vwarn(format, ap);
    va_end(ap);
    exit(1);
}

static void warn(const char * format, ...)
{
    va_list ap;

    va_start(ap, format);
    vwarn(format, ap);
    va_end(ap);
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

    extract(G.filename); // will be included here ...
    return 0;
}

// not much resemblance left with examples/untar.c

static void extract_file(const char *, struct archive *,struct archive_entry *);
static void extract_dir(const char * name, struct archive_entry * entry);
static void extract_symlink(const char * name, struct archive_entry * entry);
static void extract_hardlink(const char * name, struct archive_entry * entry);

static void
extract(const char * filename)
{
	struct archive * a;
	struct archive_entry * entry;
	int r;

	a = archive_read_new();

	archive_read_support_format_tar(a);

	/*
	 * On my system, enabling other archive formats adds 20k-30k
	 * each.  Enabling gzip decompression adds about 20k.
	 * Enabling bzip2 is more expensive because the libbz2 library
	 * isn't very well factored.
	 */
	/* ^^^^ from original author (of libarchive) ^^^^ (goes away soon ?)*/
	archive_read_support_compression_gzip(a);
	archive_read_support_compression_bzip2(a);

	/* read from stdin in case filename == '-' */
	if (filename != null && strcmp(filename, "-") == 0)
		filename = null;
	if ((r = archive_read_open_file(a, filename, 10240)) != 0)
	    die("%s\n", archive_error_string(a));

	for (;;)
	{
	    r = archive_read_next_header(a, &entry);
	    if (r == ARCHIVE_EOF)
		break;
	    if (r != ARCHIVE_OK)
		die("%s\n", archive_error_string(a));

	    const char * pathname = archive_entry_pathname(entry);
	    int m = archive_entry_mode(entry);

	    if (AE_ISREG(m)) extract_file(pathname, a, entry);
	    else if (AE_ISDIR(m)) extract_dir(pathname, entry);
	    else if (AE_ISLNK(m)) extract_symlink(pathname, entry);
	    else if (AEX_ISHL(m)) extract_hardlink(pathname, entry);
	    else /* skipping */ { ; }
#if 0
	    warn("name: %s, mode: %x -- %s (%d)",
		 archive_entry_pathname(entry), m, s, m & AE_IFMT);
#endif
	}
	archive_read_close(a);
	archive_read_finish(a);
}

static void doparents(const char * name)
{
    // single element cache for last path...
    return;
}

static void writefully(int fd, char * buf, int len)
{
    write(fd, buf, len); // XXX
}

static void extract_file(const char * name, struct archive * a,
			 struct archive_entry * entry)
{
    doparents(name);
    int fd = open(name, O_WRONLY|O_CREAT|O_TRUNC, 0644); // XXX permissions
    if (fd < 0)
	die("open('%s') for writing:", name);

    char buff[4096];
    int len = archive_read_data(a, buff, sizeof buff);
    while (len > 0) {
	writefully(fd, buff, len);
	len = archive_read_data(a, buff, sizeof buff);
    }
    if (len < 0)
	die("Error reading input archive:");
}

static void extract_dir(const char * name, struct archive_entry * entry)
{
    struct stat st;

    if (stat(name, &st) != 0) {
	if (! S_ISDIR(st.st_mode))
	    die("Path '%s' exists but is not a directory\n");
	return;
    }
    doparents(name);
#if WIN32
    if (mkdir(name) < 0)
	die("mkdir('%s'):", name);
#else
    if (mkdir(name, 0755) < 0)
	die("mkdir('%s'):", name);
#endif
}

static void extract_symlink(const char * name, struct archive_entry * entry)
{
    doparents(name);
}

static void extract_hardlink(const char * name, struct archive_entry * entry)
{
    doparents(name);
}


#if 0
#if WIN32

/* unused, but defined functions -- maybe cleaned up later ? */
#if 0
int mkdev(void) { return -1; }
int minor(void) { return -1; }
int major(void) { return -1; }
#endif
#endif
#endif

