#
# Makefile for generating object files/library from library files
#

OFLAGS = -O3
SUPEROFLAGS = -fforce-addr -fomit-frame-pointer -finline-functions \
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
fixed.o: fixed.cc fixed8.h fixed.h
general.o: general.cc general.h
graphics.o: graphics.cc graphics.h general.h
keyboard.o: keyboard.cc keyboard.h keymappings.h general.h
mitshm.old.o: mitshm.old.cc
mouse.o: mouse.cc general.h mouse.h
tmp.o: tmp.cc graphics.h virtual.h general.h
virtual.o: virtual.cc virtual.h graphics.h general.h
virtual.old.o: virtual.old.cc virtual.h graphics.h general.h
