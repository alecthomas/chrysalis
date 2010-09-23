/*****************************************************************************
    Controls monsters, objects and events inside layers

  Author: Alec Thomas, Eric Sambach
  Update: Nov '94
  Comments:
*****************************************************************************/

#include "include.h"
#include <unistd.h>
#include "data.h"
#include "world.h"
#include "player.h"
#include "image.h"
#include "monsters.h"
#include "scenery.h"

// Global variables
WORLD *world;

int WORLD::firstlevel = 0;
int WORLD::layerswitch[ 4 ] = { TRUE, TRUE, TRUE, TRUE };

WORLD::WORLD()
/*    ======================================
	World (level) constructor, load up a level, swap in images, etc.
*/
{
int l;
	
	// Set up events
	event = new QUEUE <EVENT>();
	
	// Allocate page
#ifdef __X11__
	page = V_VGAscreen;
#else
	page = V_createpage( WORLDWIDTH, WORLDHEIGHT );
#endif
	
	// Construct layers
	for ( l = 0; l != 4; l++ ) layerlist[ l ] = new LAYER( l );
	playlayer = layerlist[ 2 ];
	setlayer( 0 );
	
	// Set default stuff
	lib = ::lib;
	image = ::image;
}

WORLD::~WORLD()
/*    ==========
	World destructor, swap out all images, delete layers, etc.
*/
{
int l;
	
	// delete layers
	for ( l = 0; l != 4; l++ ) delete layerlist[ l ];
}

void WORLD::load( int lvl )
/*         =================
	Load the level "LEVEL_xxxx" where xxxx is "lvl"
*/
{
int l, cnt, id;
OBJECT *c;
char name[ 64 ], lstr[ 64 ], lvstr[ 6 ];
	
	level = lvl;
	strcpy( lstr, "LEVEL_" );
	sprintf( lvstr, "%i", lvl );
	for ( l = 0; l != 4 - strlen( lvstr ); l++ )
		strcat( lstr, "0" );
	strcat( lstr, lvstr );
	// delete current level
	for ( l = 0; l != 4; l++ )
	{
		delete layerlist[ l ];
		layerlist[ l ] = new LAYER( l );
	}
	playlayer = layerlist[ 2 ];
	CHECK( lib->seek( lstr ) == -1, "level not found" );
	// Read name index list
	fread( &nameindices, 1, sizeof( int ), lib->io );
	for ( l = 0; l != nameindices; l++ )
	{
		fread( name, 1, 32, lib->io );
		if ( ( id = image->find( name ) ) == -1 )
		{
			strcpy( lstr, "image \"" );
			strcat( lstr, name );
			strcat( lstr, "\" not in current image library" );
			CHECK( TRUE, lstr );
		}
		namelist[ l ] = image->name( id );
	}
	// load layers
	for ( l = 0; l != 4; l++ )
	{
		setlayer( l );
		layer->load( lib->io );
		layer->reset();
	}
}

void WORLD::save( int lvl )
/*        =================
Save the current level data to disk.
*/
{
int l, pos, len;
char lstr[ 32 ], lvstr[ 6 ], *buffer, tmpbuffer[ 128 ];
FILE *tmp;
OBJECT *o;
	
	// Make level name string and check for existing levels
	level = lvl;
	strcpy( lstr, "LEVEL_" );
	sprintf( lvstr, "%i", lvl );
	for ( l = 0; l != 4 - strlen( lvstr ); l++ )
		strcat( lstr, "0" );
	strcat( lstr, lvstr );
	tmpbuffer[ 0 ] = 0;
	if ( getenv( "tmp" ) != NULL )
		strcpy( tmpbuffer, getenv( "tmp" ) );
	else
		if ( getenv( "temp" ) != NULL )
			strcpy( tmpbuffer, getenv( "temp" ) );
	strcat( tmpbuffer, "crysalis.$$$" );
	CHECK( ( tmp = fopen( tmpbuffer, "w+b" ) ) == NULL, "could not open temporary level data file" );
	// Make name index list
	nameindices = 0;
	for ( l = 0; l != 4; l++ )
	{
		setlayer( l );
		o = layer->naturalQ->reset();
		while ( o != NULL )
		{
			if ( nameindex( image->name( o->id ) ) == -1 )
			{ // doesn't exist, add to list
				namelist[ nameindices ] = image->name( o->id );
				nameindices++;
			}
			o = layer->naturalQ->next();
		}
	}
	// Save index
	fwrite( &nameindices, 1, sizeof( int ), tmp );
	for ( l = 0; l != nameindices; l++ )
		fwrite( namelist[ l ], 1, 32, tmp );
	// Save level data
	for ( l = 0; l != 4; l++ )
	{
		setlayer( l );
		layer->save( tmp );
	}
	len = ftell( tmp );
	fclose( tmp );
	// Read in temporary file into buffer and save to LIB
	CHECK( ( buffer = new char[ len ] ) == NULL, "could not allocate level save buffer" );
	tmp = fopen( tmpbuffer, "rb" );
	fread( buffer, 1, len, tmp );
	fclose( tmp );
	unlink( tmpbuffer );
	CHECK( lib->add( lstr, buffer, len ) == FALSE, "could not add level to data library" );
	delete buffer;
}

void WORLD::go()
/*         ======
	This is the main game control loop
*/
{
int l;
OBJECT *scenery, *active;
	
	if ( actualplayer != player ) actualplayer->go();
	V_setpage( page );
	// Display layers and scroll
	for ( l = 0; l != 4; l++ )
		if ( layerswitch[ l ] )
		{
			setlayer( l );
			layer->go();
		}
}

void WORLD::display()
{
int l;

	// Display screen
	V_showpage();
/*	for ( l = 0; l != WORLDHEIGHT; l++ )
		memcpy( V_VGAscreen->ytable[ WORLDSCREENY + l ] + WORLDSCREENX,
			V_screen->ytable[ l ], WORLDWIDTH );*/
}


int WORLD::nameindex( char *name )
{
int l;

	for ( l = 0; l != nameindices; l++ )
		if ( !strcmp( name, namelist[ l ] ) ) return l;
	return -1;
}

void WORLD::createplayer()
{
	world->playlayer->creatureQ->insert( -2000000000, ( actualplayer = new PLAYER( 80,
		80, image->find( "CREATURE_PLAYER" ) ) ) );
	player = (CREATURE*)actualplayer;
}

OBJECT *WORLD::getobjectfromname( char *n )
/* ============================================
	Return an object pointer, given a name
*/
{
int o;

	o = image->find( n );
	// Do creature stuff
	if ( !strncmp( n, "CREATURE_", 9 ) )
	{ // creatures of all shapes and sizes ;)
		return getcreaturefromname( n );
	} else
		if ( !strncmp( n, "ACTIVE_", 7 ) )
		{ // active scenery
			n += 7;
			if ( !strcmp( n, "WATER" ) ) return new WATER( 0, 0, o );
			if ( !strcmp( n, "SOLIDWATER" ) || !strncmp( n, "WATRFALL", 8 ) ) return new SOLIDWATER( 0, 0, o );
			// this handles all slime*.* files
			if ( !strncmp( n, "SLIME", 5 ) ) return new SLIME( 0, 0, o );
			if ( !strcmp( n, "SWNGVINE" ) ) return new SWINGINGVINE( 0, 0, o );
		} else
		{ // Do scenery stuff ;)
			return new SCENERY( 0, 0, o );
		}
	return NULL;
}

CREATURE *WORLD::getcreaturefromname( char *n )
{
int o;

	o = image->find( n );
	// Do creature stuff
	if ( !strncmp( n, "CREATURE_", 9 ) )
	{ // creatures of all shapes and sizes ;)
		n += 9;
		// objects that are images
		if ( !strcmp( n, "WINGMONK" ) ) return new WINGEDMONKEY( 0, 0, o );
		if ( !strcmp( n, "TRIFFID" ) ) return new TRIFFID( 0, 0, o );
		if ( !strcmp( n, "SLIMER" ) ) return new SLIMER( 0, 0, o );
		if ( !strcmp( n, "PLAYER" ) ) return new PLAYER( 0, 0, o );
	}
	return NULL;
}
