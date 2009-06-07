

all:	distclean
	@sed -n '/^all.sh:/,/^ *$$/ p' Makefile | tail -n +3 | sh -eux

all.sh:
	exit 1 # this target is not to be run.
	goals=xtar
	trap 'rm -f foo.c a.out' 0
	echo 'int main(void) { }' > foo.c
	gcc foo.c -lz -lbz2 && goals=$goals\ xtar_dynamic || true
	rm -f foo.c a.out; trap - 0
	CC_W32=	cc_w32=`env which i686-pc-mingw32-gcc | head -1`
	case $cc_w32 in /*) goals=$goals\ xtar.exe	
		CC_W32=CC_W32=`basename "$cc_w32"` ;; esac
	make -C src $goals $CC_W32


clean distclean: always
	make -C src $@


.PHONY: always
