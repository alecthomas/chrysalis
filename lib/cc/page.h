#ifndef __PAGE_H__
#define __PAGE_H__

#include <string.h>
#include "colour.h"

class Image;

class Page {
	protected :
		char **yTable, *buffer;
	public :
		Page() {}
		Page( int width, int height );
		~Page();

		virtual char *read( int row );
		virtual char *read( int column, int row, int width );
		virtual void write( char *source, int row );
		virtual void write( char *source, int column, int row, int width );
		virtual void blit( char *source, int column, int row, int width, unsigned mode );
		virtual void clear( Colour const &colour );

		virtual Image *saveBox( int column, int row, int width, int height );
		virtual void filledBox( int column, int row, int width, int height, Colour colour );
		virtual void box( int column, int row, int width, int height, Colour colour );
		virtual void line( int column, int row, int ecolumn, int erow, Colour colour );
		virtual void circle( int column, int row, int radius, Colour colour );
		virtual void filledCircle( int x1, int y1, int radius, Colour colour );
		virtual void putPixel( int column, int row, Colour colour );

		void dump( Page *video, int x, int y );
		void dump( Page *video, int x, int y, int px, int py, int pw, int ph );
		void dump( Page *video );

		enum {
			MOV,
			MASK,
			SINGLE
		};

		virtual void setSingle( Colour colour );
		static bool clip( char *&image, int &x, int &y, int &w, int &h, int tw, int th );

		int width, height;
		Colour singleColour;
};

inline void Page::putPixel( int column, int row, Colour colour )
/* ============================================================
	Display a pixel
*/
{
	if ( column < 0 || column >= width || row < 0 || row >= height ) return;
	yTable[ row ][ column ] = colour.actual;
}

inline bool Page::clip( char *&image, int &x, int &y, int &w, int &h, int tw, int th )
/* ===================================================================================
	Returns true if object becomes totally clipped
*/
{
	if ( w < 0 )
	{
		w = -w + 1;
		x -= w - 1;
	}
	if ( h < 0 )
	{
		h = -h + 1;
		y -= h - 1;
	}
	if ( y < 0 ) { h += y; image -= y * w; y = 0; }
	if ( x < 0 ) { w += x; image -= x; x = 0; }
	if ( x + w - 1 >= tw ) w = tw - x;
	if ( y + h - 1 >= th ) h = th - y;
	if ( w <= 0 || h <= 0 ) return false;
	return true;
}

inline char *Page::read( int row )
/* =========================
	Return a pointer to page memory
*/
{
	return yTable[ row ];
}

inline char *Page::read( int column, int row, int width )
/* ================================================
	Return a pointer to page memory
*/
{
	return yTable[ row ] + column;
}

inline void Page::write( char *source, int row )
/* =======================================
	Write external data to page
*/
{
	memcpy( yTable[ row ], source, width );
}

inline void Page::write( char *source, int column, int row, int width )
/* ==============================================================
	Write external data to page
*/
{
	memcpy( yTable[ row ] + column, source, width );
}

inline void Page::clear( Colour const &colour )
{
	memset( buffer, colour.actual, width * height );
}
#endif
