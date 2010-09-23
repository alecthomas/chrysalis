#include <iostream.h>
#include <stdio.h>
#include <fixed.h>
#include <cc/page.h>
#include <cc/image.h>
#include <cc/font.h>

bool Font::load( FILE *file )
/* ======================
	Create and load a font
*/
{
	image = new Image*[ 128 ];
	for ( int l = 0; l != 127; l++ ) image[ l ] = new Image( file );
	mode = Page::MOV;
	isAlias = false;
	return true;
}

void Font::alias( Font *font )
/* ============================
	Create an alias
*/
{
	image = font->image;
	page = font->page;
	x = font->x; y = font->y;
	mode = font->mode;
	isAlias = true;
}

Font::~Font()
/* ===========
	Delete font
*/
{
	if ( isAlias ) return;
	if ( image ) for ( int l = 0; l != 128; l++ ) delete image[ l ];
	delete image;
}

void Font::setSingle( Colour colour )
{
int l;

	singleColour = colour;
	for ( l = 0; l != 128; l++ ) image[ l ]->setSingle( colour );
}

void Font::newline()
/* ==================
	Go down a line, scrolls if necessary
*/
{
int l, h;

	y += ( h = height( "Jj " ) );
	x = 0;
	if ( y + h > page->height )
	{ // scroll page up
		for ( l = 0; l != page->height - h; l++ )
			page->write( page->read( l + h ), l );
		page->filledBox( 0, page->height - h, page->width, h, Colour( 0, 0, 0 ) );
		y -= h;
	}
}

void Font::print( const char *text )
/* ============================
	Display a text string (this function is used by EVERYTHING)
*/
{
char c;
int l, h;
static int lastCharWidth = 0, lastCharHeight = 0;

	if ( text == NULL ) return;
	while ( *text != 0 )
	{
		c = *text;
		switch ( c )
		{
			case '' :
				x -= lastCharWidth;
				page->filledBox( x, y, lastCharWidth, lastCharHeight, Colour( 0, 0, 0 ) );
			break;
			case '\n' :
				newline();
				lastCharWidth = lastCharHeight = 0;
			break;
			default :
				if ( image[ c ] != NULL )
				{
					image[ c ]->put( x, y, page, mode );
					x += image[ c ]->width;
					if ( x >= page->width ) 
					{
						newline();
						lastCharWidth = lastCharHeight = 0;
					} else
					{
						lastCharWidth = image[ c ]->width;
						lastCharHeight = image[ c ]->height;
					}
				}
			break;
		}
		text++;
	}
}





/* ================================================================================================================================
													Redirection operator overrides
================================================================================================================================ */
Font &Font::operator << ( const char *string )
{
	print( string );
	return *this;
}

Font &Font::operator << ( char character )
{
char buffer[ 32 ];

	sprintf( buffer, "%c", character );
	print( buffer );
	return *this;
}

Font &Font::operator << ( int integer )
{
char buffer[ 32 ];

	sprintf( buffer, "%i", integer );
	print( buffer );
	return *this;
}

Font &Font::operator << ( float floating )
{
char buffer[ 32 ];

	sprintf( buffer, "%f", floating );
	print( buffer );
	return *this;
}

Font &Font::operator << ( fixed const &f )
{
int t;

	(*this) << f.integer();
	if ( ( t = f.fraction() ) != 0 )
	{
		if ( t < 0 ) t = -t;
		t = ( (unsigned)t * 100000 ) / 65536;
		(*this) << "." << t;
	}
	return *this;
}

Font &endl( Font &font )
{
	font.newline();
	return font;
}

int Font::width( char *text )
{
int w = 0;
char *c;

	if ( text == NULL ) return 0;
	c = text;
	while ( *c != 0 ) 
	{
		w += image[ *c ]->width;
		c++;
	}
	return w;
}

int Font::height( char *text )
{
int h = 0;
char *c;

	if ( text == NULL ) return 0;
	c = text;
	while ( *c != 0 ) 
	{
		if ( image[ *c ]->height > h ) h = image[ *c ]->height;
		c++;
	}
	return h;
}
