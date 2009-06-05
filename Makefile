
CC_NATIVE = gcc
CC=	gcc

.SUFFIXES: # none

LA_OBJ = archive_check_magic.o archive_entry.o archive_read.o \
	archive_read_open_fd.o archive_read_open_filename.o \
	archive_read_support_format_tar.o archive_string.o \
	archive_string_sprintf.o archive_util.o archive_virtual.o \
	archive_read_support_compression_gzip.o \
	archive_read_support_compression_bzip2.o

OBJ = main.o $(LA_OBJ)

OBJ_UX = $(OBJ:%=obj_ux/%)

TRG_UX = xtar

all: $(TRG_UX)
.PHONY: all

TRGS_ALL = $(TRG_UX)

$(TRG_UX): $(OBJ_UX)
	$(CC) -o $@ $(LFLAGS) $(OBJ_UX)

# dependencies not exact, just to help compiling sometimes.
# enter make distclean all for 
$(OBJ_UX): obj_ux/.dirstamp Makefile archive_platform.h

obj_ux/.dirstamp:
	test -d obj_ux || mkdir obj_ux
	touch $@

# To less warnings, enter 'make <target> WARN1='
WARN0=	-Wall -Wstrict-prototypes -pedantic -Wno-long-long \
	-Wcast-align -Wpointer-arith #-Wfloat-equal #-Werror
WARN1?= -W -Wwrite-strings -Wcast-qual -Wshadow #-Wconversion

WOPTS=	$(WARN0) $(WARN1)

CFLAGS=-O2
C99FLAFGS=-std=gnu99

LFLAGS=-s -lz -lbz2

OPTS= $(CFLAGS) $(C99FLAGS) $(WOPTS)

obj_ux/%.o: %.c
	$(CC) -o $@ -c $< $(OPTS)

clean:
	rm -rf obj_ux *~

distclean: clean
	rm -f $(TRGS_ALL)
