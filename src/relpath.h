
#ifndef RELPATH_H
#define RELPATH_H

#include <stdlib.h>
//#define RP_BUFSIZ PATH_MAX
#define RP_BUFSIZ 4096

const char * cleandotrefs(const char * s, char obuf[RP_BUFSIZ]);
const char * relpath(const char * f, const char * t, char obuf[RP_BUFSIZ]);

#endif /* RELPATH_H */
