#include "include.h"
#include "object.h"
#include "world.h"

int layershade[ 4 ] = { -4, -2, 0, +2 };
int layertranslucentshade[ 4 ] = { TRANSLUCENTFADE( -4 ), TRANSLUCENTFADE( -2 ), TRANSLUCENT,
		TRANSLUCENTFADE( +2 ) };

void OBJECT::display( int xadj )
/* ======================================
	Default object display
*/
{
	image->put( x - xadj, y, id, frame, fade );
}

void OBJECT::go()
/* =======================
	Default object action routine
*/
{
	x += xdir; y += ydir;
	if ( x + getw() < world->getposition() || y + geth() < 0 || y > 200 ) attribute.alive = FALSE;
}

void OBJECT::save( FILE *f )
/*          =================
	Saves the current object to disk for later loading (saving levels!!)
*/
{
	fwrite( &x.number, 1, sizeof( int ), f );
	fwrite( &y.number, 1, sizeof( int ), f );
	fwrite( &fade, 1, sizeof( int ), f );
	fwrite( &attribute, 1, sizeof( ATTRIBUTE ), f );
}

void OBJECT::load( FILE *f )
/*          =================
	Load the current object. ** NOTE ** the id will already be
	loaded (for obvious reasons)
*/
{
	fread( &x.number, 1, sizeof( int ), f );
	fread( &y.number, 1, sizeof( int ), f );
	fread( &fade, 1, sizeof( int ), f );
	fread( &attribute, 1, sizeof( ATTRIBUTE ), f );
}

void OBJECT::reset_data()
/* =================
	Used internally by objects to reset their data
*/
{
	memset( &attribute, 0, sizeof( ATTRIBUTE ) );
	frame = action = subaction = count = 0;
	xdir = ydir = xdisp = ydisp = 0;
	damage = health = 1;
	fade = FADE( layershade[ world->getlayer() ] );
	scenerycollision = NULL;
	creaturecollision = NULL;
	attribute.creatureghost = TRUE;
}

int OBJECT::checkcollision( OBJECT *o )
/* ===============================
	Return TRUE if two objects collide
*/
{
	if ( o->attribute.scenery )
		if ( attribute.sceneryghost ) return FALSE; else ;
	else
		if ( attribute.creatureghost && o->attribute.creatureghost ) return FALSE;
	if ( ::touching( o->getx().integer(), o->gety().integer(), o->getw(), o->geth(),
		x.integer(), y.integer(), getw(), geth() ) && ( o->attribute.solid || !o->attribute.scenery ) )
	{
		if ( o->attribute.scenery ) 
			scenerycollision = o; 
		else 
			creaturecollision = o;
		if ( attribute.scenery )
			o->scenerycollision = this;
		else
			o->creaturecollision = this;
		return TRUE;
	}
	return FALSE;
}

void OBJECT::setposition( fixed _x, fixed _y ) 
{ 
	x = _x; 
	y = _y; 
}

fixed OBJECT::getx() 
{ 
	return x; 
}

fixed OBJECT::gety() 
{ 
	return y; 
}

int OBJECT::getw() 
{ 
	return image->getw( id, frame ); 
}

int OBJECT::geth() 
{ 
	return image->geth( id, frame ); 
}

int OBJECT::touching( fixed x, fixed y ) 
{ 
	return ::touching( x.integer(), y.integer(), OBJECT::x.integer(), OBJECT::y.integer(), getw(), geth() ); 
}

void OBJECT::setfade( int f ) 
{ 
	fade = FADE( f ); 
}

OBJECT::OBJECT()
{
}

OBJECT::OBJECT( fixed x, fixed y, int speed )
{
}

OBJECT::~OBJECT() 
{
}
