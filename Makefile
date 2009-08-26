
# 'all' -target will rebuild everything from scratch (to be sure)

VERSION = 0.97
VERDATE = 2009-07-33

# in many targets, distclean is added as dependencies to avoid parallelism

all:	distclean src/version.h
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


src/version.h: distclean Makefile
	echo '#define VERSION "$(VERSION)":#define VERDATE "$(VERDATE)"' \
		| tr : \\012 > $@

README_AND_COPYRIGHT: Makefile
	echo '/^xtar version /s/.*/xtar version $(VERSION) $(VERDATE)/:wq:' \
		| tr : \\012 | ed $@

clean distclean: always
	make -C src $@
	rm -f *~ src/version.h

gitlog: distclean
	git log --name-status > $@


targz: distclean gitlog README_AND_COPYRIGHT
	cd .. && ln -s xtar xtar-$(VERSION)
	cd .. && trap 'rm xtar-$(VERSION)' 0 \
		&& tar --exclude .git -zhcvf xtar-$(VERSION).tar.gz xtar-$(VERSION)

.PHONY: always
