
CC_NATIVE = gcc
CC=	gcc

.SUFFIXES: # none

LA_OBJ = archive_check_magic.o archive_entry.o archive_read.o \
	archive_read_open_fd.o archive_read_open_filename.o \
	archive_read_support_format_tar.o archive_string.o \
	archive_string_sprintf.o archive_util.o archive_virtual.o \
	archive_read_support_compression_gzip.o \
	archive_read_support_compression_bzip2.o

BZ2_OBJ = bzlib_partial.o crctable.o decompress.o huffman.o randtable.o
Z_OBJ = inflate.o inffast.o inftrees.o crc32.o adler32.o zutil.o

OBJ = main.o $(LA_OBJ) $(BZ2_OBJ) $(Z_OBJ) 
OBJ_DYN = main.o $(LA_OBJ)

OBJ_UX = $(OBJ:%=obj_ux/%)
OBJ_UX_DYN = $(OBJ_DYN:%=obj_ux/%)

TRG_UX = xtar
TRG_UX_DYN = xtar_dynamic

all: $(TRG_UX)
.PHONY: all clean distclean

TRGS_ALL = $(TRG_UX) $(TRG_UX_DYN)

LFLAGS=-s
LFLAGS_DYN=-s -lz -lbz2

$(TRG_UX): $(OBJ_UX)
	$(CC) -o $@ $(LFLAGS) $(OBJ_UX)

$(TRG_UX_DYN): $(OBJ_UX_DYN)
	$(CC) -o $@ $(LFLAGS_DYN) $(OBJ_UX_DYN)

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

CFLAGS=-O2 -I. -I..
C99FLAGS=-std=gnu99

OPTS= $(CFLAGS) $(C99FLAGS) $(WOPTS)

obj_ux/%.o: %.c
	$(CC) -o $@ -c $< $(OPTS)

obj_ux/%.o: archive/%.c
	$(CC) -o $@ -c $< $(OPTS)

obj_ux/%.o: bzip2/%.c
	$(CC) -o $@ -c $< $(OPTS)

obj_ux/%.o: zlib/%.c
	$(CC) -o $@ -c $< $(OPTS)

clean:
	rm -rf obj_ux *~

distclean: clean
	rm -f $(TRGS_ALL)
