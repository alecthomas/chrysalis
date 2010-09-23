# Chrysalis - Source to a 2D Platformer from 1994 #

[Chrysalis](http://github.com/alecthomas/chrysalis) is a game a friend (Eric Sambach) and I wrote in 1994, just out of high school.

We sent this game to Epic MegaGames for consideration, but without success (and
they hadn't even seen the source). Ouch.

## The Game ##

I really thought the game itself could have been great. It centered around
an amorphous blob of green slime (which wasn't as ugly as it sounds) that could assume the
shape and powers of defeated opponents. Only a limited number of shapes could be stored, and the various powers had to be used to solve puzzles and bypass obstacles.

## The Engine ##

The source is horrendous. Almost zero encapsulation. Globals everywhere. You get the picture.

There is C, C++, Objective-C, Intel-style assembler, and GNU-style assembler.

The game ran only on 320x200 256-bit indexed video surfaces, initially
supporting only DOS (using the Watcom C++ compiler and mode 13), and the Linux
console (via svgalib). A friend (Brian Spilsbury) from #c on EFnet later wrote
an X11 surface. We divided the 256 colour palette into a 6x6x6 RGB cube, which allowed us to easily compute fade transitions, though limited the fidelity of images somewhat.

The underlying graphics library was largely equivalent to SDL, but preceded its public release by about four years.
It supported full clipping, transparent images, virtual
pages, bitmapped text fonts, circle and line (Bresenham) primitives. It also
included a full windowing system with transparency, used for the Chrysalis
level editor.

Assembler was used for the low-level rendering primitives. 16-bit and 8-bit
fixed-point number classes were used for all coordinates.

As a sign of its vintage, the game implements nearly everything from scratch and has almost no external dependencies, primarily because there were very few available. The STL didn't exist, nor did SDL, or any number of other libraries that would have saved us a lot of time.

## The Visuals ##

The art I created was quite nice too, I thought, though lost now, buried
inside the proprietary packed file format we created.

The game itself used four-layer parallax scrolling. The play layer was the second from the front. Each layer faded off slightly more, to ensure it didn't intrude too much visually.

Entities in the game could be animated, such as the water and vines in creature.cc.

## Last Words ##

If anybody has an old Linux machine lying around and can run the binaries (which are included on GitHub), I'd love a screenshot.
