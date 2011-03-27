
#ifndef SYSDEFS_WRAPPER_H
#define SYSDEFS_WRAPPER_H

#define HAVE_INTTYPES_H 1
#define HAVE_LIMITS_H 1

/* XXX problem with this in sysdefs_orig.h */

//typedef unsigned char _Bool;
#define HAVE__BOOL 1

#include "sysdefs_orig.h"

#endif /* SYSDEFS_WRAPPEER_H */
