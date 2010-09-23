#include <windows/window.h>
#include <windows/scrollbar.h>
#include <windows/button.h>
#include <cc/image.h>

char ScrollBar::leftArrow[] = {  8, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 219, 219, 0, 0, 0, 0, 219, 219, 219, 219, 0, 0,
  219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
  0, 0, 219, 219, 219, 219, 0, 0, 0, 0, 0, 0, 219, 219, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, };

char ScrollBar::rightArrow[] = {  8, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 219, 219, 0, 0, 0, 0, 0, 0, 219, 219, 219, 219, 0, 0,
  219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
  0, 0, 219, 219, 219, 219, 0, 0, 0, 0, 219, 219, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, };

char ScrollBar::upArrow[] = {  8, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 219, 219, 0, 0, 0,
  0, 0, 0, 219, 219, 0, 0, 0, 0, 0, 219, 219, 219, 219, 0, 0,
  0, 0, 219, 219, 219, 219, 0, 0, 0, 219, 219, 219, 219, 219, 219, 0,
  0, 219, 219, 219, 219, 219, 219, 0, 0, 0, 0, 219, 219, 0, 0, 0,
  0, 0, 0, 219, 219, 0, 0, 0, };
 
char ScrollBar::downArrow[] = {  8, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 219, 219, 0, 0, 0,
  0, 0, 0, 219, 219, 0, 0, 0, 0, 219, 219, 219, 219, 219, 219, 0,
  0, 219, 219, 219, 219, 219, 219, 0, 0, 0, 219, 219, 219, 219, 0, 0,
  0, 0, 219, 219, 219, 219, 0, 0, 0, 0, 0, 219, 219, 0, 0, 0,
  0, 0, 0, 219, 219, 0, 0, 0, };

ScrollBar::ScrollBar( Region *parent, int x, int y, int w, int h, bool horizontal )
{
	ScrollBar::horizontal = horizontal;
	init( parent, x, y, w, h );
}

ScrollBar::~ScrollBar()
{
	delete down;
	delete up;
	delete right;
	delete left;
}

unsigned ScrollBar::init( Region *parent, int x, int y, int w, int h )
{
	Region::init( parent, x, y, w, h );
	colour = Window::widget;
	if ( horizontal )
	{
		left = new Image( leftArrow[ 0 ], leftArrow[ 4 ] );
		memcpy( left->buffer, leftArrow + 8, (int)leftArrow[ 0 ] * (int)leftArrow[ 4 ] );
		right = new Image( rightArrow[ 0 ], rightArrow[ 4 ] );
		memcpy( right->buffer, rightArrow + 8, (int)rightArrow[ 0 ] * (int)rightArrow[ 4 ] );
		left->setMode( Page::SINGLE );
		left->setSingle( Colour( 255, 255, 255 ) );
		right->setMode( Page::SINGLE );
		right->setSingle( Colour( 255, 255, 255 ) );
		addRegion( new ImageButton( this, x, y, left ) );
		addRegion( new ImageButton( this, x + w - 14, y, right ) );
		addRegion( new Button( this, x + 14, y, 24, 14 ) );
	} else
	{
		up = new Image( upArrow[ 0 ], upArrow[ 4 ] );
		memcpy( up->buffer, upArrow + 8, (int)upArrow[ 0 ] * (int)upArrow[ 4 ] );
		down = new Image( downArrow[ 0 ], downArrow[ 4 ] );
		memcpy( down->buffer, downArrow + 8, (int)downArrow[ 0 ] * (int)downArrow[ 4 ] );
		up->setMode( Page::SINGLE );
		up->setSingle( Colour( 255, 255, 255 ) );
		down->setMode( Page::SINGLE );
		down->setSingle( Colour( 255, 255, 255 ) );
		addRegion( new ImageButton( this, x, y, up ) );
		addRegion( new ImageButton( this, x, y + h - 14, down ) );
		addRegion( new Button( this, x, y + 14, 14, 24 ) );
	}
}

bool ScrollBar::display()
{
	return redraw();
}
