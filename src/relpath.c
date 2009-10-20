#if 0 /* -*- mode: c; c-file-style: "stroustrup"; tab-width: 8; -*-
 set -e; TRG=`basename $0 .c`; rm -f "$TRG"
 WARN="-Wall -Wno-long-long -Wstrict-prototypes -pedantic"
 WARN="$WARN -Wcast-align -Wpointer-arith " # -Wfloat-equal #-Werror
 WARN="$WARN -W -Wwrite-strings -Wcast-qual -Wshadow" # -Wconversion
 case $1 in '') set x -O2 ### set x -ggdb;
	shift ;; esac;
 set -x; ${CC:-gcc} $WARN "$@" -o "$TRG" "$0" -DRP_TEST -DCDATE="\"`date`\""
 exit $?
 */
#endif
/*
 * $Id; relpath.c $
 *
 * Author: Tomi Ollila  -- too ät iki piste fi
 *
 *	Copyright (c) 2009 Tomi Ollila
 *	    All rights reserved
 *
 * Created: Tue 20 Oct 2009 18:28:02 EEST too
 * Last modified: Tue 20 Oct 2009 22:57:29 EEST too
 */

/* See README_AND_COPYRIGHT for license information */

#include <stdio.h>
#include <string.h>

#define null 0

/* clear internal /./, // and /../ from t (if any) */
/* fixme: return int, false if illegal chars (so to be skipped). */
const char * cleandotrefs(const char * s, char * buf, int buflen)
{
    char * pd;
    int state, other;

    pd = buf;

    state = (s[0] == '.')? 1: 0;
    other = 0;
    while (*s)
    {
	switch (state)
	{
	case -1:
	    other++;
	    state = 0;
	case 0: if (*s == '/') state = 1;
	    break;
	case 1:
	    if (*s == '.') { state = 2; break; }
	    if (*s == '/') pd--; else state = -1;
	    break;
	case 2:
	    if (*s == '/') { state = 1; pd -= 2; break; }
	    if (*s == '.') state = 3; else state = -1;
	    break;
	case 3:
	    if (*s != '/') { state = -1; break; }
	    state = 1;
	    if (other) {
		other--;
		pd -= 4;
		while (*pd != '/')
		    pd--;
	    }
	    break;
	}
	if (pd - buf >= buflen)
	    return null;
	*pd++ = *s++;
    }
    *pd = '\0';
    return buf;
}

#if !WIN32 /* Not needed for w32 system (for now) */

char * relpath(const char * f, char * t)
{
    char tbuf[4096];
    t = cleandotrefs(t, tbuf, 4096);

    if (t[0] == '/') {
	static char relpath[4096];
	const char * p;
	char * rpp;
	int l;
	/* absolute part */
	t++;
	while (t[0] == '.' && t[1] == '.' && t[2] == '/')
	    t+= 3;

	t[-1] = '\0';
	/* search common prefix... */
	while (*f == *t ) {
	    f++; t++;
	}
	f--; t--;
	/* up to last '/' */
	while (*t != '/' && *t != '\0') {
	    f--; t--;
	}
	f++; t++;

	rpp = relpath;
	rpp[0] = '\0';
	p = f;
	//printf("xxx %s\n", p);
	while ((p = strchr(p, '/')) != null) {
	    if (rpp - relpath < sizeof relpath - 20) {
		rpp[0] = '.'; rpp[1] = '.'; rpp[2] = '/'; rpp[3] = '\0';
		rpp += 3;
	    }
	    p++;
	}
	l = strlen(t);
	if ( (rpp - relpath) + l < sizeof relpath - 2)
	    memcpy(rpp, t, l + 1);
	else
	    strcpy(rpp, "junk"); /* XXX */

	return relpath;
    }
    else {
	/* check that ../../.. is no deeper than 'f' path */
	int i = 0;
	char * x = t;
	while (x[0] == '.' && x[1] == '.' && x[2] == '/')
	    x+= 3;
	if (x != t) {
	    const char * p = f;
	    while ((p = strchr(p, '/')) != null) {
		i++; p++;
	    }
	    //printf("%s %d\n", f, i);
	    i -= ((x - t) / 3);
	    while (i < 0) {
		t += 3; i++;
	    }
	}
	return t;
    }
}

#endif /* not WIN32 */

#if RP_TEST

void tst(const char * str)
{
    char buf[1024];
    strcpy(buf, str);

    printf("%s ->", buf);
    printf(" %s\n", relpath("lib/bar/baz/bip", buf));
}

int main(int argc, char * argv[])
{
    tst("../lib/libXrender.la");
    tst("../..//lib/./libXrender.la");
    tst("../../lib/foo/../libXrender.la");
    tst("../../lib/../../libXrender.la");
    tst("../../lib/../../../libXrender.la");
    tst("../../../lib/..//.//../libXrender.la");
    tst("./../../lib/libXrender.la");
    tst("/../../lib/libXrender.la");
    tst(".///./../../lib/libXrender.la");
    return 0;
}
#endif
