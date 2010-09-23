/******************************************************************************
	Region header file

	Author: Alec Thomas
	Date: Sep 29th, '95
	Comments: 
		- Never, ever call Region::init() from a constructor.
		- Default behaviour of Region::send() is to return false, this is
		  because it should be overloaded by user defined classes
******************************************************************************/

#include <windows/region.h>
#include <windows/manager.h>
#include <general.h>
#include <iostream.h>

// As each new region's init() function is called, this variable is incremented
unsigned Region::idCount = 0;

Region::Region()
/* ==============
	Empty constructor...g++ requires it for some reason
*/
{
}

Region::~Region()
/* ===============
	Destructor...doesn't do squat
*/
{
}

unsigned Region::init( Region *p, int x, int y, int w, int h )
/* ============================================================
	Initialisation routine, should NOT be called by constructor 
*/
{
	top = y; left = x; width = w; height = h; parent = p; 
	if ( p != NULL ) 
	{
		manager = p->manager;
		page = p->page;
		font.alias( &p->font );
	}
	if ( ( regions = new QUEUE <Region> () ) == NULL ) return false;
	// Information stuff, for identifying types, debugging, etc.
	id = idCount++;
	type = -1;						// invalid type...can't actually use a region directly
	name = "Region";
	return id;
}

void Region::deInit()
/* ===================
	De initialise a region, should be called instead of destructor. Return non-zero value on error.
*/
{
	delete regions;
}

bool Region::addRegion( Region *region )
/* ======================================
	Add a new sub-region
*/
{
	regions->insert( regions->lowest_priority(), region );
}

void Region::setManager( Manager *manager )
/* =========================================
	Set the manager for the region and all sub-regions
*/
{
Region *region;

	Region::manager = manager;
	if ( regions == NULL ) return;
	region = regions->reset();
	while ( region != NULL )
	{
		region->setManager( manager );
		region = regions->next();
	}
}

bool Region::setPosition( int x, int y )
/* ======================================
	Reposition region
*/
{
	left = x; top = y;
	return true;
}

int Region::getAbsX() 
/* ====================
	Return absolute X position of region
*/
{ 
	if ( parent == NULL ) return left; 
	return parent->getAbsX() + left; 
}

int Region::getAbsY() 
/* ====================
	Return absolute Y position of region
*/
{ 
	if ( parent == NULL ) return top; 
	return parent->getAbsY() + top; 
}

void Region::dumpStats()
/* ======================
	Dump information about a window
*/
{
    clog << name << " " << id << ":(" << getAbsX() << "," << getAbsY() << ")-(" << getAbsX() + width << "," << getAbsY() + height << ") (" <<
		width << "x" << height << ")" << endl;
}

/*
	Notes about sending messages to regions:
		- Most messages are depth first. That is, the message gets sent to the innermost region FIRST.
*/

bool Region::sendMessage( MessageType t )
/* =======================================
	Send messages that don't require parameters
*/
{
bool retVal, depthFirst = true;
Region *region;

	switch ( t )
	{
		case Run :
		case Redraw :
		case Display :
		case Focus :
		case UnFocus :
			depthFirst = false;
		break;
		case ReverseDisplay :
			retVal = reverseDisplay();
			return retVal;
		break;
		case Close :
		break;
	}

	/* Recurse BEFORE sending message if depthFirst */
	if ( depthFirst )
	{
		region = regions->reset();
		while ( region != NULL )
		{
			retVal = region->sendMessage( t );
			if ( retVal ) return true;
			region = regions->next();
		}
	}

	/* Actually send message */
	switch ( t )
	{
		case Run :
			retVal = run();
		break;
		case Redraw :
			retVal = redraw();
		break;
		case Display :
			retVal = display();
		break;
		case Close :
			retVal = close();
		break;
		case Focus :
			retVal = focus();
		break;
		case UnFocus :
			retVal = unFocus();
		break;
		default :
			manager->halt( -1, "Invalid number of arguments in message" );
		break;
	}

	/* Recurse AFTER message if NOT depthFirst */
	if ( !depthFirst )
	{
		region = regions->reset();
		while ( region != NULL )
		{
			retVal = region->sendMessage( t );
			if ( retVal ) return true;
			region = regions->next();
		}
	}
	return false;
}

bool Region::sendMessage( MessageType t, int a )
/* =======================================
	Send messages that take 1 parameter
*/
{
bool retVal, depthFirst = true;
Region *region;


	switch ( t )
	{
		case KeyPress :
		case KeyRelease :
		break;
	}

	/* Recurse BEFORE sending message if depthFirst */
	if ( depthFirst )
	{
		region = regions->reset();
		while ( region != NULL )
		{
			retVal = region->sendMessage( t, a );
			if ( retVal ) return true;
			region = regions->next();
		}
	}
	/* Actually send message */
	switch ( t )
	{
		case KeyPress :
			retVal = keyPress( a );
		break;
		case KeyRelease :	// equivalent
			retVal = keyRelease( a );
		break;
		default :
			manager->halt( -1, "Invalid number of arguments in message" );
		break;
	}
	/* Recurse AFTER message if NOT depthFirst */
	if ( !depthFirst )
	{
		region = regions->reset();
		while ( region != NULL )
		{
			retVal = region->sendMessage( t, a );
			if ( retVal ) return true;
			region = regions->next();
		}
	}
	return false;
}

bool Region::sendMessage( MessageType t, int a, int b )
/* =======================================
	Send messages that take 2 parameters
*/
{
bool retVal, depthFirst = true;
Region *region;

	switch ( t )
	{
		case Ping :
		case MousePosition :
		break;
	}
	/* Recurse BEFORE sending message if depthFirst */
	if ( depthFirst )
	{
		region = regions->reset();
		while ( region != NULL )
		{
			retVal = region->sendMessage( t, a, b );
			if ( retVal ) return true;
			region = regions->next();
		}
	}
	/* Actually send message */
	switch ( t )
	{
		case MousePosition :
			retVal = mousePosition( a, b );
		break;
		case Ping :
			retVal = ping( a, b );
		break;
		default :
			manager->halt( -1, "Invalid number of arguments in message" );
		break;
	}
	/* Recurse AFTER message if NOT depthFirst */
	if ( !depthFirst )
	{
		region = regions->reset();
		while ( region != NULL )
		{
			retVal = region->sendMessage( t, a, b );
			if ( retVal ) return true;
			region = regions->next();
		}
	}
	return false;
}

bool Region::sendMessage( MessageType t, int a, int b, int c )
/* =======================================
	Send messages that take 3 parameters
*/
{
bool retVal, depthFirst = true;
Region *region;

	switch ( t )
	{
		case MousePress :
		case MouseRelease : // equivalent
		break;
	}
	/* Recurse BEFORE sending message if depthFirst */
	if ( depthFirst )
	{
		region = regions->reset();
		while ( region != NULL )
		{
			retVal = region->sendMessage( t, a, b, c );
			if ( retVal ) return true;
			region = regions->next();
		}
	}
	/* Actually send message */
	switch ( t )
	{
		case MousePress :
			retVal = mousePress( a, b, c );
		break;
		case MouseRelease : // equivalent
			retVal = mouseRelease( a, b, c );
		break;
		default :
			manager->halt( -1, "Invalid number of arguments in message" );
		break;
	}
	/* Recurse AFTER message if NOT depthFirst */
	if ( !depthFirst )
	{
		region = regions->reset();
		while ( region != NULL )
		{
			retVal = region->sendMessage( t, a, b, c );
			if ( retVal ) return true;
			region = regions->next();
		}
	}
	return false;
}

bool Region::sendMessage( MessageType t, int a, int b, int c, int d )
/* =======================================
	Send messages that take 4 parameters
*/
{
bool retVal, depthFirst = true;
Region *region;

	switch ( t )
	{
		case PartialDisplay :
		case PartialRedraw :
			depthFirst = false;
		break;
		case ReversePartialDisplay :
			retVal = reversePartialDisplay( a, b, c, d );
			return retVal;
		break;
		case MouseMove :
		break;
	}
	/* Recurse BEFORE sending message if depthFirst */
	if ( depthFirst )
	{
		region = regions->reset();
		while ( region != NULL )
		{
			retVal = region->sendMessage( t, a, b, c, d );
			if ( retVal ) return true;
			region = regions->next();
		}
	}
	/* Actually send message */
	switch ( t )
	{
		case MouseMove :
			retVal = mouseMove( a, b, c, d );
		break;
		case PartialRedraw :
			retVal = partialRedraw( a, b, c, d );
		break;
		case PartialDisplay :
			retVal = partialDisplay( a, b, c, d );
		break;
		break;
		default :
			manager->halt( -1, "Invalid number of arguments in message" );
		break;
	}
	/* Recurse AFTER message if NOT depthFirst */
	if ( !depthFirst )
	{
		region = regions->reset();
		while ( region != NULL )
		{
			retVal = region->sendMessage( t, a, b, c, d );
			if ( retVal ) return true;
			region = regions->next();
		}
	}
	return retVal;
}

/* Message handling methods */

bool Region::focus()
{
	return true;
}

bool Region::unFocus()
{
	return true;
}

bool Region::run()
{
	return false;
}

bool Region::keyPress( unsigned key )
{
	return false;
}

bool Region::keyRelease( unsigned key )
{
	return false;
}

bool Region::mousePosition( int x, int y )
{
	return false;
}

bool Region::mouseMove( int oldX, int oldY, int x, int y )
{
	return false;
}

bool Region::mousePress( unsigned button, int x, int y )
{
	return false;
}

bool Region::mouseRelease( unsigned button, int x, int y )
{
	return false;
}

bool Region::redraw()
{
	return false;
}

bool Region::partialRedraw( int x, int y, int w, int h )
{
	return false;
}

bool Region::display()
{
	return false;
}

bool Region::partialDisplay( int x, int y, int w, int h )
{
	return false;
}

bool Region::reverseDisplay()
{
	if ( parent != NULL ) parent->sendMessage( Region::ReverseDisplay );
//&& parent != manager 
	display();
	return false;
}

bool Region::reversePartialDisplay( int x, int y, int w, int h )
{
	if ( parent != NULL ) parent->sendMessage( Region::ReversePartialDisplay, x, y, w, h );
//&& parent != manager 
	partialDisplay( x, y, w, h );
	return false;
}

bool Region::close()
{
	deInit();
	return false;
}

bool Region::ping( int x, int y )
{
	return touching( x, y, left, top, width, height );
}

BufferedRegion::~BufferedRegion()
{
	delete page;
}

unsigned BufferedRegion::init( Region *parent, int x, int y, int w, int h )
{
unsigned retVal;

	retVal = Region::init( parent, x, y, w, h );
	colour = Colour( 128, 128, 200 );
	page = new Page( w, h );
	font.setPage( page );
	return retVal;
}

bool BufferedRegion::display()
{
	page->dump( parent->getPage(), left, top );
	return false;
}

bool BufferedRegion::partialDisplay( int x, int y, int w, int h )
{
	if ( !touching( x, y, w, h, getAbsX(), getAbsY(), width, height ) ) return false;
	page->dump( parent->getPage(), left, top, x, y, w, h );
	return false;
}

bool BufferedRegion::redraw()
{
	page->filledBox( 0, 0, width, height, colour );
	return false;
}

bool BufferedRegion::partialRedraw( int x, int y, int w, int h )
{
	if ( !touching( x, y, w, h, getAbsX(), getAbsY(), width, height ) ) return false;
	page->filledBox( x, y, w, h, colour );
	return false;
}
