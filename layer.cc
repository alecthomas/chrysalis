/*****************************************************************************
    This class controls each layer individually

    Author: Alec Thomas
    Update: 10th Dec '94
    Comments:
*****************************************************************************/
#include "include.h"
#include "world.h"
#include "monsters.h"
#include "scenery.h"
#include "player.h"

LAYER::LAYER( int depth )
/*           =============
	Layer constructor
*/
{
	int w;
	// Create virtual page
	
	objectwrap = FALSE;
	switch ( depth )
	{
		case 0 : // static background
			edgewidth = 0;
			w = WORLDWIDTH;
		break;
		case 1 : // parallax background (1 pixel scroll, 2 page wrap)
			edgewidth = 1;
			w = WORLDWIDTH * 4;
			objectwrap = TRUE;
		break;
		case 2 : // parallax play layer (2 pixel scroll, 16 pages)
			edgewidth = 2;
			w = WORLDWIDTH * 16;
		break;
		case 3 : // parallax foreground (4 pixel scroll, 4 page wrap)
			edgewidth = 4;
			w = WORLDWIDTH * 8;
			objectwrap = TRUE;
		break;
	}
	CHECK( ( page = V_createpage( w, WORLDHEIGHT ) ) == NULL, "could not allocate layer" );
	// Set miscellaneous stuff
	worldposition = position = 0;
	// Create object queues
	naturalQ = new QUEUE <OBJECT>();
	sceneryQ = new QUEUE <OBJECT>();
	creatureQ = new QUEUE <OBJECT>();
	Q = new QUEUE <OBJECT>();
}

LAYER::~LAYER()
/*    ==========
	Layer destructor
*/
{
OBJECT *o;
	
	// Delete queue objects
	o = naturalQ->reset();
	while ( o != NULL )
	{
		delete o;
		o = naturalQ->next();
	}

	// Delete object queues
	delete Q;
	delete creatureQ;
	delete sceneryQ;
	delete naturalQ;
	
	// Delete virtual pages
	V_deletepage( page );
}

void LAYER::reset()
/*  =================
	Redraw all objects in entire level! Reset x-ordered Q, discarding dud
	objects
*/
{
	OBJECT *o;
	
	V_setpage( page );
	// clear page
	memset( page->ptr, 0, page->w * page->h );
	// delete Q's
	delete Q;
	delete creatureQ;
	delete sceneryQ;
	sceneryQ = new QUEUE <OBJECT>();
	creatureQ = new QUEUE <OBJECT>();
	Q = new QUEUE <OBJECT>();
	// draw all objects
	o = naturalQ->reset();
	while ( o != NULL )
	{
		if ( !o->attribute.dynamic )
		{ // object is static, put it down
			o->display( 0 );
			if ( objectwrap )
			{
				o->display( page->w );
				o->display( -page->w );
			}
			if ( edgewidth == 2 )
				// if the static object is on the play level, add it to "Q"
				Q->insert( o->getx().integer(), o );
		} else
		{
			// if the object is dynamic, add it to "Q" regardless of layer
			Q->insert( o->getx().integer(), o );
		}
		o = naturalQ->next();
	}
	V_restorepage();
}

#ifdef __linux__
extern "C" void __blit4__( char *targ, char *src, int len );
#endif

#ifdef __WATCOMC__
void __blit4__( char *targ, char *src, int len );
#pragma aux __blit4__ = \
		"mov edx, ecx" \
		"and edx, 11b" \
		"mov eax, 0" \
		"sub eax, eax" \
		"xchg esi, edi" \
		"sub esi, edi" \
		"shr ecx, 2" \
	"maskLOOP:" \
		"repe scasd" \
		"jz exitblit" \
		"sub edi, 4" \
		"inc ecx" \
	"maskLOOP2:" \
		"mov eax, [edi]" \
		"cmp eax, 0" \
		"je maskLOOP" \
		"mov ebx, edi" \
		"add ebx, esi" \
		"cmp al, 0" \
		"je s1" \
		"mov [ebx], al" \
	"s1:" \
		"inc ebx" \
		"cmp ah, 0" \
		"je s2" \
		"mov [ebx], ah" \
	"s2:" \
		"inc ebx" \
		"shr eax, 16" \
		"cmp al, 0" \
		"je s3" \
		"mov [ebx], al" \
	"s3:" \
		"cmp ah, 0" \
		"je s4" \
		"inc ebx" \
		"mov [ebx], ah" \
	"s4:" \
		"add edi, 4" \
		"dec ecx" \
		"jnz maskLOOP2" \
	"exitblit:" \
		"cmp edx, 0" \
		"je superexit" \
	"eloop:" \
		"mov al, [edi]" \
		"cmp al, 0" \
		"je s5" \
		"mov [edi+esi], al" \
	"s5:" \
		"inc edi" \
		"dec edx" \
		"jnz eloop" \
	"superexit:" \
		parm [edi] [esi] [ecx] \
		modify [eax ebx edx]
#endif
		
void LAYER::updateobjects()
/* =========================
	Update main object list, moving objects into creatureQ, etc.
	Doesn't actually do anything other than branch objects into their
	correct queues.
*/
{
OBJECT *o;
	
	// Go through main x-ordered list, moving objects into their
	// appropriate queues
	o = Q->reset();
	while ( o != NULL )
	{
		// If object is not on screen (off right side) stop search
		if ( o->getx() > worldposition + WORLDWIDTH ) break;
		Q->remove();
		if ( o->attribute.scenery )
		{ // insert into scenery Q
			if ( o->attribute.solid || o->attribute.dynamic ) sceneryQ->insert( o->priority, o );
		} else
			// object is not scenery, therefore it's a creature
			creatureQ->insert( o->priority, o );
		o = Q->retrieve();
	}

	// Go through scenery list, discarding scenery that has disappeared
	o = sceneryQ->reset();
	while ( o != NULL )
	{
		if ( o->getx() + o->getw() < worldposition )
		{ // Delete object from sceneryQ if it's off the screen
			sceneryQ->remove();
			if ( o->attribute.dynamic )
				if ( objectwrap )
				{ // if layer wraps, move it so it will wrap turn up again
					o->setposition( o->getx() + page->w, o->gety() );
					sceneryQ->insert( o->getx().integer(), o );
					o = sceneryQ->next();
					continue;
				}
			if ( o->attribute.temporary ) delete o;
			if ( ( o = sceneryQ->retrieve() ) != NULL ) continue;
			break;
		}
		o = sceneryQ->next();
	}

	// Go through creature list
	o = creatureQ->reset();
	while ( o != NULL )
	{
		if ( !o->attribute.alive && !o->attribute.dying )
		{ // it says it's dead, so remove from creatureQ
			creatureQ->remove();
			if ( o->attribute.temporary ) delete o;
			if ( ( o = creatureQ->retrieve() ) != NULL ) continue;
			break;
		}
		o = creatureQ->next();
	}
}

void LAYER::go()
/* ==============
	Scroll, update objects, etc.
*/
{
	display();
	updateobjects();
	displayobjects();
	scroll( 1 );
}

OBJECT *LAYER::checkscenerycollisions( OBJECT *o )
/* ================================================
	Check all scenery collisions against an object
*/
{
OBJECT *scenery = NULL;

	if ( !o->attribute.dying && !o->attribute.sceneryghost && !o->scenerycollision )
	{ // do scenery collisions, moving away from it if necessary
		// check collisions with scenery
		scenery = sceneryQ->reset();
		while ( scenery != NULL )
		{
			if ( scenery->attribute.solid )
				if ( o->checkcollision( scenery ) )
					break;
			scenery = sceneryQ->next();
		}
	}
	return scenery;
}

OBJECT *LAYER::checkcreaturecollisions( OBJECT *o )
/* =================================================
	Check creature collisions in the current layer
*/
{
OBJECT *c = NULL;

	if ( !o->attribute.dying && !o->attribute.creatureghost && !o->creaturecollision )
	{ // do creature collisions
		c = creatureQ->reset();
		while ( c != NULL )
		{
			if ( c->attribute.possessed == o->attribute.possessed )
				if ( o->checkcollision( c ) )
					break;
			c = creatureQ->next();
		}
	}
	return c;
}

void LAYER::displayobjects()
/* =========================
	Update active objects, check for collisions
*/
{
OBJECT *o, *creature, *scenery, *sceneryB;
	
	// Reset creature collisions
	o = creatureQ->reset();
	while ( o != NULL )
	{
		o->resetcollision();
		o = creatureQ->next();
	}
	// Do collision detection
	o = creatureQ->reset();
	while ( o != NULL )
	{
		// check collisions with creatures
		if ( !o->attribute.dying && !o->attribute.creatureghost && !o->creaturecollision )
		{
			creature = creatureQ->reset();
			while ( creature != NULL )
			{
				if ( o->attribute.possessed != creature->attribute.possessed )
					if ( o->checkcollision( creature ) )
					{
						o->health -= creature->damage;
						creature->health -= o->damage;
						break;
					}
				creature = creatureQ->next();
			}
		}
		o = creatureQ->next();
	}

	// Do creature logic
	o = creatureQ->reset();
	while ( o != NULL )
	{
		// do creature logic
		if ( o != world->player || o == world->actualplayer ) o->go();
		if ( creatureQ->retrieve() != o )
			// removed itself ?!?!?
			o = creatureQ->retrieve(); 
		if ( !o->attribute.alive && !o->attribute.dying )
		{ // Remove from creatureQ
			creatureQ->remove();
			if ( o->attribute.temporary ) delete o;
			if ( ( o = creatureQ->retrieve() ) != NULL ) continue;
			break;
		} 
		o->display( worldposition );
		if ( flags.superdebug )
		{
			V_box( o->getx().integer() - world->layer->position, o->gety().integer(), o->getw(), o->geth(), 248 );
			if ( o->creaturecollision )
				V_box( o->getx().integer() - world->layer->position, o->gety().integer(), o->getw(), o->geth(), 30 );
			if ( o->scenerycollision )
				V_box( o->getx().integer() - world->layer->position, o->gety().integer(), o->getw(), o->geth(), 255 );
		}
		o = creatureQ->next();
	}
	// Go through scenery, updating dynamic scenery
	o = sceneryQ->reset();
	while ( o != NULL )
	{
		if ( o->attribute.dynamic )
		{ // dynamic scenery, can be doing tricky things ;)
			o->go();
			o->display( worldposition );
		}
		o = sceneryQ->next();
	}
}

void LAYER::display()
/*         ===========
	Display layer to active virtual page <=> (WORLDWIDTH * WORLDHEIGHT)
*/
{
int l, w;
	
	// Display layer to active screen
	switch ( edgewidth )
	{
		case 0 : // background
			memcpy( V_screen->ptr, page->ptr, WORLDWIDTH * WORLDHEIGHT );
		break;
		case 1 : // parallax background
		case 4 : // parallax foreground
			if ( position + WORLDWIDTH > page->w )
			{
				w = page->w - position;
				for ( l = 0; l != WORLDHEIGHT; l++ )
				{
					__blit4__( V_screen->ytable[ l ], page->ytable[ l ] + position, w );
					__blit4__( V_screen->ytable[ l ] + w, page->ytable[ l ], WORLDWIDTH - w );
				}
			} else
				for ( l = 0; l != WORLDHEIGHT; l++ )
					__blit4__( V_screen->ytable[ l ], page->ytable[ l ] + position, WORLDWIDTH );
		break;
		case 2 : // active layer
			for ( l = 0; l != WORLDHEIGHT; l++ )
				__blit4__( V_screen->ytable[ l ], page->ytable[ l ] + position, WORLDWIDTH );
		break;
	}
}

void LAYER::displayall()
{
OBJECT *o;

	display();
	o = naturalQ->reset();
	while ( o != NULL )
	{
		if ( o->attribute.dynamic )
		{
			if ( o->attribute.scenery && objectwrap )
			{
				o->display( position );
				o->display( position - page->w );
			}
			else
				o->display( worldposition );
		}
		o = naturalQ->next();
	}
}

OBJECT *LAYER::insertobject( int priority, OBJECT *object )
/* =========================================================
	Insert an object
*/
{
	object->priority = priority;
	naturalQ->insert( priority, object );
	Q->insert( object->getx().integer(), object );
	return object;
}

OBJECT *LAYER::insertobject( int x, int y, int pri, char *n )
/*                   ==============================================
	This is the routine which inserts all objects, scenery, etc. into
	the layer
*/
{
OBJECT *obj = NULL;
int o;

	obj = world->getobjectfromname( n );
	CHECK( obj == NULL, "invalid object for insertion" );
	obj->setposition( x, y );
	insertobject( pri, obj );
	return obj;
}

OBJECT *LAYER::insertobject( int x, int y, int pri, int n )
{
	return insertobject( x, y, pri, image->name( n ) );
}

OBJECT *LAYER::insertobject( int p, char *ot, FILE *io )
/*             =================================
	This insertion routine inserts an object loaded from disk
*/
{
OBJECT *o;
	
	o = insertobject( 0, 0, p, ot );
	o->load( io );
	return o;
}

void LAYER::usepage()
/*         ===========
	Make the virtual screen point to this layer
*/
{
	V_setpage( page );
}

void LAYER::scroll( int dir )
/*         =====================
	Scroll screens
*/
{
	worldposition += edgewidth * dir;
	position += edgewidth * dir;
	if ( position > page->w ) position -= page->w;
	if ( position < 0 ) position += page->w;
}

void LAYER::save( FILE *io )
/* ==========================
	Save the current layer to disk
*/
{
OBJECT *o;
int l = 0, id;
char *name;
	
	o = naturalQ->reset();
	l = naturalQ->items();
	// write number of objects in layer (4 times for a limited checksum)
	fwrite( &l, 1, sizeof( int ), io );
	l = ~l;
	fwrite( &l, 1, sizeof( int ), io );
	l = ~l;
	fwrite( &l, 1, sizeof( int ), io );
	l = ~l;
	fwrite( &l, 1, sizeof( int ), io );
	l = ~l;
	l = 0;
	o = naturalQ->reset();
	while ( o != NULL )
	{
		name = image->name( o->id );
		l = world->nameindex( name );
		fwrite( &l, 1, sizeof( int ), io ); // write object name
		l = naturalQ->priority();
		fwrite( &l, 1, sizeof( int ), io ); // write object priority
		o->save( io );
		o = naturalQ->next();
	}
}

int LAYER::load( FILE *io )
/* ==========================
	Load a layer from disk
*/
{
OBJECT *o;
int l, c[ 4 ], p, id;
char *name, sbuf[ 128 ];
	
	// delete previous data
	o = naturalQ->reset();
	while ( o != NULL )
	{
		delete o;
		o = naturalQ->next();
	}
	
	// Delete and reallocate queues (to clear them out)
	delete Q;
	Q = new QUEUE <OBJECT>();
	delete creatureQ;
	creatureQ = new QUEUE <OBJECT>();
	delete naturalQ;
	naturalQ = new QUEUE <OBJECT>();
	
	// read number of objects in layer
	fread( c, 1, sizeof( int ) * 4, io );
	CHECK( c[ 0 ] != ~c[ 1 ] || c[ 0 ] != c[ 2 ] || c[ 0 ] != ~c[ 3 ], "level checksum failed" );
	for ( l = 0; l != c[ 0 ]; l++ )
	{
		fread( &p, 1, sizeof( int ), io );
		CHECK( p >= world->nameindices, "invalid image index" );
		name = world->namelist[ p ];
		fread( &p, 1, sizeof( int ), io );
		o = insertobject( p, name, io );
		o->priority = p;
	}
	return TRUE;
}

CREATURE *LAYER::inserttemporarycreature( int priority, CREATURE *creature )
/* ==============================================================
	Insert a creature directly into creatureQ, object should be have the
	"temporary" attribute set.
*/
{
	creature->priority = priority;
	creatureQ->insert( priority, creature );
	return creature;
}

SCENERY *LAYER::inserttemporaryscenery( int priority, SCENERY *scenery )
/* ==========================================================
	Insert scenery directly into scenery Q's, either dynamic or static
*/
{
	scenery->priority = priority;
	if ( !scenery->attribute.dynamic )
	{ // object is static, put it down
		V_setpage( page );
		scenery->display( 0 );
		sceneryQ->insert( priority, scenery );
		V_restorepage();
	} else
		// if the object is dynamic, add it to "Q" regardless of layer
		sceneryQ->insert( priority, scenery );
	return scenery;
}

OBJECT *LAYER::updatexcollision( OBJECT *o )
/* ==========================================
	Move an object in XDIR, checking scenery collisions as it goes.
	This routine moves through EVERY bit of scenery, finding the closest
	scenery on the x direction, then moves away from it.
*/
{
OBJECT *s, *closest = NULL;
fixed x, xd = o->xdir;

	if ( o->xdir == 0 ) return NULL;
	o->x += o->xdir;
	if ( o->attribute.sceneryghost ) return NULL;
	s = sceneryQ->reset();
	if ( o->xdir < 0 )
	{ // object is moving left
		x = -1000000;
		while ( s != NULL )
		{
			if ( s->attribute.solid )
				if ( o->checkcollision( s ) )
					if ( s->getx() + s->getw() > x )
					{
						x = s->getx();
						closest = s;
					}
			s = sceneryQ->next();
		}
		if ( closest != NULL )
			do {
				o->x++;
				xd++;
				/* only move a maximum of XDIR pixels away from object, so we don't
				   get big jumps */
				if ( xd == 0 )
				{
					if ( !o->attribute.canstick && o->checkcollision( closest ) )
					{
						o->health--;
						o->x += o->xdir;
						return NULL;
					}
					break;
				}
			} while ( o->checkcollision( closest ) );
	} else
	{ // object is moving right
		x = 1000000;
		while ( s != NULL )
		{
			if ( s->attribute.solid )
				if ( o->checkcollision( s ) )
					if ( s->getx() < x )
					{
						x = s->getx();
						closest = s;
					}
			s = sceneryQ->next();
		}
		if ( closest != NULL )
			do {
				o->x--;
				xd--;
				/* only move a maximum of XDIR pixels away from object, so we don't
				   get big jumps */
				if ( xd == 0 )
				{
					if ( !o->attribute.canstick && o->checkcollision( closest ) )
					{
						o->health--;
						o->x += o->xdir;
						return NULL;
					}
					break;
				}
			} while ( o->checkcollision( closest ) );
	}
	return closest;
}

OBJECT *LAYER::updateycollision( OBJECT *o )
/* ==========================================
	Move an object in YDIR, checking scenery collisions as it goes.
	This routine moves through EVERY bit of scenery, finding the closest
	scenery on the y direction, then moves away from it.
*/
{
OBJECT *s, *closest = NULL;
fixed y, yd = o->ydir;

	if ( o->ydir == 0 ) return NULL;
	o->y += o->ydir;
	if ( o->attribute.sceneryghost ) return NULL;
	s = sceneryQ->reset();
	if ( o->ydir < 0 )
	{ // object is moving up
		y = -1000000;
		while ( s != NULL )
		{
			if ( s->attribute.solid )
				if ( o->checkcollision( s ) )
					if ( s->gety() + s->geth() > y )
					{
						y = s->gety();
						closest = s;
					}
			s = sceneryQ->next();
		}
		if ( closest != NULL )
			do {
				o->y++;
				yd++;
				/* only move a maximum of YDIR pixels away from object, so we don't
				   get big jumps */
				if ( yd == 0 )
				{
					if ( !o->attribute.canstick && o->checkcollision( closest ) )
					{
						o->health--;
						o->y += o->ydir;
						return NULL;
					}
					break;
				}
			} while ( o->checkcollision( closest ) );
	} else
	{ // object is moving down
		y = 1000000;
		while ( s != NULL )
		{
			if ( s->attribute.solid )
				if ( o->checkcollision( s ) )
					if ( s->gety() < y )
					{
						y = s->gety();
						closest = s;
					}
			s = sceneryQ->next();
		}
		if ( closest != NULL )
			do {
				o->y--;
				yd--;
				/* only move a maximum of YDIR pixels away from object, so we don't
				   get big jumps */
				if ( yd == 0 )
				{
					if ( !o->attribute.canstick && o->checkcollision( closest ) )
					{
						o->health--;
						o->y += o->ydir;
						return NULL;
					}
					break;
				}
			} while ( o->checkcollision( closest ) );
	}
	return closest;
}
