# Optimization flags
OFLAGS = -O3
SUPEROFLAGS = -fforce-mem -fforce-addr -fomit-frame-pointer -finline-functions \
	-fcaller-saves -fno-function-cse -ffast-math -funroll-all-loops
CFLAGS = -D__X11__ -Wcomment -m486 -I.. -c -funsigned-char\
	$(OFLAGS) $(SUPEROFLAGS) $(DFLAGS)
# Debug flags
#DFLAGS = -D__DEBUG__ 
LDFLAGS = -L.. -L/usr/X11/lib -lforge++ -lforge -lvga -lm -lX11 -lXext
#-lvga -lvgagl

# Project specific stuff
OUTPUTFILE = ../libforge++.a
LIBS = ../libforge.a
MAIN = ../libforge++.a
OBJS = video.o page.o font.o image.o colour.o mouse.o keyboard.o \
	colourtable.o partialpage.o 

# Compiler flags
CC = g++ 
LD = ld

.cc.o:
	$(CC) $(CFLAGS) $<

.cc:
	$(CC) $(CFLAGS) $<

.o:
	$(CC) $< $(LDFLAGS) -o $*

$(OUTPUTFILE): $(OBJS) $(LIBS)
	make link

link:
	ar r $(OUTPUTFILE) $(OBJS)

tmp: tmp.o $(OBJS) $(OUTPUTFILE)

all:
	make clean
	make

clean: 
	rm -f $(OBJS)

dep:
	makedepend -I/usr/include/ -I/usr/lib/gcc-lib/i486-linux/2.7.0/include/ -I/usr/include/g++-include/ -I/home/kestrel/Forge/lib/ *.cc
# DO NOT DELETE

colour.o: /usr/include/stdlib.h /usr/include/features.h
colour.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
colour.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
colour.o: /usr/include/stdio.h /usr/include/bits/types.h /usr/include/libio.h
colour.o: /usr/include/_G_config.h /usr/include/wchar.h
colour.o: /usr/include/bits/wchar.h /usr/include/gconv.h
colour.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
colour.o: /usr/include/bits/stdio_lim.h colour.h video.h page.h
colour.o: /usr/include/string.h font.h
font.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
font.o: /usr/include/gnu/stubs.h
font.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
font.o: /usr/include/bits/types.h /usr/include/libio.h
font.o: /usr/include/_G_config.h /usr/include/wchar.h
font.o: /usr/include/bits/wchar.h /usr/include/gconv.h
font.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
font.o: /usr/include/bits/stdio_lim.h
image.o: /usr/include/string.h /usr/include/features.h
image.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
image.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
keyboard.o: /usr/include/stdlib.h /usr/include/features.h
keyboard.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
keyboard.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
keyboard.o: /usr/include/string.h
mouse.o: /usr/include/string.h /usr/include/features.h
mouse.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
mouse.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
mouse.o: /usr/include/stdio.h /usr/include/bits/types.h /usr/include/libio.h
mouse.o: /usr/include/_G_config.h /usr/include/wchar.h
mouse.o: /usr/include/bits/wchar.h /usr/include/gconv.h
mouse.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
mouse.o: /usr/include/bits/stdio_lim.h mouse.h
tmp.o: video.h page.h /usr/include/string.h /usr/include/features.h
tmp.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
tmp.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h colour.h
tmp.o: mouse.h image.h /usr/include/stdio.h /usr/include/bits/types.h
tmp.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
tmp.o: /usr/include/bits/wchar.h /usr/include/gconv.h
tmp.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stdarg.h
tmp.o: /usr/include/bits/stdio_lim.h
video.o: /usr/include/string.h /usr/include/features.h
video.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
video.o: /usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include/stddef.h
