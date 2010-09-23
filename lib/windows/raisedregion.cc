#include <windows/raisedregion.h>
#include <windows/window.h>
#include <cc/partialpage.h>

RaisedRegion::RaisedRegion( Region *r, int x, int y, int w, int h )
{
	init( r, x, y, w, h );
}

RaisedRegion::~RaisedRegion()
{
	delete page;
}

unsigned RaisedRegion::init( Region *r, int x, int y, int w, int h )
{
unsigned retVal;

	retVal = Region::init( r, x, y, w, h );
	page = new PartialPage( r->getPage(), x, y, w, h );
	indented = false;
	colour = Window::face;
	return retVal;
}

bool RaisedRegion::redraw()
{
	if ( indented )
	{
		page->line( 0, 0, width - 2, 0, colour / 2 );
		page->line( 0, 0, 0, height - 2, colour / 2 );
		page->line( 1, height - 1, width - 1, height - 1, colour * 2 );
		page->line( width - 1, 1, width - 1, height - 1, colour * 2 );
	} else
	{
		page->line( 0, 0, width - 2, 0, colour * 2 );
		page->line( 0, 0, 0, height - 2, colour * 2 );
		page->line( 1, height - 1, width - 1, height - 1, colour / 2 );
		page->line( width - 1, 1, width - 1, height - 1, colour / 2 );
	}
	return false;
}

bool RaisedRegion::display()
{
	return redraw();
}

bool RaisedRegion::partialDisplay( int x, int y, int w, int h )
{
	if ( !touching( x, y, w, h, left, top, width, height ) ) return false;
	return redraw();
}
