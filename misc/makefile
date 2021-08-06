#	@(#)makefile	1.6
CFLAGS=

all:  xstr mkstr fold ctags

# Must use cc
mkstr: mkstr.c
	cc $(CFLAGS) mkstr.c -o mkstr

xstr:  xstr.c
	cc $(CFLAGS) xstr.c -o xstr

fold:  fold.c
	cc $(CFLAGS) fold.c -o fold

ctags: ctags.c
	cc $(CFLAGS) ctags.c -o ctags

clean:
	rm -f ctags xstr mkstr fold
