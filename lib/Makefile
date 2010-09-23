#
# Makefile for generating object files/library from library files
#

OFLAGS = -O3
SUPEROFLAGS = -fforce-mem -fforce-addr -fomit-frame-pointer -finline-functions \
	-fcaller-saves -fno-function-cse -ffast-math
CFLAGS = -D__X11__ -I/usr/include/g++-include -I/usr/local/include -I/home/kestrel/source/lib -c -funsigned-char\
	$(OFLAGS) $(SUPEROFLAGS) $(DFLAGS)
LDFLAGS = -L/home/kestrel/source/lib -lforge -lvga -lvgagl -lX11 -lXext
#-lX11 -lXext
#-D__DEBUG__

.c.o:
	gcc $(CFLAGS) $<

.cc.o:
	g++ $(CFLAGS) $<

.S.o:
	g++ -D__elf__ -g2 $< -c

OBJS = general.o graphics.o keyboard.o virtual.o vblit.o mouse.o fixed.o fixeddiv.o mitshm.o 

libforge.a: $(OBJS)
	ar r libforge.a $(OBJS)

tmp.o: tmp.cc
	g++ $(CFLAGS) tmp.cc

tmp: tmp.o $(OBJS)
	g++ tmp.o -o tmp -L./ $(LDFLAGS)

clean:
	rm -f $(OBJS) libforge.a

all:
	make clean
	make

dep:
	makedepend -I/usr/include/ -I/usr/lib/gcc-lib/i486-linux/2.7.0/include/ -I/usr/include/g++-include/ -I/home/kestrel/source/lib/ *.cc *.c
# DO NOT DELETE

fixed.o: /usr/include/stdio.h /usr/include/features.h
fixed.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
fixed.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
fixed.o: /usr/include/bits/types.h /usr/include/libio.h
fixed.o: /usr/include/_G_config.h /usr/include/wchar.h
fixed.o: /usr/include/bits/wchar.h /usr/include/gconv.h
fixed.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
fixed.o: /usr/include/bits/stdio_lim.h fixed8.h fixed.h
general.o: /usr/include/stdio.h /usr/include/features.h
general.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
general.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
general.o: /usr/include/bits/types.h /usr/include/libio.h
general.o: /usr/include/_G_config.h /usr/include/wchar.h
general.o: /usr/include/bits/wchar.h /usr/include/gconv.h
general.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
general.o: /usr/include/bits/stdio_lim.h /usr/include/malloc.h
general.o: /usr/include/string.h general.h
graphics.o: /usr/include/string.h /usr/include/features.h
graphics.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
graphics.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
graphics.o: /usr/include/stdlib.h /usr/include/stdio.h
graphics.o: /usr/include/bits/types.h /usr/include/libio.h
graphics.o: /usr/include/_G_config.h /usr/include/wchar.h
graphics.o: /usr/include/bits/wchar.h /usr/include/gconv.h
graphics.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
graphics.o: /usr/include/bits/stdio_lim.h graphics.h general.h
keyboard.o: /usr/include/stdlib.h /usr/include/features.h
keyboard.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
keyboard.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
keyboard.o: /usr/include/string.h keyboard.h keymappings.h general.h
mouse.o: /usr/include/string.h /usr/include/features.h
mouse.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
mouse.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
mouse.o: /usr/include/stdio.h /usr/include/bits/types.h /usr/include/libio.h
mouse.o: /usr/include/_G_config.h /usr/include/wchar.h
mouse.o: /usr/include/bits/wchar.h /usr/include/gconv.h
mouse.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
mouse.o: /usr/include/bits/stdio_lim.h general.h mouse.h
tmp.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
tmp.o: /usr/include/gnu/stubs.h
tmp.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
tmp.o: /usr/include/bits/types.h /usr/include/libio.h
tmp.o: /usr/include/_G_config.h /usr/include/wchar.h
tmp.o: /usr/include/bits/wchar.h /usr/include/gconv.h
tmp.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
tmp.o: /usr/include/bits/stdio_lim.h graphics.h virtual.h
tmp.o: /usr/include/string.h general.h
virtual.o: /usr/include/malloc.h /usr/include/features.h
virtual.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
virtual.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
virtual.o: /usr/include/string.h virtual.h /usr/include/stdio.h
virtual.o: /usr/include/bits/types.h /usr/include/libio.h
virtual.o: /usr/include/_G_config.h /usr/include/wchar.h
virtual.o: /usr/include/bits/wchar.h /usr/include/gconv.h
virtual.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
virtual.o: /usr/include/bits/stdio_lim.h graphics.h general.h
virtual.old.o: /usr/include/malloc.h /usr/include/features.h
virtual.old.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
virtual.old.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
virtual.old.o: /usr/include/string.h virtual.h /usr/include/stdio.h
virtual.old.o: /usr/include/bits/types.h /usr/include/libio.h
virtual.old.o: /usr/include/_G_config.h /usr/include/wchar.h
virtual.old.o: /usr/include/bits/wchar.h /usr/include/gconv.h
virtual.old.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
virtual.old.o: /usr/include/bits/stdio_lim.h graphics.h general.h
