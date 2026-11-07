#
#					Makefile for Chrysalis
#

CXX ?= c++
CXXFLAGS = -O2 -std=gnu++14 -Wno-deprecated -Werror=return-type -funsigned-char -D__SDL__ -D__DEBUG__ \
	-Ilib $(shell sdl2-config --cflags) -MMD -MP
LDLIBS = -Llib -lforge $(shell pkg-config --libs sdl2)
OBJS = layer.o init.o lib.o creature.o player.o world.o image.o routines.o \
	monsters.o object.o scenery.o morph.o alloc.o event.o
EDITOROBJS = editor.o editwin.o
GAMEOBJS = play.o
ALLOBJS = $(OBJS) $(EDITOROBJS) $(GAMEOBJS)

all: Chrysalis Editor

Chrysalis: $(GAMEOBJS) $(OBJS) lib/libforge.a
	$(CXX) $(GAMEOBJS) $(OBJS) -o $@ $(LDLIBS)

Editor: $(EDITOROBJS) $(OBJS) lib/libforge.a
	$(CXX) $(EDITOROBJS) $(OBJS) -o $@ $(LDLIBS)

lib/libforge.a: FORCE
	$(MAKE) -C lib

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(ALLOBJS) $(ALLOBJS:.o=.d) Chrysalis Editor
	$(MAKE) -C lib clean

FORCE:

.PHONY: all clean FORCE

-include $(ALLOBJS:.o=.d)
