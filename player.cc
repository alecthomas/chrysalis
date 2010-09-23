/*****************************************************************************
    Player control class

  Author: Alec Thomas
  Update: Nov '94
  Comments:
*****************************************************************************/

#include "include.h"
#include "world.h"
#include "image.h"
#include "data.h"
#include "player.h"
#include "morph.h"
#include "monsters.h"

#define MORPHING	1

PLAYER::PLAYER( int _x, int _y, int _o )
{
	reset_data();
	x = _x; y = _y;
	xdir = 2;
	actualid = _o;
	attribute.dynamic = TRUE;
	attribute.alive = TRUE;
	attribute.creatureghost = FALSE;
	attribute.possessed = TRUE;
	score = selectedindex = 0;
	health = 60;
	lives = 3;
	memset( shape, 0, 4 * sizeof( CREATURE* ) );
	memset( shapename, 0, 4 * sizeof( char* ) );
	memset( shapeid, 0, 4 * sizeof( int ) );
	selectedshape = shape[ 0 ] = this;
	shapeid[ 0 ] = image->add_image( "PLAYER_SHAPE0", image->get_data( actualid ), 3 );
	image->setunswappable( shapeid[ 0 ] );
	shapeid[ 1 ] = image->add_image( "PLAYER_SHAPE1", image->get_data( actualid ), 3 );
	image->setunswappable( shapeid[ 1 ] );
	shapeid[ 2 ] = image->add_image( "PLAYER_SHAPE2", image->get_data( actualid ), 3 );
	image->setunswappable( shapeid[ 2 ] );
	shapeid[ 3 ] = image->add_image( "PLAYER_SHAPE3", image->get_data( actualid ), 3 );
	image->setunswappable( shapeid[ 3 ] );
	// create a dummy image for the player
	id = shapeid[ 0 ];
	// make the player's image unswappable
	image->setunswappable( actualid );
	possess( new WINGEDMONKEY( 0, 0, image->find( "CREATURE_WINGMONK" ) ), 3 );
	possess( new SLIMER( 0, 0, image->find( "CREATURE_SLIMER" ) ), 1 );
	changeshape( 0 );
}

PLAYER::~PLAYER()
{
}

void PLAYER::go()
{
	if ( K_index( kb1 ) ) changeshape( 0 );
	if ( K_index( kb2 ) ) changeshape( 1 );
	if ( K_index( kb3 ) ) changeshape( 2 );
	if ( K_index( kb4 ) ) changeshape( 3 );
	if ( K_index( kbSQUAREBRACKETOPEN ) ) changeshape( -20 );
	if ( K_index( kbSQUAREBRACKETCLOSE ) ) changeshape( +20 );
	if ( action == MORPHING )
	{
		if ( !morph->go() )
		{ // finished morphing
			delete morph;
			world->playlayer->creatureQ->remove( this );
			world->player = selectedshape = targetshape;
			if ( morph->getw() > targetshape->getw() )
				x += ( morph->getw() - targetshape->getw() ) / 2;
			else
				x -= ( morph->getw() - targetshape->getw() ) / 2;
			if ( morph->geth() > targetshape->geth() )
				y += ( morph->geth() - targetshape->geth() ) / 2;
			else
				y -= ( morph->geth() - targetshape->geth() ) / 2;
			world->player->setposition( x, y );
			world->playlayer->creatureQ->insert( priority, world->player );
			world->playlayer->creatureQ->prev();
			action = 0;
			xdisp = ydisp = 0;
			attribute.canstick = FALSE;
		} else
			attribute.canstick = TRUE;
	}
	activity.left = K_index( kbpLEFT ) ? 1 : 0;
	activity.right = K_index( kbpRIGHT ) ? 1 : 0;
	activity.up = K_index( kbpUP ) ? 1 : 0;
	activity.down = K_index( kbpDOWN ) ? 1 : 0;
	activity.shot = K_index( kbSPACE ) ? 1 : 0;
	activity.special = K_index( kbALT ) ? 1 : 0;
	world->player->performaction( &activity );
}

void PLAYER::performaction( ACTION *a )
{
	if ( action != MORPHING )
	{
		xdir = 2;
		ydir = 0;
		count++;
		frame++; if ( frame == 3 ) frame = 0;
		if ( a->left ) xdir -= 4;
		if ( a->right ) xdir += 4;
		if ( a->up ) ydir -= 4;
		if ( a->down ) ydir += 4;
		if ( world->layer->updateycollision( this ) ) ydir = 0;
		if ( world->layer->updatexcollision( this ) ) xdir = 0;
	} else
	{ // perform collision detection
		if ( !world->layer->updateycollision( this ) )
		{
			if ( ydir < 0 ) ydir++;
			if ( ydir > 0 ) ydir--;
		} else
		{
			y -= ydisp >> 2;
			ydir = 1;
		}
		if ( !world->layer->updatexcollision( this ) )
		{
			if ( xdir < 0 ) xdir++;
			if ( xdir > 0 ) xdir--;
		}
	}
}

void PLAYER::display( int xadj )
{
	if ( action == MORPHING )
		V_put( x.integer() - xadj, y.integer(), morph->image(), MASK );
	else
		if ( world->player == this )
			image->put( x - xadj, y, id, frame, TRANSLUCENT );
}

#ifdef __linux__
extern "C" int PLAYER_copy_reversed_image( char *source, char *target );
#endif

#ifdef __WATCOMC__
void PLAYER_copy_reversed_image( char *source, char *target );
/* ============================================================
	This function copies a source image to a target image, flipping it
	horizontally
*/
#pragma aux PLAYER_copy_reversed_image = \
		"mov edx, [esi]" \
		"mov ecx, [esi+4]" \
		"add esi, 8" \
		"mov [edi], edx" \
		"mov [edi+4], ecx" \
		"add edi, 8" \
	"yloop:" \
		"push ecx" \
		"add edi, edx" \
		"push edi" \
		"dec edi" \
		"mov ecx, edx" \
	"xloop:" \
		"mov al, [esi]" \
		"mov [edi], al" \
		"dec edi" \
		"inc esi" \
		"dec ecx" \
		"jnz xloop" \
		"pop edi" \
		"pop ecx" \
		"dec ecx" \
		"jnz yloop" \
	modify [eax ebx ecx edx] \
	parm [esi] [edi]
#endif

void PLAYER::possess( CREATURE *o, int index )
{
char *t;
int l, c;

	CHECK( o == NULL, "attempt to morph NULL object" );
	if ( !o->attribute.possessable || action == MORPHING ) return;
	// copy and reverse images
	c = image->getentries( o->id );
	imagelist = new char*[ c ];
	for ( l = 0; l != c; l++ )
	{
		t = image->get_data( o->id, l );
		imagelist[ l ] = new char[ ((int*)t)[ 0 ] * ((int*)t)[ 1 ] + 8 ];
		PLAYER_copy_reversed_image( t, imagelist[ l ] );
	}
	// set up target image
	image->set_image( shapeid[ index ], imagelist, c );
	shape[ index ] = o;
	o->id = shapeid[ index ];
	o->setfade( 0 );
}

void PLAYER::changeshape( int id )
/* ================================
	Change to a shape that's already in the list
*/
{
char *sourceimage, *targetimage;

	if ( id == +20 )
	{
		id = selectedindex;
		do {
			id++;
			if ( id > 3 ) id = 0;
			if ( id == selectedindex ) return;
		} while ( shape[ id ] == NULL || shape[ id ] == this ||
			action == MORPHING || shape[ id ] == selectedshape );
	}
	if ( id == -20 )
	{
		id = selectedindex;
		do {
			id--;
			if ( id < 0 ) id = 3;
			if ( id == selectedindex ) return;
		} while ( shape[ id ] == NULL || shape[ id ] == this ||
			action == MORPHING || shape[ id ] == selectedshape );
	}
	if ( id < 0 ) id = 3;
	if ( id > 3 ) id = 0;
	if ( shape[ id ] == NULL || shape[ id ] == this ||
		action == MORPHING || shape[ id ] == selectedshape ) return;
	xdir = selectedshape->xdir; ydir = selectedshape->ydir;
	attribute.creatureghost = TRUE;
	shape[ id ]->setposition( x, y );
	shape[ id ]->attribute.possessed = TRUE;
	shape[ id ]->attribute.sceneryghost = FALSE;
	shape[ id ]->attribute.creatureghost = FALSE;
	shape[ id ]->health = selectedshape->health;
	health = shape[ id ]->health;
	shape[ id ]->id = shapeid[ id ];
	shape[ id ]->reset();
	shape[ id ]->xdir = shape[ id ]->ydir = 0;
	targetshape = shape[ id ];
	selectedindex = id;
	morph = new MORPH( selectedshape->getimage(), targetshape->getimage() );
	x = selectedshape->getx(); y = selectedshape->gety();
	if ( selectedshape->getw() < targetshape->getw() )
	{
		xdisp = ( targetshape->getw() - selectedshape->getw() ) / 2;
		x -= xdisp;
	} else
		xdisp = -( targetshape->getw() - selectedshape->getw() ) / 2;
	if ( selectedshape->geth() < targetshape->geth() )
	{
		ydisp = ( targetshape->geth() - selectedshape->geth() ) / 2;
		y -= ydisp;
	} else
		ydisp = -( targetshape->geth() - selectedshape->geth() ) / 2;
	action = MORPHING;
	if ( selectedshape != this )
	{
		world->layer->creatureQ->remove( selectedshape );
		world->layer->creatureQ->insert( -2000000000, this );
	}
	world->player = this;
	selectedshape = targetshape;
	attribute.sceneryghost = selectedshape->attribute.sceneryghost;
	attribute.creatureghost = selectedshape->attribute.creatureghost;
	world->layer->creatureQ->reset();
}

void PLAYER::setshape( int id )
/* ================================
	Set a shape immediately (don't morph)
*/
{
	if ( id == +20 )
	{
		id = selectedindex;
		do {
			id++;
			if ( id > 3 ) id = 0;
			if ( id == selectedindex ) return;
		} while ( shape[ id ] == NULL || shape[ id ] == this ||
			action == MORPHING || shape[ id ] == selectedshape );
	}
	if ( id == -20 )
	{
		id = selectedindex;
		do {
			id--;
			if ( id < 0 ) id = 3;
			if ( id == selectedindex ) return;
		} while ( shape[ id ] == NULL || shape[ id ] == this ||
			action == MORPHING || shape[ id ] == selectedshape );
	}
	if ( id < 0 ) id = 3;
	if ( id > 3 ) id = 0;
	if ( shape[ id ] == NULL || shape[ id ] == this ||
		action == MORPHING || shape[ id ] == selectedshape ) return;
	xdir = selectedshape->xdir; ydir = selectedshape->ydir;
	shape[ id ]->setposition( x, y );
	shape[ id ]->attribute.possessed = TRUE;
	shape[ id ]->attribute.sceneryghost = FALSE;
	shape[ id ]->attribute.creatureghost = FALSE;
	shape[ id ]->health = selectedshape->health;
	health = shape[ id ]->health;
	shape[ id ]->id = shapeid[ id ];
	shape[ id ]->reset();
	shape[ id ]->xdir = shape[ id ]->ydir = 0;
	targetshape = shape[ id ];
	selectedindex = id;
/*	x = selectedshape->getx(); y = selectedshape->gety();
	if ( selectedshape->getw() < targetshape->getw() )
	{
		xdisp = ( targetshape->getw() - selectedshape->getw() ) / 2;
		x -= xdisp;
	} else
		xdisp = -( targetshape->getw() - selectedshape->getw() ) / 2;
	if ( selectedshape->geth() < targetshape->geth() )
	{
		ydisp = ( targetshape->geth() - selectedshape->geth() ) / 2;
		y -= ydisp;
	} else
		ydisp = -( targetshape->geth() - selectedshape->geth() ) / 2;*/
	if ( selectedshape != this )
		world->layer->creatureQ->remove( selectedshape );
	else
		world->layer->creatureQ->remove( this );
	selectedshape = targetshape;
	attribute.sceneryghost = selectedshape->attribute.sceneryghost;
	attribute.creatureghost = selectedshape->attribute.creatureghost;
	world->player = selectedshape = targetshape;
	world->playlayer->creatureQ->insert( priority, world->player );
	action = 0;
	xdisp = ydisp = 0;
	attribute.canstick = FALSE;
}

void PLAYER::display_shapeicon( int x, int y, int index )
/* ==================================================
	Display an icon for a shape
*/
{
	if ( shape[ index ] == this || shape[ index ] == NULL ) return;
	if ( shape[ index ] == selectedshape )
		V_box( x - 1, y - 1, 18, 18, 255 );
	shape[ index ]->display_icon( x, y, MASK );
}
