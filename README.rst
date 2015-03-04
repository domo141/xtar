xtar
====

xtar is self-contained (compressed) tar file extractor with
progress indication and all file paths converted to relative
inside extraction directory (absolute symlinks, and /../ components
do not leak outside extraction directory tree).

File decompression and tar extraction support comes from selected parts
of imported source files. See the following files for the copyright
information of these parts:

- libarchive-2.8.4-COPYING (tar formats)
- bzip2-1.0.5-LICENSE (bzip2 decompression)
- zlib-1.2.3-README (gzip decompression)
- xz-5.0.1-COPYING (xz/lzma decompression)

This software is known to compile on Linux, MacOS X and Windows (XP+). If
this doesn't compile on some other Unix(-like) systems porting should not
be difficult.

Copyright for the other parts of this sofware the following (Simplified
BSD) License applies
::

   Copyright (c) 2008 - 2015 Tomi Ollila
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer
      in this position and unchanged.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

| Tomi Ollila, too ät iki piste fi
| xtar version 2.2 2011-04-15
