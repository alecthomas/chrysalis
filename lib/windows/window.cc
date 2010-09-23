#include <windows/window.h>
#include <windows/manager.h>
#include <string.h>
#include <iostream.h>

Colour Window::face( 0, 64, 100 ), Window::border( 94, 94, 192 ), Window::titleText( 0, 0, 0 ), 
	Window::widget( 94, 94, 192 ), Window::inFocus( 192, 192, 255 );

char Window::CloseButton::imageData[] = {  8, 0, 0, 0, 8, 0, 0, 0, 219, 219, 0, 0, 0, 0, 219, 219,
  0, 219, 219, 0, 0, 219, 219, 0, 0, 0, 219, 219, 219, 219, 0, 0,
  0, 0, 0, 219, 219, 0, 0, 0, 0, 0, 219, 219, 219, 219, 0, 0,
  0, 219, 219, 0, 0, 219, 219, 0, 219, 219, 0, 0, 0, 0, 219, 219,
  0, 0, 0, 0, 0, 0, 0, 0, };

Window::Window( Region *parent, int left, int top, int width, int height )
/* ========================================================================
	Construct a new window
*/
{
	memset( &flags, 1, sizeof( flags ) );
	init( parent, left, top, width, height );
	title = "Untitled";
	name = __FUNCTION__;
}

Window::Window( Region *parent, int left, int top, int width, int height, char *title )
/* ========================================================================
	Construct a new window
*/
{
	memset( &flags, 1, sizeof( flags ) );
	Window::title = title;
	init( parent, left, top, width, height );
	name = __FUNCTION__;
}

Window::Window()
/* ==============
	Default constructor
*/
{
}

unsigned Window::init( Region *parent, int left, int top, int width, int height )
/* ===============================================================================
	Initialise a window
*/
{
int moveWidth;

	memset( &action, 0, sizeof( action ) );
	BufferedRegion::init( parent, left, top, width, height );
	moveWidth = getWidth() - 14;
	font.setPage( page );
	if ( flags.closeButton )
		regions->insert( regions->highest_priority(), ( closeButton = new CloseButton( this, getWidth() - 13, 1, 12, 12 ) ) );
	else
	{
		moveWidth += 12;
		closeButton = NULL;
	}
	regions->insert( regions->highest_priority(), ( titleBar = new TitleBar( this, 1, 1, moveWidth, 12 ) ) );
}

bool Window::focus()
/* ==================
	Focus window
*/
{
	if ( closeButton != NULL ) closeButton->colour = inFocus;
	titleBar->colour = inFocus;
	sendMessage( Region::Redraw );
	manager->sendMessage( Region::PartialDisplay, titleBar->getAbsX(), titleBar->getAbsY(), titleBar->getWidth(), 
		titleBar->getHeight() + 1 );
	if ( closeButton != NULL )
		manager->sendMessage( Region::PartialDisplay, closeButton->getAbsX(), closeButton->getAbsY(), closeButton->getWidth(), 
			closeButton->getHeight() + 1 );
	return false;
}

bool Window::unFocus()
/* ====================
	Un focus window
*/
{
	if ( closeButton != NULL ) closeButton->colour = widget;
	titleBar->colour = widget;
	sendMessage( Region::Redraw );
	manager->sendMessage( Region::PartialDisplay, titleBar->getAbsX(), titleBar->getAbsY(), titleBar->getWidth(), 
		titleBar->getHeight() + 1 );
	if ( closeButton != NULL )
		manager->sendMessage( Region::PartialDisplay, closeButton->getAbsX(), closeButton->getAbsY(), closeButton->getWidth(), 
			closeButton->getHeight() + 1 );
	return false;
}

bool Window::redraw()
/* ===================
	Redraw the window
*/
{
int t = 0, h = height;

	page->filledBox( 0, 0, width, height, face );
	page->putPixel( width - 1, 0, border );
	page->putPixel( 0, height - 1, border );
	page->line( 1, t + 1, width - 2, t + 1, border );
	page->line( 1, t + 1, 1, t + height - 2, border );
	page->line( 1, t + height - 2, width - 2, t + height - 2, border );
	page->line( width - 2, t + 1, width - 2, t + height - 2, border );
	page->line( 0, t, width - 2, t, border * 2 );
	page->line( 0, t, 0, t + h - 2, border * 2 );
	page->line( 1, t + h - 1, width - 1, t + h - 1, border / 2 );
	page->line( width - 1, t + 1, width - 1, t + h - 1, border / 2 );
	if ( titleBar != NULL )
	{
		t += 11;
		h -= 11;
	}
	page->putPixel( 2, t + h - 3, border );
	page->putPixel( width - 3, t + 2, border );
	page->line( 2, t + 2, width - 4, t + 2, titleBar->colour / 2 );
	page->line( 2, t + 2, 2, t + h - 4, border / 2 );
	page->line( 3, t + h - 3, width - 3, t + h - 3, border * 2 );
	page->line( width - 3, t + 3, width - 3, t + h - 3, border * 2 );
	return false;
}

bool Window::partialRedraw( int x, int y, int w, int h )
{
	redraw();
	return false;
}

bool Window::mousePress( unsigned button, int x, int y )
{
	return false;
}

bool Window::mouseRelease( unsigned button, int x, int y )
{
	return false;
}

bool Window::mouseMove( int oldX, int oldY, int x, int y )
{
	return false;
}

Window::CloseButton::CloseButton( Window *window, int x, int y, int w, int h )
{
	init( window, x, y, w, h );
	name = __FUNCTION__; 
	colour = window->widget;
	name = __FUNCTION__;
	image = new Image( imageData[ 0 ], imageData[ 4 ] );
	image->setPage( page );
	memcpy( image->buffer, imageData + 8, (int)imageData[ 0 ] * (int)imageData[ 4 ] );
	image->setSingle( Colour( 1, 2, 3 ) ); 
	image->setMode( Page::SINGLE );
}

Window::CloseButton::~CloseButton()
{
	delete image;
}

void Window::CloseButton::triggered()
{
	manager->removeRegion( parent );
}

Window::TitleBar::TitleBar( Window *window, int x, int y, int w, int h )
{
	init( window, x, y, w, h );
	colour = window->widget;
	font.setMode( Page::SINGLE );
	font.setSingle( window->titleText );
	text = window->getTitle();
	name = __FUNCTION__;
	attachedTo = window;
}

void Window::TitleBar::triggered()
{
}

bool Window::TitleBar::redraw()
{
int l, textWidth;

	TextButton::redraw();
	textWidth = font.width( text );
	if ( action.drawPressed )
		for ( l = 0; l != ( height - 4 ) / 2; l++ )
		{
			page->line( 3, l * 2 + 3, 6, l * 2 + 3, titleText );
			page->line( 11 + textWidth, l * 2 + 3, width - 3, l * 2 + 3, titleText );
		}
	else
		for ( l = 0; l != ( height - 4 ) / 2; l++ )
		{
			page->line( 2, l * 2 + 2, 6, l * 2 + 2, titleText );
			page->line( 10 + textWidth, l * 2 + 2, width - 3, l * 2 + 2, titleText );
		}
	return false;
}

bool Window::TitleBar::mousePress( unsigned button, int x, int y )
{
	if ( !Button::mousePress( button, x, y ) ) return false;
	moveDisplaceX = x - attachedTo->getAbsX();
	moveDisplaceY = y - attachedTo->getAbsY();
	manager->lockFocus( this );
	return true;
}

bool Window::TitleBar::mouseRelease( unsigned button, int x, int y )
{
	TextButton::mouseRelease( button, x, y );
	manager->unlockFocus( this );
	manager->generateVisibilityMap();
	return true;
}

bool Window::TitleBar::mouseMove( int oldX, int oldY, int x, int y )
{
	if ( !action.pressed ) return false;
	attachedTo->setPosition( attachedTo->getX() + x - oldX, attachedTo->getY() + y - oldY );
	if ( x - oldX > 0 )	// moved right
		manager->sendMessage( Region::PartialDisplay, oldX - moveDisplaceX, oldY - moveDisplaceY, x - oldX, attachedTo->getHeight() );
	else	// moved left
		manager->sendMessage( Region::PartialDisplay, oldX - moveDisplaceX + attachedTo->getWidth() - ( oldX - x ), oldY - moveDisplaceY, 
			oldX - x, attachedTo->getHeight() );

	if ( y - oldY > 0 ) // moved down
		manager->sendMessage( Region::PartialDisplay, oldX - moveDisplaceX, oldY - moveDisplaceY, attachedTo->getWidth(), y - oldY );
	else	// moved up
		manager->sendMessage( Region::PartialDisplay, oldX - moveDisplaceX, oldY - moveDisplaceY + attachedTo->getHeight() - ( oldY - y ), 
			attachedTo->getWidth(), oldY - y );
	sendMessage( Region::ReverseDisplay );
	manager->generateVisibilityMap();
	return true;
}
