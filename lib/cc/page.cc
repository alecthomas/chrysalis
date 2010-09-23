#include <cc/colour.h>
#include <cc/page.h>
#include <cc/image.h>
#include <iostream.h>

Page::Page( int width, int height )
/* =================================
	Create a new page
*/
{
	Page::width = width; Page::height = height;
	buffer = new char[ width * height ];
	yTable = new char*[ height ];
	for ( int l = 0; l != height; l++ ) yTable[ l ] = buffer + width * l;
}

Page::~Page()
/* ===========
	Delete page
*/
{
	delete yTable;
	delete buffer;
}

void Page::dump( Page *page )
/* ====================================
	Display page to page
*/
{
	for ( int l = 0; l != ( height < page->height ? height : page->height ); l++ )
		page->write( yTable[ l ], 0, l, width < page->width ? width : page->width );
}

void Page::dump( Page *page, int x, int y, int px, int py, int w, int h )
/* =========================================================================
	Copy a region of this page to a target page
*/
{
char *i;

	if ( py < y ) { h += py - y; py = y; }
	if ( px < x ) { w += px - x; px = x; }
	if ( px - x + w - 1 >= width ) w = width - ( px - x );
	if ( py - y + h - 1 >= height ) h = height - ( py - y );
	i = buffer + ( py - y ) * width + ( px - x );
	if ( w <= 0 || h <= 0 ) return;
	if ( py < 0 ) { h += py; i -= py * width; py = 0; }
	if ( px < 0 ) { w += px; i -= px; px = 0; }
	if ( px + w - 1 >= page->width ) w = page->width - px;
	if ( py + h - 1 >= page->height ) h = page->height - py;
	if ( w <= 0 || h <= 0 ) return;
	for ( int l = 0; l != h; l++, i += width ) page->write( i, px, py + l, w );
}

void Page::dump( Page *page, int x, int y )
/* =========================================================================
	Copy a region of this page to a target page
*/
{
char *i = buffer;
int w = width, h = height;

	if ( !clip( i, x, y, w, h, page->width, page->height ) ) return;
	for ( int l = 0; l != h; l++, i += width ) page->write( i, x, y + l, w );
}

void Page::filledBox( int x, int y, int w, int h, Colour colour )
/* =======================================================
	Should be pretty fast, uses the MIGHTY BLIT yay, yay!!
*/
{
int l;
char *i;

	if ( !clip( i, x, y, w, h, width, height ) ) return;
	for ( l = 0; l != h; l++ )
		memset( yTable[ l + y ] + x, colour.actual, w );
}

void Page::box( int x, int y, int w, int h, Colour colour )
/* =======================================================
	Definitely not optimised, but hey, you shouldn't be using BOX in hard-core
	areas anyway :ù)
*/
{
	filledBox( x, y, w, 1, colour );
	filledBox( x, y, 1, h, colour );
	filledBox( x + w - 1, y, 1, h, colour );
	filledBox( x, y + h - 1, w, 1, colour );
}

void Page::line( int x1, int y1, int x2, int y2, Colour colour )
/* ======================================================
	Draw a line (I'm a thief, I ripped this directly from the PCGPE...and
	I'm proud of it) :)
*/
{
int i, deltax, deltay, numpixels, d, dinc1, dinc2, x, xinc1, xinc2, y, 
	yinc1, yinc2;

	/* Calculate deltax and deltay for initialisation */
	deltax = x2 - x1;
	if ( deltax < 0 ) deltax = -deltax;
	deltay = y2 - y1;
	if ( deltay < 0 ) deltay = -deltay;

	/* Initialize all vars based on which is the independent variable */
	if ( deltax >= deltay )
	{ /* x is independent variable */
		numpixels = deltax + 1;
		d = ( deltay << 1 ) - deltax;
		dinc1 = deltay << 1;
		dinc2 = (deltay - deltax) << 1;
		xinc1 = xinc2 = yinc2 = 1;
		yinc1 = 0;
	} else
	{ /* y is independent variable */
		numpixels = deltay + 1;
		d = (deltax << 1) - deltay;
		dinc1 = deltax << 1;
		dinc2 = (deltax - deltay) << 1;
		xinc1 = 0;
		xinc2 = yinc1 = yinc2 = 1;
	};

	/* Make sure x and y move in the right directions */
	if ( x1 > x2 )
	{
		xinc1 = - xinc1;
		xinc2 = - xinc2;
	}
	if ( y1 > y2 )
	{
		yinc1 = - yinc1;
		yinc2 = - yinc2;
	}

	/* Start drawing at <x1, y1> */
	x = x1;
	y = y1;

	/* Draw the pixels */
	for ( i = 0; i != numpixels; i++ )
	{
		putPixel( x, y, colour );
		if ( d < 0 )
		{
			d += dinc1;
			x += xinc1;
			y += yinc1;
		} else
		{
			d += dinc2;
			x += xinc2;
			y += yinc2;
		}
	}
}

void Page::circle( int x1, int y1, int radius, Colour colour )
/* ====================================================
	Draw a circle :) how hard can it be, right? Once again, I ripped this
	directly from PCGPE!
*/
{
int d, x, y;

	d = 3 - ( radius << 1 );
	x = 0;
	y = radius;

	do {
		putPixel( x1 + x, y1 + y, colour );
		putPixel( x1 + x, y1 - y, colour );
		putPixel( x1 - x, y1 + y, colour );
		putPixel( x1 - x, y1 - y, colour );
		putPixel( x1 + y, y1 + x, colour );
		putPixel( x1 + y, y1 - x, colour );
		putPixel( x1 - y, y1 + x, colour );
		putPixel( x1 - y, y1 - x, colour );
		if ( d < 0 ) 
		{
			d += ( x << 2 ) + 6;
		} else
		{
			d += ( ( x - y ) << 2 ) + 10;
			y--;
		}
		x++;
	} while ( x <= y );
}

void Page::filledCircle( int x1, int y1, int radius, Colour colour )
/* ====================================================
	Draw a circle :) how hard can it be, right? Once again, I ripped this
	directly from PCGPE!
*/
{
int d, x, y;

	d = 3 - ( radius << 1 );
	x = 0;
	y = radius;

	do {
		filledBox( x1 - x, y1 + y, x * 2, 1, colour );
		filledBox( x1 - x, y1 - y, x * 2, 1, colour );
		filledBox( x1 - y, y1 + x, y * 2, 1, colour );
		filledBox( x1 - y, y1 - x, y * 2, 1, colour );
		if ( d < 0 ) 
		{
			d += ( x << 2 ) + 6;
		} else
		{
			d += ( ( x - y ) << 2 ) + 10;
			y--;
		}
		x++;
	} while ( x <= y );
}

void Page::blit( char *source, int column, int row, int width, unsigned mode )
/* ============================================================================
	Transfer data to page, doing things to it at the same time :)
*/
{
int l;

	switch ( mode )
	{
		case MOV :
			memcpy( yTable[ row ] + column, source, width );
		break;
		case MASK :
			for ( l = 0; l != width; l++, source++, column++ )
				if ( *source ) yTable[ row ][ column ] = *source;
		break;
		case SINGLE :
			for ( l = 0; l != width; l++, source++, column++ )
				if ( *source ) yTable[ row ][ column ] = singleColour.actual;
		break;
	}
}

Image *Page::saveBox( int column, int row, int width, int height )
/* ================================================================
	Save a region of the page to an image
*/
{
Image *image;
char *i;

	if ( !clip( i, column, row, width, height, Page::width, Page::height ) ) 
	{
		image = new Image( 0, 0 );
		image->x = column; image->y = row;
	}
	image = new Image( width, height );
	for ( int l = 0; l != height; l++ )
		memcpy( image->buffer + width * l, read( column, row + l, width ), width );
	image->x = column; image->y = row;
	image->mode = MOV;
	image->setPage( this );
	return image;
}

void Page::setSingle( Colour colour )
{
	singleColour = colour;
}
