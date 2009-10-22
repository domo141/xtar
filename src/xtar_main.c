/*
 * $Id; main.c $
 *
 * Author: Tomi Ollila -- too ät iki piste fi
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Fri 05 Jun 2009 15:56:03 EEST too
 * Last modified: Thu 22 Oct 2009 19:30:08 EEST too
 */

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "archive_platform.h"
#include "archive.h"
#include "archive_entry.h"

#include "xtar_util.h"

#include "relpath.h"

#if WIN32
#include "xtar_w32lib.h"
#endif

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
    size_t seekto;
    size_t filesize;
    size_t bytesread;
} G;

void init_G(const char * pn)
{
    memset(&G, 0, sizeof G);
    G.progname = pn;
}

#include "version.h"

static void usage(const char * format, ...)
{
    if (format) {
	va_list ap;
	fprintf(stderr, "\n%s (%s): ", G.progname, VERSION);
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fputc('\n', stderr);
    }
    fprintf(stderr, "\nUsage: %s [-C <dir>] [-S seekpos[:length]] [-n <file>] "
	    " [-l <file>] tarfile\n\n", G.progname);
    exit(1);
}

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
    char * seekarg = null;

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
	case 'S': seekarg = get_next_arg(&argc, &argv, ARGREQ("S")); break;
	default:
	    usage("%s: unknown option.", argv[0]);
	}
	argc--; argv++;
    }
    G.filename = argv[0];
    if (G.filename == null)
	usage("'tarfile' missing");

    if (seekarg) {
	char * p;

	G.seekto = strtol(seekarg, &p, 10);
	if (*p == ':')
	    G.filesize = strtol(p + 1, &p, 10);

	if (*p != '\0')
	    usage("'%s': illegal seek[/len] format", seekarg);
    }
    if (argc > 1)
	usage("too many arguments");
}

static void getfilesize(void)
{
    struct stat st;

    if (stat(G.filename, &st) < 0)
	die("stat('%s') failed:", G.filename);

    /* unfortunately st.st_size not unsigned... */
    if (G.filesize + G.seekto > st.st_size)
	die("Input file size %d smaller than size given for '-S'\n",
	    st.st_size);

    if (G.filesize == 0)
	G.filesize = st.st_size - G.seekto;
}

static const char * filename_rooted(const char * pathname, char buf[RP_BUFSIZ])
{
    pathname = cleandotrefs(pathname, buf);
    if (pathname == null)
	return null;
    if (pathname[0] == '/')
	pathname++;
    while (pathname[0] == '.' && pathname[1] == '.' &&  pathname[2] == '/')
	pathname += 3;
    return pathname;
}

static void extract_file(const char *, struct archive *,struct archive_entry *);
static void extract_dir(const char * name, struct archive_entry * entry);
static void extract_symlink(const char * name, struct archive_entry * entry);
static void extract_hardlink(const char * name, struct archive_entry * entry);

int main(int argc, char * argv[])
{
    init_G(argv[0]);
    argc--; argv++;

    handle_args(argc, argv);

    if (G.namefile) {
	G.namefh = fopen(G.namefile, "w");
	if (G.namefh == null)
	    die("fopen('%s') failed:", G.namefile);
    }
    if (G.linkfile) {
	G.linkfh = fopen(G.linkfile, "w");
	if (G.linkfh == null)
	    die("fopen('%s') failed:", G.linkfile);
    }

    struct archive * a;
    struct archive_entry * entry;
    int r, fd;

    a = archive_read_new();

    archive_read_support_format_tar(a);

    archive_read_support_compression_gzip(a);
    archive_read_support_compression_bzip2(a);

    /* read from stdin in case filename == '-' */
    if (G.filename != null && strcmp(G.filename, "-") == 0) {
	G.filename = null;
	if (G.seekto)
	    die("No seek supported when input from stdin\n");
    }
    else
	getfilesize();

#ifndef O_BINARY
#define O_BINARY 0
#endif
    if (G.filename) {
	fd = open(G.filename, O_RDONLY | O_BINARY);
	if (fd < 0)
	    die("Failed to open tar file '%s':");

	if (G.seekto) {
	    if (lseek(fd, G.seekto, SEEK_SET) < 0)
		die("seek('%d') failed:", G.seekto);
	}
    }
    else fd = 1;

    /*if ((r = archive_read_open_file(a, G.filename, 10240)) != 0) */
    if ((r = archive_read_open_fd(a, fd, 81920)) != 0)
	die("%s\n", archive_error_string(a));

    if (G.xdir) {
	if (chdir(G.xdir) < 0) // XXX check windows operation
	    die("chdir('%s') failed:", G.xdir);
    }

    for (;;)
    {
	const char * pathname;
	char buf[RP_BUFSIZ];
	int m;

	r = archive_read_next_header(a, &entry);
	if (r == ARCHIVE_EOF)
	    break;
	if (r != ARCHIVE_OK)
	    die("%s\n", archive_error_string(a));

	pathname = filename_rooted(archive_entry_pathname(entry), buf);
	m = archive_entry_mode(entry);

	if (pathname == null || *pathname == 0)
	    continue;

#if WIN32
	/* on windows, do not extract files with ':' in file path name */
	if (strchr(pathname, ':') != null)
	    continue;
#endif

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
    write(1, "\n", 1);
}

#ifndef O_BINARY
#define O_BINARY 0
#endif

static void extract_file(const char * name, struct archive * a,
			 struct archive_entry * entry)
{
    int perm;

    if (archive_entry_hardlink(entry)) {
	extract_hardlink(name, entry);
	return;
    }

    if (archive_entry_mode(entry) & S_IXUSR)
	perm = 0755;
    else
	perm = 0644;

    if (G.namefh) fprintf(G.namefh, "%s\n", name);
    doparents(name);
    int fd = open(name, O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, perm);
    if (fd < 0)
	die("open('%s') for writing:", name);

    /*char buff[4096]; */
    char buff[32768];
    int len = archive_read_data(a, buff, sizeof buff);
    while (len > 0) {
	writefully(fd, buff, len);
	len = archive_read_data(a, buff, sizeof buff);
    }
    if (len < 0)
	die("Error reading input archive: %s", archive_error_string(a));
    close(fd);
}

static void extract_dir(const char * name, struct archive_entry * entry)
{
    struct stat st;
    (void)entry;

    if (G.namefh) fprintf(G.namefh, "%s/\n", name);

    if (stat(name, &st) == 0) {
	if (! S_ISDIR(st.st_mode))
	    die("Path '%s' exists but is not a directory\n");
	return;
    }
    doparents(name);
#if 0
    if (stat(name, &st) == 0 && S_ISDIR(st.st_mode))
	return;
#endif
#if WIN32
    if (mkdir(name) < 0 && errno != EEXIST)
	die("mkdir('%s'):", name);
#else
    if (mkdir(name, 0755) < 0 && errno != EEXIST)
	die("mkdir('%s'):", name);
#endif
}

static void extract_symlink(const char * name, struct archive_entry * entry)
{
    if (G.namefh) fprintf(G.namefh, "%s@\n", name);
    if (G.linkfh) {
	const char * linkname = archive_entry_symlink(entry);
#if WIN32
	// hack for the usual case: path/to/FOO -> foo (caseinsensitive fs)
	const char * basename = strrchr(name, '/');
	if (basename == null) basename = name;
	if (strcasecmp(basename, linkname) != 0)
#endif
	    fprintf(G.linkfh, "%s -> %s\n", name, linkname);
    }
#if !WIN32
    else {
	const char * linkname;
	char buf[RP_BUFSIZ];

	linkname = relpath(name, archive_entry_symlink(entry), buf);
	if (linkname == null || *linkname == '\0')
	    return; /* XXX error message */

	doparents(name);
	if (symlink(linkname, name) != 0) {
	    if (errno == EEXIST)
		warn("Can not create symbolic link '%s' -> '%s': "
		     "destination file exists", name, linkname);
	    else
		die("symlink('%s', '%s'):", linkname, name);
	}
    }
#endif
}

static void extract_hardlink(const char * name, struct archive_entry * entry)
{
    const char * linkname;
    char buf[RP_BUFSIZ];

    if (G.namefh) fprintf(G.namefh, "%s#\n", name);
    linkname = filename_rooted(archive_entry_hardlink(entry), buf);
    if (linkname == null || *linkname == 0)
	return; /* XXX error message */
    doparents(name);

#if WIN32
    DWORD error = link_w32(linkname, name);
    if (error != 0) {
	if (error == ERROR_FILE_EXISTS)
	    warn("Can not create hard link '%s' => '%s': destination file exists",
		 name, linkname);
	else
	    die("link('%s', '%s'):", linkname, name);
    }
#else
    if (link(linkname, name) != 0) {
	if (errno == EEXIST)
	    warn("Can not create hard link '%s' => '%s': destination file exists",
		 name, linkname);
	else
	    die("link('%s', '%s'):", linkname, name);
    }
#endif
#if 0
    if (G.linkfh)
	fprintf(G.linkfh, "%s => %s\n", name, archive_entry_hardlink(entry));
#endif
}


/* ... */

static void output_dots(void)
{
    static int prevv = -1;
    static const char * nums = ".9876543210";

    int currv = G.bytesread / (G.filesize / 50);
    //printf("%d (%d %d)\n", l, prevv, currv);
    if (prevv != currv) {
	prevv = currv;
	if ( (currv % 5) == 0)
	    write(1, nums + currv / 5, 1);
	else
	    write(1, ".", 1);
    }
}

#undef read
ssize_t wrapped_read(int fd, void * buf, size_t len)
{
    size_t maxlen = G.filesize - G.bytesread;
    int l;
    if (maxlen == 0)
	return 0;
    l = read(fd, buf, len > maxlen? maxlen: len);

    G.bytesread += l;
    //printf("fd: %d, len %d -> %d\n", fd, len, l);
    if (G.filesize > 5 * 1000 * 1000 && l > 0)
	output_dots();
    return l;
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

