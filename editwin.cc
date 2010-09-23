/*==============================================================================
								Movable window items
==============================================================================*/

#include "include.h"
#include "data.h"
#include "editwin.h"
#include "editor.h"
#include "event.h"

ICONS::ICONS()
/* ============
	Set up icon data stuff ;)
*/
{
	title = "Icon bar";
	x = INI_icon.x; y = INI_icon.y;
	w = 180; h = 36;
	iconindex[ 0 ][ 0 ] = EDITOR_LOAD;
	iconindex[ 1 ][ 0 ] = EDITOR_SAVE;
	iconindex[ 2 ][ 0 ] = EDITOR_TYPE;
	iconindex[ 3 ][ 0 ] = EDITOR_MODE;
	iconindex[ 4 ][ 0 ] = EDITOR_PLANE;
	iconindex[ 5 ][ 0 ] = -1;
	iconindex[ 6 ][ 0 ] = EDITOR_FLUSH;
	iconindex[ 7 ][ 0 ] = -1;
	iconindex[ 8 ][ 0 ] = EDITOR_CANCEL;
	iconindex[ 9 ][ 0 ] = EDITOR_HELP;
	// second row
	iconindex[ 0 ][ 1 ] = EDITOR_ATTRIBMARKED;
	iconindex[ 1 ][ 1 ] = EDITOR_SOLIDSCN;
	iconindex[ 2 ][ 1 ] = EDITOR_SCENLAYE;
	iconindex[ 3 ][ 1 ] = EDITOR_EVENT;
	iconindex[ 4 ][ 1 ] = -1;
	iconindex[ 5 ][ 1 ] = -1;
	iconindex[ 6 ][ 1 ] = -1;
	iconindex[ 7 ][ 1 ] = -1;
	iconindex[ 8 ][ 1 ] = EDITOR_SCENBACK;
	iconindex[ 9 ][ 1 ] = EDITOR_SCENFORW;
}

void ICONS::display( int top )
/* ===================
	Display the icons
*/
{
int l, l1, mode;

	WINDOW::display( top );
	for ( l = 0; l != 10; l++ )
		for ( l1 = 0; l1 != 2; l1++ )
		{
			mode = MASK;
			fbox3D( x + l * 18, y + l1 * 18,
				18, 18, TRUE );
			if ( !l1 )
				switch( l )	// top row
				{
					case 2 :
						if ( !ICON_PLACE || ICON_LAYER == 4 ) mode = FADE( -6 );
					break;
					case 4 :
						if ( ICON_EVENT ) mode = FADE( -6 );
					break;
					case 6 :
						if ( marked->items() == 0 || ICON_LAYER == 4 ) mode = FADE( -6 );
					break;
				}
			else
				switch( l )	// bottom row
				{
					case 3 :
						if  ( !ICON_EVENT || ICON_LAYER == 4 ) mode = FADE( -6 );
					break;
					case 0 :
						if ( !ICON_MARKED || !marked->items() || ICON_LAYER == 4 || markedsetwindow != NULL ) mode = FADE( -6 );
					break;
					case 1 :
					case 2 :
					case 8 :
					case 9 :
						if ( !ICON_PLACE || ICON_LAYER == 4 ) mode = FADE( -6 );
					break;
				}
				if ( iconindex[ l ][ l1 ] != -1 )
					eimage->put( x + 1 + l * 18, y + 1 + l1 * 18,
						iconindex[ l ][ l1 ], button[ l ][ l1 ], mode );
		}
}

void ICONS::press( int x, int y, int buttonpressed )
/* ===============================
	Press an icon
*/
{
OBJECT *o;
int l, tx, ty;
QUEUE <OBJECT> *tq;

	x /= 18; y /= 18;
	if ( iconindex[ x ][ y ] == -1 ) return;
	if ( y == 0 )
	{ // top row
		switch( x )
		{
			case 0 :	// load
//				windows->insert( windows->lowest_priority() - 1, loadwindow );
				reset_all();
				image->reset_cache();
				world->load( WORLD::firstlevel );
			break;
			case 1 :	// save
//				windows->insert( windows->lowest_priority() - 1, savewindow );
				world->save( WORLD::firstlevel );
			break;
			case 2 :	// use scenery
				if ( !ICON_PLACE || ICON_LAYER == 4 ) break;
				ICON_TYPE = ( ICON_TYPE + 1 ) & 1;
				if ( ICON_TYPE == 0 )
					// placing scenery
					while ( !strncmp( image->name( currentobject ), "CREATURE", 7 ) )
					{
						currentobject++;
						if ( currentobject >= image->images() ) currentobject = 0;
					}
				else
					// placing dudes ;)
					while ( strncmp( image->name( currentobject ), "CREATURE", 7 ) )
					{
						currentobject++;
						if ( currentobject >= image->images() ) currentobject = 0;
					}
			break;
			case 3 :	// marked or cursor objects
				ICON_MODE = ( ICON_MODE + 1 ) % 3;
				reset_all();
			break;
			case 4 :	// change layer
				if ( ICON_EVENT ) break;
				reset_all();
				if ( buttonpressed == 2 )
					if ( ICON_LAYER == 4 )
						ICON_LAYER = 0;
					else
						ICON_LAYER = 4;
				else
					if ( ICON_LAYER != 4 )
						ICON_LAYER = ( ICON_LAYER + 1 ) & 3;
			break;
			case 6 :	// delete
				if ( marked->items() == 0 || ICON_LAYER == 4 ) break;
				tx = ICONS::x + x * 18 + 9 + world->layer->worldposition;
				ty = ICONS::y + y * 18 + 9;
				tq = new QUEUE <OBJECT>();
				o = marked->reset();
				while ( o != NULL )
				{
					world->layer->naturalQ->remove( o );
					tq->insert( marked->priority(), o );
					o = marked->next();
				}
				world->layer->reset();
				// flush them baby ;)
				for ( l = 0; l != 16; l++ )
				{
					display_world();
					o = tq->reset();
					while ( o != NULL )
					{
						o->setposition(
							o->getx() + ( tx - ( o->getx() + ( o->getw() >> 1 ) ) ) / ( 16 - l ),
							o->gety() + ( ty - ( o->gety() + ( o->geth() >> 1 ) ) ) / ( 16 - l ) );
						o->display( world->layer->worldposition );
						o = tq->next();
					}
					display_windows();
					if ( l < 6 )
						button[ x ][ y ] = l;
					else
						button[ x ][ y ] = 3 + l % 3;
					V_put( mousex, mousey, mouse, MASK );
					display_to_video();
				}
				// Delete object's from world
				o = marked->reset();
				while ( o != NULL )
				{
					delete o;
					o = marked->next();
				}
				reset_all();
				delete tq;
				// Close toilet ;)
				for ( l = 0; l != 4; l++ )
				{
					button[ x ][ y ] = 3 - l;
					display_all();
					V_put( mousex, mousey, mouse, MASK );
					display_to_video();
				}
			break;
			case 8 :	// cancel
			break;
			case 9 :	// help
			break;
		}
	} else
	{ // bottom row
		switch( x )
		{
			case 0 :	// marked object attributes
				if ( !ICON_MARKED || !marked->items() || ICON_LAYER == 4 || markedsetwindow != NULL ) break;
				windows->insert( windows->lowest_priority() - 1, ( markedsetwindow = new MARKEDATTRIBSET() ) );
			break;
			case 1 :	// solid scenery on/off
				if ( !ICON_PLACE || ICON_LAYER == 4 ) break;
				ICON_SOLID = ( ICON_SOLID + 1 ) & 1;
			break;
			case 2 :	// place scenery in front of or behind existing scenery
				if ( !ICON_PLACE || ICON_LAYER == 4 ) break;
				ICON_BACK = ( ICON_BACK + 1 ) & 1;
			break;
			case 3 :	// switch event trigger modes
				if ( !ICON_EVENT || ICON_LAYER == 4 ) break;
				ICON_TRIGGER = ( ICON_TRIGGER + 1 ) & 1;
			break;
			case 8 :	// previous scenery
				if ( !ICON_PLACE || ICON_LAYER == 4 ) break;
				if ( buttonpressed == 2 )
					currentobject = 0;
				else
				{
					currentobject--;
					if ( currentobject < 0 ) currentobject = image->images() - 1;
				}
				if ( ICON_TYPE == 0 )
					// placing scenery
					while ( !strncmp( image->name( currentobject ), "CREATURE", 7 ) )
					{
						currentobject--;
						if ( currentobject < 0 ) currentobject = image->images() - 1;
					}
				else
					// placing dudes ;)
					while ( strncmp( image->name( currentobject ), "CREATURE", 7 ) )
					{
						currentobject--;
						if ( currentobject < 0 ) currentobject = image->images() - 1;
					}
			break;
			case 9 :	// next scenery
				if ( !ICON_PLACE || ICON_LAYER == 4 ) break;
				if ( buttonpressed == 2 )
					currentobject = image->images() - 1;
				else
				{
					currentobject++;
					if ( currentobject == image->images() ) currentobject = 0;
				}
				if ( ICON_TYPE == 0 )
					// placing scenery
					while ( !strncmp( image->name( currentobject ), "CREATURE", 7 ) )
					{
						currentobject++;
						if ( currentobject >= image->images() ) currentobject = 0;
					}
				else
					// placing dudes ;)
					while ( strncmp( image->name( currentobject ), "CREATURE", 7 ) )
					{
						currentobject++;
						if ( currentobject >= image->images() ) currentobject = 0;
					}
			break;
		}
	}
}

OBJECTVIEWER::OBJECTVIEWER()
{
	title = "Object viewer";
	w = image->getw( currentobject ) + 2;
	h = image->geth( currentobject ) + 2;
	x = INI_objectviewer.x; y = INI_objectviewer.y;
}

void OBJECTVIEWER::display( int top )
{
	w = image->getw( currentobject ) + 2;
	h = image->geth( currentobject ) + 2;
	WINDOW::display( top );
	fbox3D( x, y, w, h, TRUE );
	image->put( x + 1, y + 1, currentobject, MASK );
}

void OBJECTVIEWER::press( int x, int y, int button )
{
}

SCROLLBAR::SCROLLBAR()
{
	title = "Scroll bar";
	x = INI_scrollbar.x; y = INI_scrollbar.y;
	w = 320; h = 8;
}

void SCROLLBAR::display( int top )
{
	WINDOW::display( top );
	if ( ICON_LAYER == 4 )
		world->setlayer( 2 );
	fbox3D( x, y, w, h, TRUE );
	barwidth = ( ( ( w << 16 ) / world->layer->page->w ) * 320 ) >> 16;
	barpos = ( ( ( ( w - barwidth ) << 16 ) / world->layer->page->w ) * world->layer->position ) >> 16;
	V_fbox( x + barpos, y, barwidth, h, 217 );
	V_box( x + barpos, y, barwidth, h, 215 );
	V_box( x + barpos, y, barwidth, 1, 218 );
	V_box( x + barpos, y, 1, h, 218 );
}

void SCROLLBAR::press( int x, int y, int button )
{
int l;

	if ( x > barpos + barwidth )
		scroll( 320 / ( 1 << ( world->getlayer() - 1 ) ) );
	if ( x < barpos )
		scroll( -320 / ( 1 << ( world->getlayer() - 1 ) ) );
}

INFO::INFO()
{
	x = INI_information.x; y = INI_information.y;
	w = 320; h = 32;
	title = "Information";
	background = TRUE;
}

#ifndef __WATCOMC__
/*
	Dummy procedure, to placate GCC
*/
char *ltoa( int i, char *b, int base )
{
	sprintf( b, "%i", i );
	return b;
}
#endif

void INFO::display( int top )
{
char buffer[ 32 ];
int type;

	WINDOW::display( top );
	fbox3D( x, y, w, h, TRUE );
	if ( ICON_EVENT )
	{
		V_print( x + 2, y + 2, "Triggers", SINGLE( 26 ) );
		V_print( x + 13, y + 12, "active: ", SINGLE( 250 ) );
		V_print( x + 70, y + 12, ltoa( world->event->items(), buffer, 10 ), SINGLE( 255 ) );
		h = 24;
		if ( activeevent != NULL )
		{
			V_print( x + 13, y + 22, "event:", SINGLE( 250 ) );
			V_print( x + 70, y + 22, activeevent->getname(), SINGLE( 255 ) );
			h = 32;
			if ( ( type = activeevent->gettype() ) != EVENT::DT_NONE )
			{
				if ( type & EVENT::DT_CHAR )
				{
					V_print( x + 13, y + h, "data:", SINGLE( 250 ) );
					V_print( x + 70, y + h, "<string>", SINGLE( 255 ) );
					h += 12;
					V_print( x + 23, y + h, activeevent->geteventmessage(), SINGLE( 255 ) );
					h += 12;
				}
				if ( type & EVENT::DT_OBJECT )
				{
					V_print( x + 13, y + h, "data:", SINGLE( 250 ) );
					V_print( x + 70, y + h, "<object list>", SINGLE( 255 ) );
					h += 12;
				}
				if ( type & EVENT::DT_LINE || type & EVENT::DT_NUMBER ) 
				{
					V_print( x + 13, y + h, "data:", SINGLE( 250 ) );
					if ( type & EVENT::DT_LINE )
						V_print( x + 70, y + h, "<line>", SINGLE( 255 ) );
					else
						V_print( x + 70, y + h, "<number>", SINGLE( 255 ) );
					h += 12;
					V_print( x + 23, y + h, ltoa( activeevent->geteventx().integer(), buffer, 10 ), SINGLE( 255 ) );
					h += 12;
				}
				if ( type & EVENT::DT_VECTOR || type & EVENT::DT_POINT || 
					type & EVENT::DT_NUMBERS )
				{
					V_print( x + 13, y + h, "data:", SINGLE( 250 ) );
					if ( type & EVENT::DT_VECTOR )
						V_print( x + 70, y + h, "<vector>", SINGLE( 255 ) );
					if ( type & EVENT::DT_POINT )
						V_print( x + 70, y + h, "<point>", SINGLE( 255 ) );
					if ( type & EVENT::DT_NUMBERS )
						V_print( x + 70, y + h, "<numbers>", SINGLE( 255 ) );
					h += 12;
					V_print( x + 23, y + h, ltoa( activeevent->geteventx().integer(), buffer, 10 ), SINGLE( 255 ) );
					h += 12;
					V_print( x + 23, y + h, ltoa( activeevent->geteventy().integer(), buffer, 10 ), SINGLE( 255 ) );
					h += 12;
				}
			}
		}
	} else
		if ( ICON_MARKED )
		{
			h = 34;
			V_print( x + 2, y + 2, "Layer", SINGLE( 26 ) );
			V_print( x + 13, y + 12, "objects:", SINGLE( 250 ) );
			V_print( x + 80, y + 12, ltoa( world->layer->naturalQ->items(), buffer, 10 ), SINGLE( 255 ) );
			V_print( x + 13, y + 22, "marked:", SINGLE( 250 ) );
			V_print( x + 80, y + 22, ltoa( marked->items(), buffer, 10 ), SINGLE( 255 ) );
		}
}

void INFO::press( int x, int y, int button )
{
}

WINDOWLIST::WINDOWLIST()
{
 	bar = close = FALSE;
	x = mousex - 50; y = mousey - ( ( windowlist->items() * 10 + 4 ) >> 1 );
	w = 100; h = windowlist->items() * 10 + 3;
	if ( x < 0 ) x = 0;
	if ( y < 0 ) y = 0;
	if ( x + w > 319 ) x = 319 - w;
	if ( y + h > 199 ) y = 199 - h;
}

void WINDOWLIST::display( int top )
{
WINDOW *m;
int pos;

	WINDOW::display( top );
	fbox3D( x, y, w, h, TRUE );
	fbox3D( x, y, w, h, TRUE );
	pos = 0;
	m = windowlist->reset();
	while ( m != NULL )
	{
		V_print( WINDOW::x + 2, WINDOW::y + 2 + pos, m->title, SINGLE(
			251 + ( windows->exists( m ) << 2 ) ) );
		pos += 10;
		m = windowlist->next();
	}
}

void WINDOWLIST::press( int x, int y, int button )
{
int pos;
WINDOW *m, *m1;

	while ( mousebutton == 4 || mousebutton == ( 1 | 2 ) )
	{
		display_world();
		display_windows();
		y = mousey - WINDOWLIST::y;
		x = mousex - WINDOWLIST::x;
		y /= 10;
		if ( x >= 0 && x < w )
			if ( y >= 0 && y < windowlist->items() )
			{
				V_box( WINDOWLIST::x + 1, WINDOWLIST::y + y * 10, w - 2, 12, 250 );
				V_box( WINDOWLIST::x + 1, WINDOWLIST::y + y * 10, w - 2, 12, 250 );
			}
		V_put( mousex, mousey, mouse, MASK );
		display_to_video();
	}
	if ( x >= 0 && x < w )
		if ( y >= 0 && y < windowlist->items() )
		{
			pos = 0;
			m = windowlist->reset();
			while ( m != NULL )
			{
				if ( pos == y )
					if ( !windows->exists( m ) )
					{
						windows->insert( windows->lowest_priority() - 1, m );
						break;
					} else
					{
						windows->remove( m );
						break;
					}
				pos++;
				m = windowlist->next();
			}
		}
}

HELP::HELP()
{
	bar = FALSE;
	title = "Help";
	x = y = 0;
	w = 320; h = 200;
}

void HELP::display( int top )
{
int l;

	V_blit( V_screen->ptr, V_screen->ptr, 64000, FADE( -4 ) );
	V_print( 2, 2, "Keys", SINGLE( 26 ) );
	V_print( 2, 12, "H", SINGLE( 255 ) );
	V_print( 60, 12, "Cycle through object handle positions", SINGLE( 250 ) );
	V_print( 2, 22, "[]", SINGLE( 255 ) );
	V_print( 60, 22, "Previous/next object", SINGLE( 250 ) );
	V_print( 2, 32, "Up/Down", SINGLE( 255 ) );
	V_print( 60, 32, "Previous/next layer", SINGLE( 250 ) );
}

void HELP::press( int x, int y, int button )
{
	windows->remove( this );
}

SAVE::SAVE()
{
	title = "Save level";
	y = x = 40;
	w = 100; h = 0;
	bar = close = TRUE;
}

void SAVE::display( int top )
{
	WINDOW::display( top );
}

void SAVE::press( int x, int y, int button )
{
}

LOAD::LOAD()
{
	title = "Load level";
	x = y = 20;
	w = 100; h = 0;
	bar = close = TRUE;
}

void LOAD::display( int top )
{
	WINDOW::display( top );
}

void LOAD::press( int x, int y, int button )
{
}

EXIT::EXIT()
{
	title = "Exit";
	x = 95; y = 90;
	w = 130; h = 24;
	bar = close = FALSE;
}

void EXIT::display( int top )
{
	WINDOW::display( top );
	fbox3D( x, y, w, h, TRUE );
	fbox3D( x, y, w, h, TRUE );
	V_print( x + 30, y + 2, "Exit to DOS?", SINGLE( 26 ) );
	V_print( x + 12, y + 12, "Yes", SINGLE( 250 ) );
	V_print( x + 104, y + 12, "No", SINGLE( 255 ) );
}

void EXIT::press( int x, int y, int button )
{
	if ( ::touching( x, y, 104, 12, V_textwidth( "No" ), 10 ) )
		windows->remove( this );
	else
		if ( ::touching( x, y, 12, 12, V_textwidth( "Yes" ), 10 ) )
			quit = TRUE;
}

MARKEDATTRIBSET::MARKEDATTRIBSET()
{
OBJECT *o;

	title = "Marked attributes";
	x = 80; y = 40;
	w = 100; h = 74;
	bar = TRUE; close = FALSE;
	memset( &set, 0, sizeof( set ) );
	memset( &userset, 0, sizeof( userset ) );
	memset( &userreset, 0, sizeof( userreset ) );
	fade = 1000;
	// determine which attribtes are active in any of the objects
	o = marked->reset();
	while ( o != NULL )
	{
		if ( o->attribute.scenery ) set.scenery = TRUE;
		if ( o->attribute.dynamic ) set.dynamic = TRUE;
		if ( o->attribute.solid ) set.solid = TRUE;
		if ( o->attribute.alive ) set.alive = TRUE;
		if ( o->attribute.possessable ) set.possessable = TRUE;
		o = marked->next();
	}
}

void MARKEDATTRIBSET::display( int top )
{
int c;
char buffer[ 32 ];

	WINDOW::display( top );
	fbox3D( x, y, w, h, TRUE );
	MAS_display_tick_box( 2, scenery, "scenery" );
	MAS_display_tick_box( 12, dynamic, "dynamic" );
	MAS_display_tick_box( 22, solid, "solid" );
	MAS_display_tick_box( 32, possessable, "possessable" );
	MAS_display_tick_box( 42, alive, "alive" );
	V_print( x + 2, y + 52, "fade", SINGLE( 250 ) );
	ltoa( fade, buffer, 10 );
	V_print( x + 80, y + 52, fade == 1000 ? "?" : buffer, SINGLE( 255 ) );
	V_print( x + 2, y + 62, "Apply", SINGLE( 255 ) );
	V_print( x + 60, y + 62, "Cancel", SINGLE( 255 ) );
}

void MARKEDATTRIBSET::press( int x, int y, int button )
{
OBJECT *o;
OBJECT::ATTRIBUTE *a;

	if ( ::touching( x, y, 2, 62, V_textwidth( "Apply" ), 10 ) )
	{ // apply attributes to list
		o = marked->reset();
		while ( o != NULL )
		{
			a = &o->attribute;
			if ( userreset.scenery ) a->scenery = FALSE; else
				if ( userset.scenery ) a->scenery = TRUE;
			if ( userreset.dynamic ) a->dynamic = FALSE; else
				if ( userset.dynamic ) a->dynamic = TRUE;
			if ( userreset.possessable ) a->possessable = FALSE; else
				if ( userset.possessable ) a->possessable = TRUE;
			if ( userreset.solid ) a->solid = FALSE; else
				if ( userset.solid ) a->solid = TRUE;
			if ( fade != 1000 ) o->setfade( layershade[ world->getlayer() ] + fade );
			o = marked->next();
		}
		markedsetwindow = NULL;
		windows->remove( this );
		delete this;
		world->layer->reset();
		return;
	}
	if ( ::touching( x, y, 60, 62, V_textwidth( "Cancel" ), 10 ) )
	{
		markedsetwindow = NULL;
		windows->remove( this );
		delete this;
		return;
	}
	if ( ::touching( x, y, 80, 2, 8, 8 ) ) MAS_set_attribute( scenery );
	if ( ::touching( x, y, 80, 12, 8, 8 ) ) MAS_set_attribute( dynamic );
	if ( ::touching( x, y, 80, 22, 8, 8 ) ) MAS_set_attribute( solid );
	if ( ::touching( x, y, 80, 32, 8, 8 ) ) MAS_set_attribute( possessable );
	if ( ::touching( x, y, 80, 42, 8, 8 ) ) MAS_set_attribute( alive );
	if ( ::touching( x, y, 80, 52, 20, 8 ) )
	{ // set fade value for objects
		if ( fade == 1000 ) fade = 0;
		if ( ( button & 1 ) && fade < 14 ) fade++;
		if ( ( button & 2 ) && fade > -14 ) fade--;
	}
}

ATTRIB::ATTRIB()
{
	title = "Attributes";
	x = INI_attribute.x; y = INI_attribute.y;
	w = 128; h = 74;
	bar = close = TRUE;
}

void ATTRIB::display( int top )
{
int c;
char buffer[ 32 ];

	WINDOW::display( top );
	fbox3D( x, y, w, h, TRUE );
	if ( activeobject == NULL )
		V_print( x + 2, y + 2, image->name( currentobject ), SINGLE( 255 ) );
	else
		V_print( x + 2, y + 2, image->name( activeobject->id ), SINGLE( 255 ) );
	ATTRIB_display_tick_box( 12, scenery, "scenery" );
	ATTRIB_display_tick_box( 22, dynamic, "dynamic" );
	ATTRIB_display_tick_box( 32, solid, "solid" );
	ATTRIB_display_tick_box( 42, possessable, "possessable" );
	ATTRIB_display_tick_box( 52, alive, "alive" );
	if ( activeobject )
		ltoa( activeobject->getfade() - layershade[ world->getlayer() ], buffer, 10 );
	else
		ltoa( objectfade, buffer, 10 );
	V_print( x + 2, y + 62, "fade", SINGLE( 250 ) );
	V_print( x + 100, y + 62, buffer, SINGLE( 255 ) );
}

void ATTRIB::press( int x, int y, int button )
{
	if ( ::touching( x, y, 100, 12, 8, 8 ) ) attribute.scenery = !attribute.scenery;
	if ( ::touching( x, y, 100, 22, 8, 8 ) ) attribute.dynamic= !attribute.dynamic;
	if ( ::touching( x, y, 100, 32, 8, 8 ) ) attribute.solid = !attribute.solid;
	if ( ::touching( x, y, 100, 42, 8, 8 ) ) attribute.possessable = !attribute.possessable;
	if ( ::touching( x, y, 100, 52, 8, 8 ) ) attribute.alive = !attribute.alive;
	if ( ::touching( x, y, 100, 62, 28, 8 ) )
	{
		if ( ( button & 1 ) && ( objectfade < 14 - layershade[ world->getlayer() ] ) ) objectfade++;
		if ( ( button & 2 ) && ( objectfade > -14 - layershade[ world->getlayer() ] ) ) objectfade--;
	}
}

EVENTLIST::EVENTLIST()
{
	title = "Trigger list";
	w = 200; h = EVENT::getmaxevents() * 10 + 4;
	x = 60; y = 100 - h / 2;
	bar = close = TRUE;
	finished = FALSE;
	displaymode = -1;
	entrypos = entry = 0;
}

void EVENTLIST::display( int top )
{
int l;

	WINDOW::display( top );
	fbox3D( x, y, w, h, TRUE );
	switch ( displaymode )
	{
		case -1 :	//
			for ( l = 0; l != EVENT::getmaxevents(); l++ )
				V_print( x + 2, y + 2 + l * 10, EVENT::geteventname( l ), SINGLE( 255 ) );
		break;
		case EVENT::DT_CHAR :
			V_print( x + 2, y + 2, "Enter a text string", SINGLE( 250 ) );
			if ( K_pressed() )
			{
				switch ( K_pressed() )
				{
					case kbBACKSPACE :
						if ( entrypos ) entryA[ --entrypos ] = 0;
					break;
					case kbENTER :
						finished = TRUE;
					break;
					case kbESC :
						finished = 2;
					break;
					default :
						if ( ( entryA[ entrypos ] = K_convertascii( K_pressed() ) ) != 0 )
							entrypos++;
				}
				K_clearpressed();
			}
			entryA[ entrypos ] = 0;
			V_print( x + 2, y + 12, entryA, SINGLE( 255 ) );
			fbox3D( x + 2 + V_textwidth( entryA ), y + 12, 8, 8, FALSE );
			fbox3D( x + 2 + V_textwidth( entryA ), y + 12, 8, 8, FALSE );
			fbox3D( x + 2 + V_textwidth( entryA ), y + 12, 8, 8, FALSE );
		break;
		case EVENT::DT_NUMBER :
		break;
		case EVENT::DT_NUMBERS :
		break;
	}
}

void EVENTLIST::press( int x, int y, int _button )
{
int mx, my, addevent = TRUE, eventtype;
char *message;
// Data types
EVENT *t;
int tx, ty, type;
QUEUE <OBJECT> *objq;

	eventtype = ( y - 2 ) / 10;
	while ( mousebutton )
	{
		display_all();
		if ( eventtype != -1 )
			V_box( WINDOW::x + 1, WINDOW::y + 1 + eventtype * 10, w - 2, 10, 255 );
		V_put( mousex, mousey, mouse, MASK );
		display_to_video();
		if ( ::touching( mousex, mousey, WINDOW::x + 2, WINDOW::y + 2, w - 4, h - 4 ) )
			eventtype = ( mousey - WINDOW::y - 2 ) / 10;
		else
			eventtype = -1;
	}
	if ( eventtype != -1 )
	{ // add an event to the world
		mx = eventx + eventw / 2;
		my = eventy + eventh / 2;
		type = EVENT::geteventtype( eventtype );
		if ( ICON_TRIGGERREGION )
			world->event->insert( world->event->highest_priority() + 1, ( t = 
				new EVENT( eventx, eventy, eventw, eventh, eventtype ) ) );
		if ( ICON_TRIGGERPROXIMITY )
			world->event->insert( world->event->highest_priority() + 1, ( t =
				new EVENT( eventx, eventy, eventw, eventtype ) ) );
		if ( type & EVENT::DT_CHAR )
		{
			h = 24;
			y = 100 - h / 2;
			displaymode = type;
			while ( !finished )
			{
				display_all();
				V_put( mousex, mousey, mouse, MASK );
				display_to_video();
			}
			if ( finished == TRUE ) 
				t->set( entryA );
			else 
				world->event->remove( t );
		}
	}
	windows->remove( this );
	displaymode = -1;
	entrypos = 0;
	finished = FALSE;
	h = EVENT::getmaxevents() * 10 + 4;
	y = 100 - h / 2;
}

OBJECTLIST::OBJECTLIST()
{
	title = "Object list";
	x = y = 0;
	w = 320; h = 200;
	bar = close = FALSE;
}

void OBJECTLIST::display( int top )
{
int l;
int x, y;
char match[ 32 ];

	onindex = -1;
	V_blit( V_screen->ptr, V_screen->ptr, 64000, FADE( -4 ) );
	strcpy( match, "dummy" );
	for ( x = 2, y = 2, l = 0; l != image->images(); l++ )
	{
		if ( strncmp( strchr( image->name( l ), '_' ) + 1, match, strlen( match ) ) &&
			valid_object( l ) )
		{
			if ( ::touching( mousex, mousey, x, y, 80, 10 ) )
			{
				onindex = l;
				onx = x; ony = y;
			}
			strcpy( match, strchr( image->name( l ), '_' ) + 1 );
			if ( strchr( match, '0' ) != NULL ) *strchr( match, '0' ) = 0;
			if ( !strncmp( image->name( l ), "ACTIVE", 6 ) )
				V_print( x, y, match, SINGLE( ( l == currentobject ? 46 : 41 ) ) );
			else
				V_print( x, y, match, SINGLE( ( l == currentobject ? 255 : 250 ) ) );
			y += 10;
			if ( y >= 190 )
			{
				x += 80;
				y = 2;
			}
		}
	}
}

void OBJECTLIST::press( int x, int y, int button )
{
	while ( mousebutton )
	{
		display_all();
		// we are on top of a word
		if ( onindex != -1 ) V_box( onx - 2, ony - 2, 81, 11, 255 );
		V_put( mousex, mousey, mouse, MASK );
		display_to_video();
	}
	if ( onindex != -1 ) currentobject = onindex;
	windows->remove( this );
}
