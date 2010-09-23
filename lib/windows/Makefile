#
# Makefile for generating object files/library from library files
#

OFLAGS = -O3
SUPEROFLAGS = -fforce-mem -fforce-addr -fomit-frame-pointer -finline-functions \
	-fcaller-saves -fno-function-cse -ffast-math -funroll-all-loops
CFLAGS = -D__X11__ -Wcomment -m486 -I.. -c -funsigned-char\
	$(OFLAGS) $(SUPEROFLAGS) $(DFLAGS)
LDFLAGS = -L.. -L/usr/X11/lib -lforge++ -lforge -lm -lvga -lvgagl -lX11 -lXext
OBJS = manager.o region.o window.o button.o raisedregion.o

.S.o: 
	g++ -c $(CFLAGS) $<

.cc.o:
	g++ -c $(CFLAGS) $<

.o:
	g++ $< $(LDFLAGS) -o $*


../libforge++.a: $(OBJS)
	ar r ../libforge++.a $(OBJS)

all:
	make clean
	make

tmp: tmp.o $(OBJS) ../libforge++.a
	g++ tmp.o $(LDFLAGS) -o tmp

clean:
	rm -f $(OBJS)

dep:
	makedepend -I/usr/include/ -I/usr/include/g++/ -I/home/kestrel/source/lib/ *.cc
# DO NOT DELETE

manager.o: /usr/include/stdlib.h /usr/include/features.h
manager.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
manager.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
window.o: /usr/include/string.h /usr/include/features.h
window.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
window.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
