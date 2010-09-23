#
#					Makefile for Linux Chrysalis ver 0.0.1
#

CFLAGS = -Wno-deprecated -D__X11__ -Wcomment -march=athlon-tbird -Ilib -c -funsigned-char -D__DEBUG__ -O3
OFLAGS = -O3 -fforce-mem -fforce-addr -fomit-frame-pointer -finline-functions \
	-fcaller-saves -fno-function-cse -ffast-math -funroll-all-loops
LINKFLAGS = -Llib -L/usr/X11R6/lib -lforge -lX11 -lXext
OBJS = layer.o init.o lib.o creature.o player.o world.o image.o assembler.o \
	monsters.o object.o scenery.o morph.o alloc.o event.o
EDITOROBJS = editor.o editwin.o
GAMEOBJS = play.o

.cc.o:
	g++ $(CFLAGS) $(OFLAGS) $<

.S.o:
	g++ $(CFLAGS) $(OFLAGS) $<

Chrysalis: $(GAMEOBJS) $(OBJS) lib/libforge.a
	make link

Editor: $(EDITOROBJS) $(OBJS) lib/libforge.a
	make elink

link:
	g++ $(GAMEOBJS) $(OBJS) -o Chrysalis $(LINKFLAGS) 

elink:
	g++ $(EDITOROBJS) $(OBJS) -o Editor $(LINKFLAGS)
	
all:
	make
	make Editor

clean: 
	rm -f $(OBJS) $(EDITOROBJS) $(GAMEOBJS) Chrysalis Editor

dep:
	makedepend -I/usr/lib/gcc-lib/i486-linux/2.6.3/include -I/usr/g++include -I/usr/include/g++ -I../lib *.cc
# DO NOT DELETE

alloc.o: include.h /usr/include/string.h /usr/include/features.h
alloc.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
alloc.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
alloc.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
alloc.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
alloc.o: /usr/include/bits/types.h /usr/include/libio.h
alloc.o: /usr/include/_G_config.h /usr/include/wchar.h
alloc.o: /usr/include/bits/wchar.h /usr/include/gconv.h
alloc.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
alloc.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
alloc.o: chkerror.h data.h /usr/include/malloc.h
chkerror.o: /usr/include/stdio.h /usr/include/features.h
chkerror.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
chkerror.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
chkerror.o: /usr/include/bits/types.h /usr/include/libio.h
chkerror.o: /usr/include/_G_config.h /usr/include/wchar.h
chkerror.o: /usr/include/bits/wchar.h /usr/include/gconv.h
chkerror.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
chkerror.o: /usr/include/bits/stdio_lim.h /usr/include/time.h
chkerror.o: /usr/include/bits/time.h
creature.o: include.h /usr/include/string.h /usr/include/features.h
creature.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
creature.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
creature.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
creature.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
creature.o: /usr/include/bits/types.h /usr/include/libio.h
creature.o: /usr/include/_G_config.h /usr/include/wchar.h
creature.o: /usr/include/bits/wchar.h /usr/include/gconv.h
creature.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
creature.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
creature.o: chkerror.h creature.h object.h image.h lib.h queue.h player.h
creature.o: morph.h
editor.o: /usr/include/signal.h /usr/include/features.h
editor.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
editor.o: /usr/include/bits/sigset.h /usr/include/bits/types.h
editor.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
editor.o: /usr/include/bits/signum.h /usr/include/unistd.h
editor.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
editor.o: include.h /usr/include/string.h /usr/include/stdlib.h
editor.o: ../lib/fixed.h ../lib/keyboard.h ../lib/keymappings.h
editor.o: ../lib/graphics.h /usr/include/stdio.h /usr/include/libio.h
editor.o: /usr/include/_G_config.h /usr/include/wchar.h
editor.o: /usr/include/bits/wchar.h /usr/include/gconv.h
editor.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
editor.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
editor.o: chkerror.h editwin.h object.h image.h lib.h queue.h editor.h data.h
editor.o: world.h layer.h ../lib/mouse.h event.h
editwin.o: include.h /usr/include/string.h /usr/include/features.h
editwin.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
editwin.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
editwin.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
editwin.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
editwin.o: /usr/include/bits/types.h /usr/include/libio.h
editwin.o: /usr/include/_G_config.h /usr/include/wchar.h
editwin.o: /usr/include/bits/wchar.h /usr/include/gconv.h
editwin.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
editwin.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
editwin.o: chkerror.h data.h editwin.h object.h image.h lib.h queue.h
editwin.o: editor.h world.h layer.h ../lib/mouse.h event.h
event.o: include.h /usr/include/string.h /usr/include/features.h
event.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
event.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
event.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
event.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
event.o: /usr/include/bits/types.h /usr/include/libio.h
event.o: /usr/include/_G_config.h /usr/include/wchar.h
event.o: /usr/include/bits/wchar.h /usr/include/gconv.h
event.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
event.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
event.o: chkerror.h event.h queue.h creature.h object.h image.h lib.h world.h
event.o: layer.h
image.o: include.h /usr/include/string.h /usr/include/features.h
image.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
image.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
image.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
image.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
image.o: /usr/include/bits/types.h /usr/include/libio.h
image.o: /usr/include/_G_config.h /usr/include/wchar.h
image.o: /usr/include/bits/wchar.h /usr/include/gconv.h
image.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
image.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
image.o: chkerror.h /usr/include/malloc.h image.h lib.h queue.h
init.o: include.h /usr/include/string.h /usr/include/features.h
init.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
init.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
init.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
init.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
init.o: /usr/include/bits/types.h /usr/include/libio.h
init.o: /usr/include/_G_config.h /usr/include/wchar.h
init.o: /usr/include/bits/wchar.h /usr/include/gconv.h
init.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
init.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
init.o: chkerror.h /usr/include/signal.h /usr/include/bits/sigset.h
init.o: /usr/include/bits/signum.h object.h image.h lib.h queue.h world.h
init.o: layer.h data.h
layer.o: include.h /usr/include/string.h /usr/include/features.h
layer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
layer.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
layer.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
layer.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
layer.o: /usr/include/bits/types.h /usr/include/libio.h
layer.o: /usr/include/_G_config.h /usr/include/wchar.h
layer.o: /usr/include/bits/wchar.h /usr/include/gconv.h
layer.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
layer.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
layer.o: chkerror.h world.h queue.h lib.h layer.h object.h image.h monsters.h
layer.o: creature.h data.h scenery.h player.h morph.h
lib.o: include.h /usr/include/string.h /usr/include/features.h
lib.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
lib.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
lib.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
lib.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
lib.o: /usr/include/bits/types.h /usr/include/libio.h
lib.o: /usr/include/_G_config.h /usr/include/wchar.h
lib.o: /usr/include/bits/wchar.h /usr/include/gconv.h
lib.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
lib.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
lib.o: chkerror.h lib.h
monsters.o: include.h /usr/include/string.h /usr/include/features.h
monsters.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
monsters.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
monsters.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
monsters.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
monsters.o: /usr/include/bits/types.h /usr/include/libio.h
monsters.o: /usr/include/_G_config.h /usr/include/wchar.h
monsters.o: /usr/include/bits/wchar.h /usr/include/gconv.h
monsters.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
monsters.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
monsters.o: chkerror.h creature.h object.h image.h lib.h queue.h monsters.h
monsters.o: data.h world.h layer.h player.h morph.h
morph.o: include.h /usr/include/string.h /usr/include/features.h
morph.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
morph.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
morph.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
morph.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
morph.o: /usr/include/bits/types.h /usr/include/libio.h
morph.o: /usr/include/_G_config.h /usr/include/wchar.h
morph.o: /usr/include/bits/wchar.h /usr/include/gconv.h
morph.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
morph.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
morph.o: chkerror.h morph.h
object.o: include.h /usr/include/string.h /usr/include/features.h
object.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
object.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
object.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
object.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
object.o: /usr/include/bits/types.h /usr/include/libio.h
object.o: /usr/include/_G_config.h /usr/include/wchar.h
object.o: /usr/include/bits/wchar.h /usr/include/gconv.h
object.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
object.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
object.o: chkerror.h object.h image.h lib.h queue.h world.h layer.h
play.o: /usr/include/unistd.h /usr/include/features.h
play.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
play.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
play.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
play.o: /usr/include/bits/confname.h include.h /usr/include/string.h
play.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
play.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
play.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
play.o: /usr/include/bits/wchar.h /usr/include/gconv.h
play.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
play.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
play.o: chkerror.h world.h queue.h lib.h layer.h object.h image.h monsters.h
play.o: creature.h data.h player.h morph.h
player.o: include.h /usr/include/string.h /usr/include/features.h
player.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
player.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
player.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
player.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
player.o: /usr/include/bits/types.h /usr/include/libio.h
player.o: /usr/include/_G_config.h /usr/include/wchar.h
player.o: /usr/include/bits/wchar.h /usr/include/gconv.h
player.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
player.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
player.o: chkerror.h world.h queue.h lib.h layer.h object.h image.h data.h
player.o: player.h creature.h morph.h monsters.h
scenery.o: include.h /usr/include/string.h /usr/include/features.h
scenery.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
scenery.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
scenery.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
scenery.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
scenery.o: /usr/include/bits/types.h /usr/include/libio.h
scenery.o: /usr/include/_G_config.h /usr/include/wchar.h
scenery.o: /usr/include/bits/wchar.h /usr/include/gconv.h
scenery.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
scenery.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
scenery.o: chkerror.h scenery.h object.h image.h lib.h queue.h world.h
scenery.o: layer.h
world.o: include.h /usr/include/string.h /usr/include/features.h
world.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
world.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stddef.h
world.o: /usr/include/stdlib.h ../lib/fixed.h ../lib/keyboard.h
world.o: ../lib/keymappings.h ../lib/graphics.h /usr/include/stdio.h
world.o: /usr/include/bits/types.h /usr/include/libio.h
world.o: /usr/include/_G_config.h /usr/include/wchar.h
world.o: /usr/include/bits/wchar.h /usr/include/gconv.h
world.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.1.1/include/stdarg.h
world.o: /usr/include/bits/stdio_lim.h ../lib/virtual.h ../lib/general.h
world.o: chkerror.h /usr/include/unistd.h /usr/include/bits/posix_opt.h
world.o: /usr/include/bits/confname.h data.h world.h queue.h lib.h layer.h
world.o: object.h image.h player.h creature.h morph.h monsters.h scenery.h
