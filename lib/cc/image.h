#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <cc/colour.h>
#include <stdio.h>

class Page;

class Image {
	public :
		Image( int width, int height );
		Image( Image const &image );
		Image( FILE *file );
		~Image();
		inline void put( unsigned mode ) { put( x, y, page, mode ); }
		inline void put( int x, int y ) { put( x, y, page, mode ); }
		inline void put( int x, int y, unsigned mode ) { put( x, y, page, mode ); }
		void put( int x, int y, Page *page, unsigned mode );
		void setPage( Page *page ) { Image::page = page; }

		inline void setPosition( int x, int y ) { Image::x = x; Image::y = y; }
		inline void setMode( int mode ) { Image::mode = mode; }
		inline void setSingle( Colour colour ) { singleColour = colour; }

		int width, height, x, y, mode;
		char *buffer;
		Page *page;
		Colour singleColour;
};
#endif
