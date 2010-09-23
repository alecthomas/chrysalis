#ifndef __VIDEO_H__
#define __VIDEO_H__

/*
	I'm getting a bit sick of making new routines for all my app's :(. But anyway, hopefully this will be the best/final
	one I'll do. Supports heaps of 256 colour modes.

	Author: Alec Thomas
	Date: Wed Nov 22 10:17:54 EST 1995
*/

#ifdef __SVGA__
#include <vga.h>
#endif
#include "page.h"

class Colour;

class Video : public Page {
#ifdef __SVGA__
		typedef struct {
			int mode;
			char *name;
		} Mode;
		static Mode modeList[];
#endif
		friend Colour;
		static int palette[];
		static char readBuffer[ 1024 ], *actual;
	public :
		Video();
		Video( char *mode );
		~Video();

#ifdef __SVGA__
		char *read( int row );
		char *read( int column, int row, int width );

		void write( char *source, int row );
		void write( char *source, int column, int row, int width );

		void blit( char *source, int column, int row, int width, unsigned mode );
		void clear( Colour const &colour );

		void filledBox( int column, int row, int width, int height, Colour colour );
		void box( int column, int row, int width, int height, Colour colour );
		void line( int column, int row, int ecolumn, int erow, Colour colour );
		void circle( int column, int row, int radius, Colour colour );
		void putPixel( int column, int row, Colour colour );
#endif

		// probably only used by X, but it flushes the buffer to the video
		void sync();
		void syncPartial( int x, int y, int w, int h );

		void setPalette( int index, int r, int g, int b );
		void waitRetrace();

		int mode;
};
#endif
