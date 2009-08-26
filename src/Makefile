#!/usr/bin/env make
# $Id; Makefile $
#
# Author: Tomi Ollila -- too ät iki piste fi
#
#	Copyright (c) 2009 Tomi Ollila
#	    All rights reserved
#
# Created: Fri 05 Jun 2009 16:10:12 EEST too
# Last modified: Tue 25 Aug 2009 19:12:16 EEST too

CC = /bin/false
CC_UX =	gcc
CC_W32= gcc

.SUFFIXES: # none

LA_OBJ = archive_check_magic.o archive_entry.o archive_read.o \
	archive_read_open_fd.o archive_read_open_filename.o \
	archive_read_support_format_tar.o archive_string.o \
	archive_string_sprintf.o archive_util.o archive_virtual.o \
	archive_read_support_compression_gzip.o \
	archive_read_support_compression_bzip2.o

BZ2_OBJ = bzlib_partial.o crctable.o decompress.o huffman.o randtable.o
Z_OBJ = inflate.o inffast.o inftrees.o crc32.o adler32.o zutil.o

OBJ = xtar_main.o xtar_util.o $(LA_OBJ) $(BZ2_OBJ) $(Z_OBJ) 
OBJ_DYN = xtar_main.o xtar_util.o $(LA_OBJ)

OBJ_UX = $(OBJ:%=obj_ux/%)
OBJ_UX_DYN = $(OBJ_DYN:%=obj_ux/%)
OBJ_W32 = $(OBJ:%=obj_w32/%) obj_w32/xtar_w32lib.o

TRG_UX = xtar
TRG_UX_DYN = xtar_dynamic
TRG_W32 = xtar.exe

$(TRG_UX):	CC = $(CC_UX)
$(TRG_UX_DYN):	CC = $(CC_UX)
$(TRG_W32):	CC = $(CC_W32)

all: $(TRG_UX) always

TRGS_ALL = $(TRG_UX) $(TRG_UX_DYN) $(TRG_W32)

LFLAGS = -s
LFLAGS_DYN = -s -lz -lbz2

$(TRG_UX): $(OBJ_UX)
	$(CC) -o $@ $(LFLAGS) $(OBJ_UX)

$(TRG_UX_DYN): $(OBJ_UX_DYN)
	$(CC) -o $@ $(LFLAGS_DYN) $(OBJ_UX_DYN)

$(TRG_W32): $(OBJ_W32)
	file $< | grep COFF >/dev/null
	$(CC) -o $@ $(LFLAGS) $(OBJ_W32)

# Dependencies not exact, just to help compiling sometimes.
# Enter make distclean all to be sure. 
$(OBJ_UX): obj_ux/.dirstamp Makefile archive_platform.h
$(OBJ_W32): obj_w32/.dirstamp Makefile archive_platform.h

obj_ux/.dirstamp:
	test -d obj_ux || mkdir obj_ux
	touch $@

obj_w32/.dirstamp:
	test -d obj_w32 || mkdir obj_w32
	touch $@


# To less warnings, enter 'make <target> WARN1='
WARN0=	-Wall -Wstrict-prototypes -pedantic -Wno-long-long \
	-Wcast-align -Wpointer-arith #-Wfloat-equal #-Werror
WARN1?= -W -Wwrite-strings -Wcast-qual -Wshadow #-Wconversion

WOPTS=	$(WARN0) $(WARN1)

CFLAGS=-O2 -I. -I..
C99FLAGS=-std=gnu99

OPTS= $(CFLAGS) $(C99FLAGS) $(WOPTS)

# --8<----8<----8<----8<----8<--

obj_ux/%.o: %.c
	$(CC) -o $@ -c $< $(OPTS)

obj_ux/%.o: archive/%.c
	$(CC) -o $@ -c $< $(OPTS)

obj_ux/%.o: bzip2/%.c
	$(CC) -o $@ -c $< $(OPTS)

obj_ux/%.o: zlib/%.c
	$(CC) -o $@ -c $< $(OPTS)

# --8<----8<----8<----8<----8<--

obj_w32/%.o: %.c
	$(CC) -o $@ -c $< $(OPTS)

obj_w32/%.o: archive/%.c
	$(CC) -o $@ -c $< $(OPTS)

obj_w32/%.o: bzip2/%.c
	$(CC) -o $@ -c $< $(OPTS)

obj_w32/%.o: zlib/%.c
	$(CC) -o $@ -c $< $(OPTS)

# --8<----8<----8<----8<----8<--

clean: always
	rm -rf obj_ux obj_w32 *~

distclean: clean
	rm -f $(TRGS_ALL)

.PHONY: always
