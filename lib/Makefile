#
# Makefile for generating object files/library from library files
#

CXX ?= c++
CXXFLAGS = -O2 -std=gnu++14 -Wno-deprecated -Werror=return-type -funsigned-char -D__SDL__ \
	$(shell sdl2-config --cflags) -MMD -MP $(DFLAGS)

OBJS = general.o graphics.o keyboard.o virtual.o vblit.o mouse.o fixed.o sdl.o

libforge.a: $(OBJS)
	$(AR) rcs libforge.a $(OBJS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) libforge.a

.PHONY: clean

-include $(OBJS:.o=.d)
