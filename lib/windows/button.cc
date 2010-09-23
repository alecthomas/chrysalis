#include <windows/window.h>
#include <windows/button.h>
#include <windows/manager.h>
#include <cc/partialpage.h>
#include <iostream.h>

Button::Button( Region *parent, int x, int y, int w, int h )
/* ==========================================================
	Button constructor
*/
{
	init( parent, x, y, w, h );
	name = __FUNCTION__;
}

Button::~Button()
/* ===============
	Delete button
*/
{
	deInit();
}

unsigned Button::init( Region *parent, int x, int y, int w, int h )
/* =================================================================
	Initialise button
*/
{
unsigned retVal; 

	memset( &action, 0, sizeof( action ) );
	retVal = Region::init( parent, x, y, w, h );
	colour = Window::widget;
	page = new PartialPage( parent->getPage(), x, y, w, h );
	font.alias( &parent->font );
	font.setPage( page );
	return retVal;
}

void Button::deInit()
/* ===================
	Deinit button, free partial page
*/
{
	delete page;
}

bool Button::redraw()
/* ====================
	Redraw button
*/
{
	page->filledBox( 0, 0, width, height, colour );
	if ( action.drawPressed )
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

bool Button::display()
/* ====================
	Display button
*/
{
	return redraw();
}

bool Button::partialDisplay( int x, int y, int w, int h )
/* =======================================================
	Display button
*/
{
	if ( !touching( left, top, width, height, x, y, w, h ) ) return false;
	return redraw();
}

bool Button::mousePress( unsigned button, int x, int y )
/* ======================================================
	Mouse button pressed
*/
{
	if ( !touching( x, y, getAbsX(), getAbsY(), width, height ) ) return false;
	action.pressed = action.drawPressed = true;
	sendMessage( Region::Redraw );
	sendMessage( Region::ReversePartialDisplay, getAbsX(), getAbsY(), width, height );
	manager->lockFocus( this );
	return true;
}

bool Button::mouseRelease( unsigned button, int x, int y )
/* ========================================================
	Mouse button released
*/
{
bool oldPressed = action.drawPressed;

	manager->unlockFocus( this );
	action.pressed = action.drawPressed = false;
	if ( oldPressed != action.drawPressed )
	{
		sendMessage( Region::Redraw );
		sendMessage( Region::ReversePartialDisplay, getAbsX(), getAbsY(), width, height );
	}
	if ( touching( x, y, getAbsX(), getAbsY(), width, height ) ) triggered();
	return true;
}

bool Button::mouseMove( int oldX, int oldY, int x, int y )
/* =======================================================
	Display button
*/
{
bool oldPressed = action.drawPressed;

	if ( !action.pressed ) return false;
	if ( action.drawPressed && !touching( x, y, getAbsX(), getAbsY(), width, height ) )
		action.drawPressed = false;
	if ( !action.drawPressed && touching( x, y, getAbsX(), getAbsY(), width, height ) )
		action.drawPressed = true;
	if ( oldPressed != action.drawPressed )
	{
		sendMessage( Region::Redraw );
		sendMessage( Region::ReversePartialDisplay, getAbsX(), getAbsY(), width, height );
	}
	return true;
}

void Button::triggered()
/* ====================
	Trigger the button
*/
{
}

TextButton::TextButton( Region *parent, int x, int y, char *text )
{
	font.alias( &parent->font );
	width = font.width( text ) + 8; height = font.height( text ) + 4;
	TextButton::text = text;
	init( parent, x, y, width, height );
	name = __FUNCTION__;
}

unsigned TextButton::init( Region *parent, int x, int y, int w, int h )
{
unsigned retVal;

	retVal = Button::init( parent, x, y, w, h );
	textColour = Window::titleText;
	font.setMode( Page::SINGLE );
	font.setSingle( textColour );
	return retVal;
}

bool TextButton::redraw()
{
	Button::redraw();
	if ( action.drawPressed )
	{
		font.moveTo( 10, 2 );
		font << text;
	} else
	{
		font.moveTo( 9, 1 );
		font << text;
	}
	return false;
}

ImageButton::ImageButton( Region *parent, int x, int y, Image *image )
{
	init( parent, x, y, image->width + 4, image->height + 4 );
	ImageButton::image = image;
	name = __FUNCTION__;
}

bool ImageButton::redraw()
{
	Button::redraw();
	if ( action.drawPressed ) 
		image->put( 3, 3 );
	else
		image->put( 2, 2 );
	return false;
}
