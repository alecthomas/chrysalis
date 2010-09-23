#include <iostream.h>
#include <string.h>
#include <cc/page.h>
#include <cc/image.h>

Image::Image( int w, int h )
/* ==========================
	Create a new image, with a specific width and height
*/
{
	buffer = new char[ w * h ];
	width = w; height = h;
	mode = Page::MASK;
}

Image::Image( FILE *file )
/* ========================
	Create and load a new file
*/
{
	fread( &width, sizeof( int ), 1, file );
	fread( &height, sizeof( int ), 1, file );
	buffer = new char[ width * height ];
	fread( buffer, 1, width * height, file );
	mode = Page::MASK;
}

Image::Image( Image const &image )
/* ================================
	Copy an image
*/
{
	width = image.width; height = image.height;
	buffer = new char[ width * height ];
	memcpy( buffer, image.buffer, width * height );
	mode = Page::MASK;
}

Image::~Image()
/* =============
	Free image memory
*/
{
	delete buffer;
}

void Image::put( int x, int y, Page *page, unsigned mode )
/* ============================================
	Dump an image to a specific page
*/
{
int l, copyw, h;
char *image;

	image = buffer;
	copyw = width;
	h = height;
	if ( Page::clip( image, x, y, copyw, h, page->width, page->height ) == false ) return;
	Image::page = page;
	Image::x = x; Image::y = y;
	Image::mode = mode;
	if ( mode == Page::SINGLE ) page->setSingle( singleColour );
	for ( l = 0; l != h; l++ )
		page->blit( image + width * l, x, y + l, copyw, mode );
}
