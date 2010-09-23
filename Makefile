#
#					Makefile for Linux Chrysalis ver 0.0.1
#

CFLAGS = -Wno-deprecated -D__X11__ -Wcomment -march=athlon-tbird -Ilib -c -funsigned-char -D__DEBUG__ -O3
OFLAGS = -O3 -fforce-addr -fomit-frame-pointer -finline-functions \
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
alloc.o: alloc.cc include.h lib/fixed.h lib/keyboard.h lib/keymappings.h \
 lib/graphics.h lib/virtual.h lib/graphics.h lib/general.h chkerror.h \
 data.h
chkerror.o: chkerror.cc
creature.o: creature.cc include.h lib/fixed.h lib/keyboard.h \
 lib/keymappings.h lib/graphics.h lib/virtual.h lib/graphics.h \
 lib/general.h chkerror.h creature.h object.h image.h lib.h queue.h \
 player.h morph.h
editor.o: editor.cc include.h lib/fixed.h lib/keyboard.h \
 lib/keymappings.h lib/graphics.h lib/virtual.h lib/graphics.h \
 lib/general.h chkerror.h editwin.h object.h image.h lib.h queue.h \
 editor.h data.h world.h layer.h lib/mouse.h event.h
editwin.o: editwin.cc include.h lib/fixed.h lib/keyboard.h \
 lib/keymappings.h lib/graphics.h lib/virtual.h lib/graphics.h \
 lib/general.h chkerror.h data.h editwin.h object.h image.h lib.h queue.h \
 editor.h world.h layer.h lib/mouse.h event.h
event.o: event.cc include.h lib/fixed.h lib/keyboard.h lib/keymappings.h \
 lib/graphics.h lib/virtual.h lib/graphics.h lib/general.h chkerror.h \
 event.h queue.h creature.h object.h image.h lib.h world.h layer.h
image.o: image.cc include.h lib/fixed.h lib/keyboard.h lib/keymappings.h \
 lib/graphics.h lib/virtual.h lib/graphics.h lib/general.h chkerror.h \
 image.h lib.h queue.h
init.o: init.cc include.h lib/fixed.h lib/keyboard.h lib/keymappings.h \
 lib/graphics.h lib/virtual.h lib/graphics.h lib/general.h chkerror.h \
 object.h image.h lib.h queue.h world.h layer.h data.h
layer.o: layer.cc include.h lib/fixed.h lib/keyboard.h lib/keymappings.h \
 lib/graphics.h lib/virtual.h lib/graphics.h lib/general.h chkerror.h \
 world.h queue.h lib.h layer.h object.h image.h monsters.h creature.h \
 data.h scenery.h player.h morph.h
lib.o: lib.cc include.h lib/fixed.h lib/keyboard.h lib/keymappings.h \
 lib/graphics.h lib/virtual.h lib/graphics.h lib/general.h chkerror.h \
 lib/general.h lib.h
monsters.o: monsters.cc include.h lib/fixed.h lib/keyboard.h \
 lib/keymappings.h lib/graphics.h lib/virtual.h lib/graphics.h \
 lib/general.h chkerror.h creature.h object.h image.h lib.h queue.h \
 monsters.h data.h world.h layer.h player.h morph.h
morph.o: morph.cc include.h lib/fixed.h lib/keyboard.h lib/keymappings.h \
 lib/graphics.h lib/virtual.h lib/graphics.h lib/general.h chkerror.h \
 morph.h
object.o: object.cc include.h lib/fixed.h lib/keyboard.h \
 lib/keymappings.h lib/graphics.h lib/virtual.h lib/graphics.h \
 lib/general.h chkerror.h object.h image.h lib.h queue.h world.h layer.h
play.o: play.cc include.h lib/fixed.h lib/keyboard.h lib/keymappings.h \
 lib/graphics.h lib/virtual.h lib/graphics.h lib/general.h chkerror.h \
 world.h queue.h lib.h layer.h object.h image.h monsters.h creature.h \
 data.h player.h morph.h
player.o: player.cc include.h lib/fixed.h lib/keyboard.h \
 lib/keymappings.h lib/graphics.h lib/virtual.h lib/graphics.h \
 lib/general.h chkerror.h world.h queue.h lib.h layer.h object.h image.h \
 data.h player.h creature.h morph.h monsters.h
scenery.o: scenery.cc include.h lib/fixed.h lib/keyboard.h \
 lib/keymappings.h lib/graphics.h lib/virtual.h lib/graphics.h \
 lib/general.h chkerror.h scenery.h object.h image.h lib.h queue.h \
 world.h layer.h
world.o: world.cc include.h lib/fixed.h lib/keyboard.h lib/keymappings.h \
 lib/graphics.h lib/virtual.h lib/graphics.h lib/general.h chkerror.h \
 data.h world.h queue.h lib.h layer.h object.h image.h player.h \
 creature.h morph.h monsters.h scenery.h
chkerror.o: chkerror.h
creature.o: creature.h object.h image.h lib/virtual.h lib/graphics.h \
 lib/general.h lib.h chkerror.h queue.h lib/fixed.h
data.o: data.h
editor.o: editor.h data.h queue.h world.h lib.h layer.h object.h image.h \
 lib/virtual.h lib/graphics.h lib/general.h chkerror.h lib/fixed.h \
 lib/mouse.h
editwin.o: editwin.h object.h image.h lib/virtual.h lib/graphics.h \
 lib/general.h lib.h chkerror.h queue.h lib/fixed.h
event.o: event.h queue.h
image.o: image.h lib/virtual.h lib/graphics.h lib/general.h lib.h \
 chkerror.h queue.h
include.o: include.h lib/fixed.h lib/keyboard.h lib/keymappings.h \
 lib/graphics.h lib/virtual.h lib/graphics.h lib/general.h chkerror.h
layer.o: layer.h object.h image.h lib/virtual.h lib/graphics.h \
 lib/general.h lib.h chkerror.h queue.h lib/fixed.h
lib.o: lib.h
monsters.o: monsters.h creature.h object.h image.h lib/virtual.h \
 lib/graphics.h lib/general.h lib.h chkerror.h queue.h lib/fixed.h data.h
morph.o: morph.h
object.o: object.h image.h lib/virtual.h lib/graphics.h lib/general.h \
 lib.h chkerror.h queue.h lib/fixed.h
player.o: player.h creature.h object.h image.h lib/virtual.h \
 lib/graphics.h lib/general.h lib.h chkerror.h queue.h lib/fixed.h \
 morph.h
queue.o: queue.h
scenery.o: scenery.h object.h image.h lib/virtual.h lib/graphics.h \
 lib/general.h lib.h chkerror.h queue.h lib/fixed.h
world.o: world.h queue.h lib.h layer.h object.h image.h lib/virtual.h \
 lib/graphics.h lib/general.h chkerror.h lib/fixed.h
