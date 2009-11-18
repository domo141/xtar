#if 0 /* -*- mode: c; c-file-style: "stroustrup"; tab-width: 8; -*-
 set -e; TRG=`basename $0 .c`; rm -f "$TRG"
 WARN="-Wall -Wno-long-long -Wstrict-prototypes -pedantic"
 WARN="$WARN -Wcast-align -Wpointer-arith " # -Wfloat-equal #-Werror
 WARN="$WARN -W -Wwrite-strings -Wcast-qual -Wshadow" # -Wconversion
 case $1 in '') set x -O2 -ggdb;
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
 * Last modified: Wed 18 Nov 2009 20:15:35 EET too
 */

/* See README_AND_COPYRIGHT for license information */

#include <stdio.h>
#include <string.h>

#include "relpath.h"
#define null 0

/* clear internal /./, // and /../ from s (if any) */
const char * cleandotrefs(const char * s, char obuf[RP_BUFSIZ])
{
    char * pd = obuf;
    int state, other;
    char c;

    state = (s[0] == '.')? 1: (s[0] == '/')? 0: -1;
    other = 0;
    while ( (c = *s++) != '\0')
    {
	/* fixme: if illegal chars found, return null */
	switch (state)
	{
	case -1:
	    other++;
	    state = 0;
	case 0: if (c == '/') state = 1;
	    break;
	case 1:
	    if (c == '.') { state = 2; break; }
	    if (c == '/') continue /* while */; else state = -1;
	    break;
	case 2:
	    if (c == '/') { state = 1; pd -= 1; continue /* while */; }
	    if (c == '.') state = 3; else state = -1;
	    break;
	case 3:
	    if (c != '/') { state = -1; break; }
	    state = 1;
	    if (other) {
		other--;
		pd -= 4;
		while (pd != obuf && *pd != '/')
		    pd--;
	    }
	    break;
	}
	if (pd - obuf >= RP_BUFSIZ)
	    return null;
	*pd++ = c;
    }
    *pd = '\0';
    return obuf;
}

#if !WIN32 /* Not needed for w32 system (for now) */

const char * relpath(const char * f, const char * t, char obuf[RP_BUFSIZ])
{
    if (t[0] == '/') {
	/* absolute part */
	const char * p;
	char * rpp;
	int l;
	char tbuf[RP_BUFSIZ];
	t = cleandotrefs(t, tbuf);

	t++;
	while (t[0] == '.' && t[1] == '.' && t[2] == '/')
	    t += 3;

	/* search common prefix... */
	while (*f && *f == *t ) {
	    f++; t++;
	}
	f--; t--;
	/* back to last '/' */
	while (*t != '/') {
	    f--; t--;
	}
	f++; t++;

	rpp = obuf;
	rpp[0] = '\0';
	p = f;
	/*printf("xxx %s\n", p);*/
	while ((p = strchr(p, '/')) != null) {
	    if (rpp - obuf < RP_BUFSIZ - 20) {
		rpp[0] = '.'; rpp[1] = '.'; rpp[2] = '/'; rpp[3] = '\0';
		rpp += 3;
	    }
	    p++;
	}
	l = strlen(t);
	if ( (rpp - obuf) + l < RP_BUFSIZ - 2)
	    memcpy(rpp, t, l + 1);
	else
	    strcpy(rpp, "junk"); /* XXX */

	/*printf("xxx %s\n", obuf);*/
	return obuf;
    }
    else {
	/* check that ../../.. is no deeper than 'f' path */
	int i = 0;
	const char * x;

	x = t = cleandotrefs(t, obuf);
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
    char obuf[RP_BUFSIZ];

    printf("%s -> ", str);
    printf("%s\n", relpath("lib3/bar/baz/bip", str, obuf));
}

int main(int argc, char * argv[])
{
    tst("../lib1/libXrender.la");
    tst("../..//lib2/./libXrender.la");
    tst("../../lib3/foo/../libXrender.la");
    tst("../../lib4/../../libXrender.la");
    tst("../../lib5/../../../libXrender.la");
    tst("../../../lib6/..//.//../libXrender.la");
    tst("./../../lib7/libXrender.la");
    tst("/../../lib8/libXrender.la");
    tst(".///./../../lib9/libXrender.la");
    return 0;
}
#endif
