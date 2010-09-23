#ifndef __FONT_H__
#define __FONT_H__

#include <stdio.h>
#include <cc/colour.h>

class Image;
class Page;
class fixed;
class Font;

typedef Font& (*__fontfuncmanip)(Font&);

class Font {
		Image **image;
		bool isAlias;
		Colour singleColour;
	public :
		Font() { x = y = 0; page = NULL; image = NULL; }
		~Font();
		void alias( Font *font );
		bool load( FILE *file );
		inline void setImage( Image **image ) { Font::image = image; }
		inline void moveTo( int x, int y ) { Font::x = x; Font::y = y; }
		inline void setMode( unsigned mode ) { Font::mode = mode; }
		inline void setPage( Page *page ) { Font::page = page; }
		void setSingle( Colour colour );
		int width( char *text );
		int height( char *text );

		void print( const char *text );
		void newline();
		Font &operator << ( const char *s );
		Font &operator << ( const unsigned char *s ) { return (*this) << (char*)s; }
		Font &operator << ( const signed char *s ) { return (*this) << (unsigned char*)s; }
		Font &operator << ( char c );
		Font &operator << ( signed char c ) { return (*this) << (char)c; }
		Font &operator << ( unsigned char c ) { return (*this) << (char)c; }
		Font &operator << ( int i );
		Font &operator << ( unsigned int i ) { return (*this) << (int)i; }
		Font &operator << ( float f );
		Font &operator << ( fixed const &f );
		Font &operator << ( __fontfuncmanip func ) { return (*func)(*this); }

		int x, y;
		unsigned mode;
		Page *page;
};

extern Font &endl( Font &font );
#endif
