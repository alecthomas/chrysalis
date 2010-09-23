/*****************************************************************************
	Controls events (or triggers)

	Author: Alec Thomas
	Update: Sep '95
	Comments:
		I've changed it significantly!
		There are now several distinct types of events
			- proximity triggers
			- area triggers
******************************************************************************/

#include "include.h"
#include <string.h>
#include "event.h"
#include "creature.h"
#include "world.h"

int EVENT::eventcount = 14;
EVENT::EVENTINFO EVENT::eventinfo[] = {
	"Teleport", DT_POINT,
	"Teleport, flash", DT_POINT,
	"Restart marker", DT_NONE,
	"Disappear", DT_OBJECT,
	"Appear", DT_OBJECT,
	"Move", DT_VECTOR | DT_OBJECT,
	"Next level", DT_NONE,
	"Warp to specific level", DT_NUMBER,
	"Damage", DT_NUMBER,
	"Death", DT_NONE,
	"Message", DT_CHAR,
	"Blink every 'n' frames", DT_NUMBER | DT_OBJECT,
	"Blink randomly", DT_OBJECT,
	"Push region", DT_VECTOR
};

EVENT::EVENT( int x, int y, int w, int h, int eventtype )
/* ==================================================
	Make an event that is triggered by a region
*/
{
	CHECK( eventtype > eventcount, "invalid event" );
	EVENT::eventtype = eventtype;
	EVENT::x = x; EVENT::y = y;
	EVENT::w = w; EVENT::h = h;
	reset_data();
	triggertype = TT_REGION;
}

EVENT::EVENT( int x, int y, int radius, int eventtype )
/* ================================================
	Make an event that is triggered by proximity
*/
{
	CHECK( eventtype > eventcount, "invalid event" );
	EVENT::eventtype = eventtype;
	EVENT::x = x; EVENT::y = y;
	proximityradius = radius * radius;
	EVENT::radius = radius;
	reset_data();
	triggertype = TT_PROXIMITY;
}

EVENT::~EVENT()
/* =============
	Die scum, die
*/
{
	if ( objectlistdata ) delete objectlistdata;
	if ( messagedata ) delete messagedata;
}

int EVENT::active( CREATURE *creature )
/* =================
	Returns true if the event gets activated by the creature
*/
{
int xd, yd;

	if ( eventactive ) return TRUE;
	switch ( triggertype )
	{
		case TT_REGION : // entered a region?
			if ( ::touching( creature->getx().integer(), creature->gety().integer(), creature->getw(),
				creature->geth(), x, y, w, h ) ) eventactive = TRUE;
		break;
		case TT_PROXIMITY : // proximity to a point
			xd = x - ( creature->getx().integer() + ( creature->getw() >> 1 ) ); xd *= xd;
			yd = y- ( creature->gety().integer() + ( creature->geth() >> 1 ) ); yd *= yd;
			if ( xd + yd < proximityradius ) eventactive = TRUE;
		break;
		default :
			ERROR( "invalid trigger type" );
	}
	return eventactive;
}

int EVENT::touching( int px, int py )
/* ==========================
	Returns true, if point (px,py) is touching the event (used by editor)
*/
{
int xd, yd;

	switch ( triggertype )
	{
		case TT_REGION :
			if ( ::touching( px, py, x, y, w, h ) ) return TRUE;
		break;
		case TT_PROXIMITY :
			xd = x - px; xd *= xd;
			yd = y - py; yd *= yd;
			if ( xd + yd < proximityradius ) return TRUE;
		break;
		default :
			ERROR( "invalid trigger type" );
	}
	return FALSE;
}


void EVENT::load( FILE *io )
/* ===================
	Load an event
*/
{
}

void EVENT::save( FILE *io )
/* ===================
	Save an event
*/
{
}

void EVENT::reset_data()
/* ===============
	Reset basic data
*/
{
	messagedata = NULL;
	objectlistdata = NULL;
	xdata = ydata = 0;
	eventactive = FALSE;
}

void EVENT::display( int xadj, char colour )
/* ============================
	Draw the trigger...used by the editor
*/
{
	switch ( triggertype )
	{
		case TT_REGION :
			V_box( x - xadj, y, w, h, colour );
		break;
		case TT_PROXIMITY :
			V_putpixel( x - xadj, y, colour );
			V_circle( x - xadj, y, radius, colour );
		break;
	}
}
