
# 'all' -target will rebuild everything from scratch (to be sure)

VERSION = 1.3
VERDATE = 2010-09-11

.NOTPARALLEL:

all:	src/version.h
	@sed -n '/^all.sh:/,/^ *$$/ p' Makefile | tail -n +3 | sh -eux

all.sh:
	exit 1 # this target is not to be run.
	goals=xtar
	trap 'rm -f foo.c a.out' 0
	echo 'int main(void) { }' > foo.c
	gcc foo.c -lz -lbz2 && goals=$goals\ xtar_dynamic || true
	rm -f foo.c a.out; trap - 0
	CC_W32=	
	cc_w32=`env which i686-pc-mingw32-gcc i586-mingw32msvc-gcc | head -1`
	case $cc_w32 in /*)	goals=$goals\ xtar.exe	
				CC_W32=CC_W32=`basename "$cc_w32"` ;; esac
	make -C src $goals $CC_W32


src/version.h: Makefile
	echo '#define VERSION "$(VERSION)":#define VERDATE "$(VERDATE)"' \
		| tr : \\012 > $@

README_AND_COPYRIGHT: Makefile
	echo '/^xtar version /s/.*/xtar version $(VERSION) $(VERDATE)/:wq:' \
		| tr : \\012 | ed $@

install: all
	@case "$(PREFIX)" in \
		'') echo Usage: make install PREFIX='<prefix>'; exit 1 ;; \
		*' '*) echo "'$(PREFIX)' has spaces"; exit 1 ;; esac
	mkdir -p "$(PREFIX)"/bin "$(PREFIX)"/share/man/man1
	cp src/xtar "$(PREFIX)"/bin
	cp xtar.1 "$(PREFIX)"/share/man/man1

clean distclean: always
	make -C src $@
	rm -f *~ src/version.h

gitlog: distclean
	git log --name-status > $@


targz: distclean gitlog README_AND_COPYRIGHT
	test ! -d ../xtar-$(VERSION) || exit 1
	cd .. && ln -s xtar xtar-$(VERSION)
	cd .. && trap 'rm xtar-$(VERSION)' 0 && tar --format=ustar --exclude .git -zhcvf xtar-$(VERSION).tar.gz xtar-$(VERSION)

.PHONY: always
