#include <cc/partialpage.h>
#include <iostream.h>

PartialPage::PartialPage( Page *page, int x, int y, int w, int h )
{
	left = x; top = y;
	width = w; height = h;
	parent = page;
	yTable = NULL; buffer = NULL;
}

PartialPage::~PartialPage()
{
}

char *PartialPage::read( int row )
{
	return parent->read( left, top + row, width );
}

char *PartialPage::read( int column, int row, int width )
{
	return parent->read( left + column, top + row, width );
}

void PartialPage::write( char *source, int row )
{
	parent->write( source, left, top + row, width );
}

void PartialPage::write( char *source, int column, int row, int width )
{
	parent->write( source, left + column, top + row, width );
}

void PartialPage::blit( char *source, int column, int row, int width, unsigned mode )
{
	parent->blit( source, left + column, top + row, width, mode );
}

void PartialPage::clear( Colour const &colour )
{
	filledBox( 0, 0, width, height, colour );
}

void PartialPage::filledBox( int x, int y, int w, int h, Colour colour )
{
char *i;
int l;

	if ( !clip( i, x, y, w, h, width, height ) ) return;
	for ( l = 0; l != h; l++ )
		parent->filledBox( left + x, top + l + y, w, 1, colour );
}

void PartialPage::putPixel( int column, int row, Colour colour )
{
	if ( column < 0 || column >= width || row < 0 || row >= height ) return;
	parent->putPixel( left + column, top + row, colour );
}

void PartialPage::setSingle( Colour colour )
{
	singleColour = colour;
	parent->setSingle( colour );
}
