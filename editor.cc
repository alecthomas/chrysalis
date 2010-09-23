/*******************************************************************************
  The editor for Chrysalis

  Author: Alec Thomas
  Updates: Dec 21, '94
  Comments:
*******************************************************************************/

/*
	do this:
		- finish off the triggers!!!!
*/

#include <signal.h>
#include <unistd.h>
#include "include.h"
#include "editwin.h"
#include "editor.h"
#include "event.h"

#define sin(a, b) sin(fixed(a))
#define __unfixed(a) (a.number >> 16)

int WINDOW::border = FALSE;
// Data
OBJECT::ATTRIBUTE attribute;
int objectfade = 0;
VPAGESTRUCT *editorpage;
OBJECT *currento, *activeobject = NULL;
EVENT *activeevent = NULL;
char *mouse;
int mousex, mousey, mousebutton, touched = FALSE, onwindow = FALSE, currentobject = 0,
	button[ 10 ][ 2 ], handle_position = 0, quit = FALSE,
	eventx, eventy, eventw, eventh, mouseactive = FALSE,
	display_boxes = TRUE;
IMAGE *eimage;
WINDOW *loadwindow, *savewindow, *eventwindow, *markedsetwindow;
QUEUE <OBJECT> *marked, *undo;
QUEUE <WINDOW> *windows, *windowlist;
INIWINDOW
	INI_icon = { 140, 0, TRUE },
	INI_information = { 0, 16, FALSE },
	INI_objectviewer = { 0, 170, FALSE },
	INI_scrollbar = { 0, 0, TRUE },
	INI_attribute = { 80, 40, FALSE };

int parse_INI_window_settings( INIWINDOW &windata, char *setting )
/* ================================================================
	Convert a string in the form "x,y,[true/false]" to a valid window
	starting state.
*/
{
char buffer[ 64 ];
char *xp, *yp, *hiddenp;

	xp = setting;
	if ( ( yp = strchr( setting, ',' ) ) == NULL ) return FALSE;
	yp++; while ( *yp == ' ' ) yp++;
	if ( ( hiddenp = strchr( yp, ',' ) ) == NULL ) return FALSE;
	hiddenp++; while ( *hiddenp == ' ' ) hiddenp++;
	while ( hiddenp[ strlen( hiddenp ) - 1 ] == ' ' )
		hiddenp[ strlen( hiddenp ) - 1 ] = 0;
	windata.x = strtol( xp, NULL, 10 );
	windata.y = strtol( yp, NULL, 10 );
	if ( !strcmp( hiddenp, "true" ) )
		windata.activeatstart = TRUE;
	else
		windata.activeatstart = FALSE;
	return TRUE;
}

int parse_INI_command( char *command, char *setting )
/* ===================================================
	Parse an INI command
*/
{
	if ( !strcmp( command, "border" ) )
	{ // turn window borders on/off
		if ( !strcmp( setting, "true" ) )
			WINDOW::border = TRUE;
		else
			WINDOW::border = FALSE;
		return TRUE;
	}
	if ( !strcmp( command, "icons" ) )
	{ // icon window
		CHECK( !parse_INI_window_settings( INI_icon, setting ), "invalid \"icons\" window definition" );
		return TRUE;
	}
	if ( !strcmp( command, "information" ) )
	{ // information window
		CHECK( !parse_INI_window_settings( INI_information, setting ), "invalid \"information\" window definition" );
		return TRUE;
	}
	if ( !strcmp( command, "viewer" ) )
	{ // object viewer window
		CHECK( !parse_INI_window_settings( INI_objectviewer, setting ), "invalid \"viewer\" window definition" );
		return TRUE;
	}
	if ( !strcmp( command, "scroll_bar" ) )
	{ // scroll bar position
		CHECK( !parse_INI_window_settings( INI_scrollbar, setting ), "invalid \"scroll_bar\" window definition" );
		return TRUE;
	}
	if ( !strcmp( command, "attribute" ) )
	{ // scroll bar position
		CHECK( !parse_INI_window_settings( INI_attribute, setting ), "invalid \"attribute\" window definition" );
		return TRUE;
	}
	return FALSE;
}

int executable()
/* ==============
	Return 1 because in editor
*/
{
	return 1;
}

void reset_all()
/* ==============
	Reset all active temporary lists
*/
{
	delete marked;
	marked = new QUEUE <OBJECT>();
	delete undo;
	undo = new QUEUE <OBJECT>();
	activeobject = NULL;
	activeevent = NULL;
	objectfade = 0;
}

void title()
/* ==========
	Display banner of editor
*/
{
    cout << "ChRySaLiS EdItOr " VERSION " ";
}

void untitle()
/* ============
	Display exit banner
*/
{
	if ( flags.debug ) cout << "exit ok" << endl;
}

void command_help()
/* =================
	Command line help
*/
{ //           -?                   ????
	cout << "  -new [name] [#]      create a new level file" << endl;
}

void clearline();

void parse( int &l )
/* ========================
	Parse command-line arguments
*/
{
LIB *lb;
char sbuf[ 128 ], lstr[ 64 ];

	if ( !strcmp( argument[ l ] + 1, "new" ) )
	{ // create a new level data file
		CHECK( ++l >= arguments, "argument -new incomplete" );
		cout << "creating level library " << argument[ l ];
		lb = new LIB();
		CHECK( !lb->valid, "could not create new level file" );
		CHECK( ++l >= arguments, "argument -new incomplete" );
		strcpy( lstr, "LEVEL_" );
		for ( l = 0; l != 4 - strlen( argument[ l ] ); l++ )
			strcat( lstr, "0" );
		strcat( lstr, argument[ l ] );
		CHECK( lb->seek( lstr ) != -1, "level already exists in library" );
		lb->create( argument[ l - 1 ] );
		lb->add( lstr, lstr, 0 );
		delete lb;
		cout << " ";
		CHECK( !lib->mergelib( argument[ l - 1 ] ), "invalid library ???" );
		cout << " ok";
		clearline();
		WORLD::firstlevel = strtol( argument[ l ], NULL, 10 );
		return;
	}
	strcpy( sbuf, "invalid command line argument: " );
	strcat( sbuf, argument[ l ] );
	CHECK( TRUE, sbuf );
}

void scroll( int direction )
/* ==========================
	Scroll entire world in a given direction
*/
{
int l, old;
int curlayer = world->getlayer();

	if ( ICON_LAYER == 4 )
		direction = direction < 0 ? -1 : +1;
	world->setlayer( 2 );
	old = world->layer->position;
	for ( l = 1; l != 4 ; l++ )
	{
		world->setlayer( l );
		world->layer->scroll( direction );
	}
	world->setlayer( 2 );
	if ( direction > 0 && world->layer->position > world->layer->page->w - 320 )
		// maxed out
		for ( l = 1; l != 4; l++ )
		{
			world->setlayer( l );
			world->layer->position = world->layer->worldposition = world->layer->page->w - 320;
		}
	world->setlayer( 2 );
	if ( direction < 0 && old < world->layer->position )
		// minned out
		for ( l = 1; l != 4; l++ )
		{
			world->setlayer( l );
			world->layer->position = world->layer->worldposition = 0;
		}
	world->setlayer( curlayer );
}

OBJECT *insert_object( int currentobject, int mousex, int mousey )
/* ================================================================
	Insert an object into the world
*/
{
OBJECT *o;

	if ( ICON_BACK == 0 )
	{
		if ( !strncmp( image->name( currentobject ), "CREATURE", 7 ) )
		{
			o = world->insertobject( mousex + world->layer->worldposition,
				mousey, world->layer->naturalQ->highest_priority() + 1,
				currentobject );
			undo->insert( undo->highest_priority() + 1, o );
		} else
		{
			o = world->insertobject( mousex + world->layer->position,
				mousey, world->layer->naturalQ->highest_priority() + 1,
				currentobject );
			undo->insert( undo->highest_priority() + 1, o );
		}
	} else
	{
		if ( !strncmp( image->name( currentobject ), "CREATURE", 7 ) )
			o = world->insertobject( mousex + world->layer->worldposition,
				mousey, world->layer->naturalQ->lowest_priority() - 1,
				currentobject );
		else
			o = world->insertobject( mousex + world->layer->position,
				mousey, world->layer->naturalQ->lowest_priority() - 1,
				currentobject );
		undo->insert( undo->highest_priority() + 1, o );
	}
	// Set default attributes user has defined
	if ( attribute.dynamic ) o->attribute.dynamic = TRUE;
	if ( attribute.solid ) o->attribute.solid = TRUE;
	o->setfade( layershade[ world->getlayer() ] + objectfade );
	return o;
}

void load_data()
/* ===============
    Load all editor data
*/
{
int l, l1;

    eimage = new IMAGE();
    eimage->set( EDITOR_LOAD, "EDITOR_LOAD" );
    eimage->set( EDITOR_CANCEL, "EDITOR_CANCEL" );
    eimage->set( EDITOR_FLUSH, "EDITOR_FLUSH" );
    eimage->set( EDITOR_HELP, "EDITOR_HELP" );
    eimage->set( EDITOR_TYPE, "EDITOR_TYPE" );
    eimage->set( EDITOR_PLANE, "EDITOR_PLANE" );
    eimage->set( EDITOR_SAVE, "EDITOR_SAVE" );
    eimage->set( EDITOR_SCENBACK, "EDITOR_SCENBACK" );
    eimage->set( EDITOR_SCENFORW, "EDITOR_SCENFORW" );
    eimage->set( EDITOR_SCENLAYE, "EDITOR_SCENLAYE" );
    eimage->set( EDITOR_SOLIDSCN, "EDITOR_SOLIDSCN" );
	eimage->set( EDITOR_MODE, "EDITOR_MODE" );
	eimage->set( EDITOR_ATTRIBMARKED, "EDITOR_MARKATTRIB" );
	eimage->set( EDITOR_EVENT, "EDITOR_EVENT" );
	// create the world
	world = new WORLD();
	// reset all layers
	for ( l = 0; l != 4; l++ )
	{
		world->setlayer( l );
		world->layer->reset();
	}
}

void display_all()
/* ================
	Display world and windows
*/
{
	display_world();
	display_windows();
}

void display_till_no_mouse()
/* ==========================
	Keep displaying until mouse is released
*/
{
	while ( mousebutton )
	{
		display_all();
		V_put( mousex, mousey, mouse, MASK );
		display_to_video();
	}
}

void shadebox( int x, int y, int w, int h, int shade )
/* ====================================================
	Draw a shaded box
*/
{
int l;
char *i;

	if ( !clip( i, x, y, w, h, V_screen->w, V_screen->h ) ) return;
    for ( l = y; l != y + h; l++ )
        V_blit( V_screen->ytable[ l ] + x, V_screen->ytable[ l ] + x, w, FADE( shade ) );
}

void fbox3D( int x, int y, int w, int h, int indent )
/* ===================================================
	Draw a 3D shaded box
*/
{
int bindent, mindent;

    if ( indent )
    {
        indent = -2;
        bindent = -4;
		mindent = -3;
    } else
    {
        indent = 2;
        bindent = 4;
		mindent = 3;
    }
	shadebox( x, y, w, h, mindent );
    shadebox( x + w - 1, y, 1, h, indent );
    shadebox( x, y + h - 1, w, 1, indent );
    shadebox( x, y, w, 1, bindent );
    shadebox( x, y, 1, h, bindent );
}

void display_world()
/* ==================
    Display the world at the current position
*/
{
int l;
EVENT *t;
OBJECT *o;
char strbuf[ 32 ];

	if ( ICON_LAYER == 4 )
		// display all layers
		for ( l = 0; l != 4; l++ )
		{
			world->setlayer( l );
			world->layer->displayall();
		}
	else
	{
		memset( V_screen->ptr, 0, 64000 );
		world->setlayer( 3 - ICON_LAYER );
		world->layer->displayall();
	}
	M_getposition( mousex, mousey );
	M_pressed( mousebutton );
	// Display all tricky events
	if ( display_boxes )
		if ( ICON_LAYER != 4 )
		{
			if ( ICON_EVENT )
			{ // Cycle events
				activeevent = NULL;
				t = world->event->reset();
				while ( t != NULL )
				{
					t->display( world->layer->worldposition, 40 + __unfixed( sin( frame << 2, 4 ) ) );
					if ( flags.debug )
						V_print( t->gettriggerx() - world->layer->worldposition - V_textwidth( t->getname() ) / 2, t->gettriggery() - 4, t->getname(), SINGLE( 255 ) );
					if ( t->touching( mousex + world->layer->worldposition, mousey ) )
						activeevent = t;
					t = world->event->next();
				}
			} else
				if ( ICON_MARKED )
				{ // Cycle marked objects
					o = marked->reset();
					while ( o != NULL )
					{
						if ( o->attribute.dynamic )
							V_box( o->getx().integer() - world->layer->worldposition, o->gety().integer(), o->getw(),
								o->geth(), 24 + __unfixed( sin( frame << 2, 4 ) ) );
						else
						{
							V_box( o->getx().integer() - world->layer->position, o->gety().integer(), o->getw(),
								o->geth(), 24 + __unfixed( sin( frame << 2, 4 ) ) );
							V_box( o->getx().integer() - world->layer->position + world->layer->page->w, o->gety().integer(), o->getw(),
								o->geth(), 24 + __unfixed( sin( frame << 2, 4 ) ) );
						}
						o = marked->next();
					}
				}
		}
	if ( K_pressed() )
	{
		if ( K_index( kbpLEFT ) )
		{
			scroll( -320 / ( 1 << ( world->getlayer() - 1 ) ) );
			K_clearpressed();
		}
		if ( K_index( kbpRIGHT ) )
		{
			scroll( 320 / ( 1 << ( world->getlayer() - 1 ) ) );
			K_clearpressed();
		}
	}
	// scroll left
	if ( mousex <= 0 )
	{
		scroll( -4 );
		world->setlayer( 3 - ICON_LAYER );
	}
	// scroll right
	if ( mousex >= 319 )
	{
		scroll( +4 );
		world->setlayer( 3 - ICON_LAYER );
	}
	// reset stuff ;)
	if ( onwindow )
	{
		activeevent = NULL;
		activeobject = NULL;
	}
}

void display_to_video()
/* =====================
	copy virtual screen to video
*/
{
	V_showpage();
//    memcpy( V_VGAscreen->ptr, editorpage->ptr, 320 * 200 );
	frame++;
}

void display_windows()
/* ==================
	Display window queue
*/
{
WINDOW *s;
int px, py;

	s = windows->reset_end();
	while ( s != NULL )
	{
		s->display( windows->at_head() );
		s = windows->prev();
	}
	if ( touched || mouseactive ) return;
	onwindow = FALSE;
	s = windows->reset();
	while ( s != NULL )
	{
		if ( !touched )
		{
			if ( onwindow == FALSE )
				if ( s->touching( mousex, mousey ) )
					onwindow = TRUE;
			if ( mousebutton )
				if ( ( touched = s->touching( mousex, mousey ) ) == 2 )
				{ // touched window bar
					px = s->x - mousex; py = s->y - mousey;
					// move to top of queue
					windows->remove();
					windows->insert( windows->lowest_priority() - 1, s );
					while ( mousebutton )
					{
						display_world();
						s->x = mousex + px; s->y = mousey + py;
						display_windows();
						V_put( mousex, mousey, mouse, MASK );
						display_to_video();
					}
					touched = 0;
					return;
				} else
					if ( touched == 3 )
					{ // touched close button
						windows->remove();
						display_till_no_mouse();
						touched = 0;
						return;
					} else
						if ( touched == 1 )
						{ // touched main window part
							windows->remove();
							windows->insert( windows->lowest_priority() - 1, s );
							s->press( mousex - s->x, mousey - s->y, mousebutton );
							display_till_no_mouse();
							touched = 0;
							return;
						}
		}
		s = windows->next();
	}
}

int image_x( int x )
/* ==================
	Return x position of image, determined by the current handle position
*/
{
	switch( handle_position )
	{
		case 0 :	// middle
			return x - ( image->getw( currentobject ) >> 1 );
		case 1 :	// top-left
		case 3 :	// bottom-left
			return x;
	}
	return x - image->getw( currentobject );
}

int image_y( int y )
/* ==================
	Return the current image y position, determined from current handle
*/
{
	switch( handle_position )
	{
		case 0 :	// middle
			return y - ( image->geth( currentobject ) >> 1 );
		case 1 :	// top-left
		case 2 :	// top-right
			return y;
	}
	return y - image->geth( currentobject );
}

void display_image( int x, int y )
/* ================================
	Display the "currentobject" 
*/
{
	image->put( image_x( x ), image_y( y ), currentobject, FADE( layershade[ world->getlayer() ] + objectfade ) );
}

int valid_object( int l )
/* =======================
	Return TRUE if the image indexed by "l" is valid, given the current 
	display type (ie. creatures or scenery)
*/
{
	if ( ICON_TYPE == 0 )
		return ( strncmp( image->name( l ), "CREATURE", 8 ) != 0 );
	return ( strncmp( image->name( l ), "CREATURE", 8 ) == 0 );
}

int fix_object( int dir )
/* =======================
	Move backward/forward through the image list and skip over images that
	aren't valid for the current mode.
*/
{
int a = FALSE;

	if ( currentobject < 0 ) currentobject = image->images() - 1;
	if ( currentobject >= image->images() ) currentobject = 0;
	while ( !valid_object( currentobject ) )
	{
		currentobject += dir;
		if ( currentobject < 0 ) currentobject = image->images() - 1;
		if ( currentobject >= image->images() ) currentobject = 0;
		a = TRUE;
	}
	return a;
}

int go()
/* ==========
    The main editor function, handles everything
*/
{
int l, l1, l2, l3;
OBJECT *o;
WINDOW *m;
EVENT *t;
char match[ 32 ];

	signal( SIGINT, SIG_IGN );
	attribute.solid = TRUE;
    load_data();
	marked = new QUEUE <OBJECT>();
	undo = new QUEUE <OBJECT>();
	windows = new QUEUE <WINDOW>();
	windowlist = new QUEUE <WINDOW>();
	if ( INI_icon.activeatstart )
	{
		windows->insert( 0, ( m = new ICONS() ) );
		windowlist->insert( 0, m );
	} else
		windowlist->insert( 0, new ICONS() );
	if ( INI_attribute.activeatstart )
	{
		windows->insert( 0, ( m = new ATTRIB() ) );
		windowlist->insert( 0, m );
	} else
		windowlist->insert( 0, new ATTRIB() );
	if ( INI_scrollbar.activeatstart )
	{
		windows->insert( 0, ( m = new SCROLLBAR() ) );
		windowlist->insert( 0, m );
	} else
		windowlist->insert( 0, new SCROLLBAR() );
	if ( INI_objectviewer.activeatstart )
	{
		windows->insert( 0, ( m = new OBJECTVIEWER() ) );
		windowlist->insert( 0, m );
	} else
		windowlist->insert( 0, new OBJECTVIEWER() );
	if ( INI_information.activeatstart )
	{
		windows->insert( 0, ( m = new INFO() ) );
		windowlist->insert( 0, m );
	} else
		windowlist->insert( 0, new INFO() );
	windowlist->insert( 0, new OBJECTLIST() );
	windowlist->insert( 0, new HELP() );
	windowlist->insert( 0, new EXIT() );
	// Menus that don't show up anywhere
	loadwindow = new LOAD();
	savewindow = new SAVE();
	eventwindow = new EVENTLIST();
	// Initalise stuff ;)
    CHECK( M_init( 320, 200 ) == FALSE, "could not initialise mouse driver" );
    V_setpage( ( editorpage = V_createpage( 320, 200 ) ) );
    CHECK( ( mouse = lib->retrieve( "MISC_MOUSE" ) ) == NULL, "could not retrieve MISC_MOUSE" );
	if ( ICON_TYPE == 0 )
	{ // placing scenery
		while ( !strncmp( image->name( currentobject ), "CREATURE", 7 ) )
		{
			currentobject++;
			if ( currentobject >= image->images() ) currentobject = 0;
		}
	} else
	{ // placing dudes ;)
		while ( strncmp( image->name( currentobject ), "CREATURE", 7 ) )
		{
			currentobject++;
			if ( currentobject >= image->images() ) currentobject = 0;
		}
	}
	while ( !quit )
    {
        display_world();
		if ( K_pressed() )
		{
			if ( K_index( kbALT ) )
			{
				if ( K_pressed() == kbX ) break;
			} else
				if ( K_index( kbCTRL ) )
				{ // jump to a specific image
					l1 = K_convertascii( K_pressed() );
					if ( l1 >= 97 ) l1 = l1 - 32;
					if ( l1 < 65 || l1 > 90 )
					{
						K_clearpressed();
						continue;
					}
					match[ ( l2 = 0 ) ] = 0;
					for ( ;; )
					{
						while ( !K_pressed() )
						{
							display_world();
							display_windows();
							V_print( 2, 190, "matching:", SINGLE( 250 ) );
							V_print( 2 + V_textwidth( "matching:" ), 190, match, SINGLE( 255 ) );
							V_put( mousex, mousey, mouse, MASK );
							display_to_video();
							if ( !K_index( kbCTRL ) ) break;
						}
						if ( !K_index( kbCTRL ) || K_pressed() == kbENTER ) break;
						if ( K_pressed() == kbBACKSPACE && l2 ) match[ --l2 ] = 0;
						l1 = K_convertascii( K_pressed() );
						if ( l1 >= 97 ) l1 = l1 - 32;
						// search
						if ( l1 != 0 )
						{
							match[ l2 ] = l1; l2++; match[ l2 ] = 0;
							for ( l = 0; l != image->images(); l++ )
								if ( !strncmp( strchr( image->name( l ), '_' ) + 1, match, l2 ) )
								{
									currentobject = l;
									if ( !valid_object( currentobject ) ) continue;
									break;
								}
						}
						K_clearpressed();
					}
					K_clearpressed();
					continue;
				} else
					if ( K_index( kbLSHIFT ) || K_index( kbRSHIFT ) )
					{
						if ( K_pressed() == kbTAB )
						{
							ICON_MODE = ( ICON_MODE - 1 ) % 3;
							reset_all();
						}
					} else
					{
						if ( K_pressed() == kbpDEL )
						// delete current object
							if ( ICON_MARKED && activeobject )
							{
								world->layer->naturalQ->remove( activeobject );
								activeobject = NULL;
								world->layer->reset();
							}
						if ( K_pressed() == kbpINS )
						// assume current object's type
							if ( ICON_MARKED && activeobject )
							{
								currentobject = activeobject->id;
								fix_object( -1 );
							}
						if ( K_pressed() == kb1 )
						{
							ATTRIB_set_tick_box( scenery );
							if ( activeobject ) world->layer->reset();
						}
						if ( K_pressed() == kb2 )
						{
							ATTRIB_set_tick_box( dynamic);
							if ( activeobject ) world->layer->reset();
						}
						if ( K_pressed() == kb3 )
						{
							ATTRIB_set_tick_box( solid );
							if ( activeobject ) world->layer->reset();
						}
						if ( K_pressed() == kb4 )
						{
							ATTRIB_set_tick_box( possessable );
							if ( activeobject ) world->layer->reset();
						}
						if ( K_pressed() == kb5 )
						{
							ATTRIB_set_tick_box( alive );
							if ( activeobject ) world->layer->reset();
						}
						if ( K_pressed() == kbpPLUS || K_pressed() == kbEQUALS )
						{
							if ( activeobject )
							{
								l = activeobject->getfade();
								if ( l < 14 ) l++;
								activeobject->setfade( l );
								world->layer->reset();
							} else
								if ( objectfade < 14 - layershade[ world->getlayer() ] ) objectfade++;
						}
						if ( K_pressed() == kbpMINUS || K_pressed() == kbMINUS )
						{
							if ( activeobject )
							{
								l = activeobject->getfade();
								if ( l > -14 ) l--;
								activeobject->setfade( l );
								world->layer->reset();
							} else
								if ( objectfade > -14 - layershade[ world->getlayer() ] ) objectfade--;
						}
						if ( K_pressed() == kbH )
							handle_position = ( handle_position + 1 ) % 5;
						if ( K_pressed() == kbTAB )
						{
							ICON_MODE = ( ICON_MODE + 1 ) % 3;
							reset_all();
						}
						if ( K_pressed() == kbB )
							ICON_BACK = ( ICON_BACK + 1 ) & 1;
						if ( K_pressed() == kbpUP )
						{
							ICON_LAYER++;
							if ( ICON_LAYER > 3 ) ICON_LAYER = 0;
							reset_all();
						}
						if ( K_pressed() == kbpDOWN )
						{
							ICON_LAYER--;
							if ( ICON_LAYER < 0 ) ICON_LAYER = 3;
							reset_all();
						}
						if ( K_pressed() == kbSQUAREBRACKETOPEN )
						{
							currentobject--;
							fix_object( -1 );
						}
						if ( K_pressed() == kbSQUAREBRACKETCLOSE )
						{
							currentobject++;
							fix_object( +1 );
						}
						if ( K_pressed() == kbBACKSPACE || K_pressed() == kbU )
						{
							o = undo->reset_end();
							if ( o != NULL )
							{
								world->layer->naturalQ->remove( o );
								undo->remove();
								world->layer->reset();
								K_clearpressed();
								continue;
							}
						}
					}
			K_clearpressed();
		}
		if ( mousebutton == 4 || mousebutton == ( 1 | 2 ) )
		{ // display activity window
			m = new WINDOWLIST();
			windows->insert( windows->lowest_priority() - 1, m );
			display_windows();
			windows->remove( m );
			delete m;
			continue;
		}
		if ( !onwindow )
			if ( ICON_EVENT )
			{ // place an event
				if ( world->getlayer() != 2 ) reset_all();
				ICON_LAYER = 1;
				world->setlayer( 3 - ICON_LAYER );
				if ( mousebutton == 1 && !windows->exists( eventwindow ) )
				{ // add a new event region
					if ( ICON_TRIGGERREGION )
					{ // region trigger definition
						eventx = mousex + world->layer->worldposition;
						eventy = mousey;
						mouseactive = TRUE;
						while( mousebutton == 1 )
						{
							display_world();
							V_box( eventx - world->layer->worldposition, eventy,
								mousex + world->layer->worldposition - eventx,
								mousey - eventy, 40 + __unfixed( sin( frame << 2, 4 ) ) );
							display_windows();
							V_put( mousex, mousey, mouse, MASK );
							display_to_video();
						}
						eventw = mousex + world->layer->worldposition - eventx;
						if ( eventw < 0 )
						{
							eventx += eventw;
							eventw = -eventw;
						}
						eventh = mousey - eventy;
						if ( eventh < 0 )
						{
							eventy += eventh;
							eventh = -eventh;
						}
						mouseactive = FALSE;
						windows->insert( windows->lowest_priority() - 1, eventwindow );
					} else
					{ // proximity region definitions
						eventx = mousex + world->layer->worldposition;
						eventy = mousey;
						mouseactive = TRUE;
						while( mousebutton == 1 )
						{
							display_world();
							eventw = eventx - ( mousex + world->layer->worldposition );
							eventh = eventy - mousey;
							eventw *= eventw; eventh *= eventh;
							eventw = sqrt( eventw + eventh );
							V_putpixel( eventx - world->layer->worldposition, eventy, 40 + __unfixed( sin( frame << 2, 4 ) ) );
							V_circle( eventx - world->layer->worldposition, eventy,
								eventw, 40 + __unfixed( sin( frame << 2, 4 ) ) );
							display_windows();
							V_put( mousex, mousey, mouse, MASK );
							display_to_video();
						}
						mouseactive = FALSE;
						windows->insert( windows->lowest_priority() - 1, eventwindow );
					}
				} else
					if ( mousebutton == 2 )
						if ( activeevent != NULL )
						{ // remove a event
							world->event->remove( activeevent );
							delete activeevent;
							activeevent = NULL;
						}
			} else
				if ( ICON_LAYER != 4 )
					if ( ICON_MARKED )
					{ // Search for objects under cursor
						activeobject = NULL;
						o = world->layer->naturalQ->reset();
						while ( o != NULL )
						{
							if ( o->attribute.dynamic )
								if ( o->touching( mousex + world->layer->worldposition, mousey ) )
									activeobject = o;
							if ( !o->attribute.dynamic )
							{
								if ( o->touching( mousex + world->layer->position, mousey ) )
									activeobject = o;
								if ( world->layer->position > world->layer->page->w - 320 )
									if ( o->touching( mousex - ( world->layer->page->w - world->layer->position ), mousey ) )
										activeobject = o;
							}
							o = world->layer->naturalQ->next();
						}
						if ( activeobject != NULL )
						{ // mouse is on top of an object
							if ( activeobject->attribute.dynamic )
								V_box( activeobject->getx().integer() - world->layer->worldposition,
									activeobject->gety().integer(), activeobject->getw(), activeobject->geth(),
									248 + __unfixed( sin( frame << 2, 4 ) ) );
							else
							{
								V_box( activeobject->getx().integer() - world->layer->position,
									activeobject->gety().integer(), activeobject->getw(), activeobject->geth(),
									248 + __unfixed( sin( frame << 2, 4 ) ) );
								V_box( activeobject->getx().integer() - world->layer->position + world->layer->page->w,
									activeobject->gety().integer(), activeobject->getw(), activeobject->geth(),
									248 + __unfixed( sin( frame << 2, 4 ) ) );
							}
							if ( mousebutton == 1 )
							{ // add a new mark or remove an existing mark
								if ( marked->exists( activeobject ) )
									marked->remove( activeobject );
								else
									marked->insert( 0, activeobject );
								display_till_no_mouse();
								continue;
							}
							if ( mousebutton == 2 && marked->exists( activeobject ) )
							{ // move/copy all marked objects
								mouseactive = TRUE;
								l = mousex + world->layer->worldposition; l1 = mousey;
								display_boxes = FALSE;
								while ( mousebutton == 2 )
								{
									display_world();
									o = marked->reset();
									while ( o != NULL )
									{
										if ( o->attribute.dynamic )
											V_box( ( mousex + world->layer->worldposition - l ) + o->getx().integer() - world->layer->worldposition, ( mousey - l1 ) + o->gety().integer(), o->getw(),
												o->geth(), 24 + __unfixed( sin( frame << 2, 4 ) ) );
										else
										{
											V_box( ( mousex + world->layer->worldposition - l ) + o->getx().integer() - world->layer->position, ( mousey - l1 ) + o->gety().integer(), o->getw(),
												o->geth(), 24 + __unfixed( sin( frame << 2, 4 ) ) );
											V_box( ( mousex + world->layer->worldposition - l ) + o->getx().integer() - world->layer->position + world->layer->page->w, ( mousey - l1 ) + o->gety().integer(), o->getw(),
												o->geth(), 24 + __unfixed( sin( frame << 2, 4 ) ) );
										}
										o = marked->next();
									}
									display_windows();
									V_put( mousex, mousey, mouse, MASK );
									display_to_video();
								}
								if ( K_index( kbALT ) )
								{ // copy images
									o = marked->reset();
									while ( o != NULL )
									{
										if ( o->attribute.dynamic )
											insert_object( o->id, o->getx().integer() - world->layer->worldposition + ( mousex + world->layer->worldposition - l ), o->gety().integer() + ( mousey - l1 ) );
										else
											insert_object( o->id, o->getx().integer() - world->layer->worldposition + ( mousex + world->layer->worldposition - l ), o->gety().integer() + ( mousey - l1 ) );
										o = marked->next();
									}
								} else
								{ // move images
									o = marked->reset();
									while ( o != NULL )
									{
										o->setposition( o->getx().integer() + ( mousex + world->layer->worldposition - l ), o->gety().integer() + ( mousey - l1 ) );
										o = marked->next();
									}
								}
								world->layer->reset();
								display_boxes = TRUE;
								mouseactive = FALSE;
								continue;
							}
						} else
						{ // no active object
							if ( mousebutton == 1 )
							{ // bounding box marking scheme
								mouseactive = TRUE;
								l = mousex + world->layer->worldposition; l1 = mousey;
								while ( mousebutton == 1 )
								{
									display_world();
									l2 = mousex + world->layer->worldposition;
									l3 = mousey;
									V_box( l - world->layer->worldposition, l1, l2 - l,
										l3 - l1, 24 + __unfixed( sin( frame << 2, 4 ) ) );
									display_windows();
									V_put( mousex, mousey, mouse, MASK );
									display_to_video();
								}
								if ( l > l2 ) xchg( l, l2 );
								if ( l1 > l3 ) xchg( l1, l3 );
								o = world->layer->naturalQ->reset();
								while ( o != NULL )
								{
									if ( o->attribute.dynamic )
									{
										if ( touching( o->getx().integer(), o->gety().integer(), o->getw(),
											o->geth(), l, l1, l2 - l, l3 - l1 ) )
											if ( !marked->exists( o ) )
												marked->insert( 0, o );
									} else
									{
										if ( touching( o->getx().integer(), o->gety().integer(), o->getw(),
											o->geth(), l % world->layer->page->w, l1, l2 - l, l3 - l1 ) )
											if ( !marked->exists( o ) )
												marked->insert( 0, o );
									}
									o = world->layer->naturalQ->next();
								}
								mouseactive = FALSE;
								continue;
							}
							if ( mousebutton == 2 )
							{ // unmark all objects
								reset_all();
								delete marked;
								marked = new QUEUE <OBJECT>();
								display_till_no_mouse();
								continue;
							}
						}
					} else
						if ( ICON_PLACE )
						{ // Display current image
							display_image( mousex, mousey );
							if ( mousebutton == 1 )
							{
								insert_object( currentobject, image_x( mousex ), image_y( mousey ) );
								world->layer->reset();
								display_till_no_mouse();
								continue;
							}
						}
		display_windows();
		V_put( mousex, mousey, mouse, MASK );
		display_to_video();
		if ( flags.retrace ) G_waitretrace();
	}
	return TRUE;
}
