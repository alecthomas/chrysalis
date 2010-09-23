#include "include.h"
#include "scenery.h"
#include "world.h"

WATER::WATER( fixed cx, fixed cy, int o )
{
	reset_data();
	x = cx; y = cy;
	attribute.dynamic = TRUE;
	attribute.scenery = TRUE;
	frame = rand() % 12;
	id = o;
	fade = layertranslucentshade[ world->getlayer() ];
}

/*void WATER::display( int xadj )
{
	image->put( x - xadj, y, id, frame, fade );
}*/

void WATER::go()
{
	frame++; if ( frame >= 12 ) frame = 0;
}

SOLIDWATER::SOLIDWATER( fixed cx, fixed cy, int o )
{
	reset_data();
	x = cx; y = cy;
	attribute.dynamic = TRUE;
	attribute.scenery = TRUE;
	id = o;
	bubbleactive = FALSE;
	bubbleid = image->find( "SCENERY_BUBBLE" );
	fade = layertranslucentshade[ world->getlayer() ];
}

void SOLIDWATER::display( int xadj )
{
	image->put( x - xadj, y, id, frame, fade );
/*	if ( bubbleactive )
		if ( bubbleframe < 5 )
			image->put( bubblex - xadj + __unfixed( sin( bubbleaction << 3, bubbleframe << 2 ) ), bubbley, bubbleid, bubbleframe, fade );
		else
			image->put( bubblex - xadj + __unfixed( sin( bubbleaction << 3, 5 << 2 ) ), bubbley, bubbleid, bubbleframe, fade );*/
}

void SOLIDWATER::go()
{
	if ( bubbleactive )
	{ // update bubble movement
		if ( bubbleframe == 8 )
			bubbleactive = FALSE;
		else
		{
			bubbleaction++;
			bubbley--;
			if ( bubbleframe >= 5 )
				bubbleframe++;
			else
				if ( ( bubbleaction & 0x3 ) == 0 ) if ( bubbleframe != 5 ) bubbleframe++;
		}
	} else
		if ( ( rand() & 0x3f ) == 0 )
		{ // create a new bubble
			action = rand();
			bubblex = x + 10 + rand() % 10;
			bubbley = y + 10 + rand() % 10;
			bubbleactive = TRUE;
			bubbleframe = 0;
			bubbleaction = rand();
		}
}

SLIME::SLIME( fixed cx, fixed cy, int o )
{
	reset_data();
	attribute.scenery = TRUE;
	frame = 0;
	x = cx; y = cy;
	id = o;
	fade = layertranslucentshade[ world->getlayer() ];
}

/*void SLIME::display( int xadj )
{
	image->put( x - xadj, y, id, frame, fade );
}*/

SWINGINGVINE::SWINGINGVINE( fixed cx, fixed cy, int o )
{
	reset_data();
	attribute.scenery = TRUE;
	attribute.dynamic = TRUE;
	frame = 2;
	x = cx; y = cy;
	id = o;
}

void SWINGINGVINE::go()
{
	subaction++;
	if ( ( subaction & 1 ) )
	{
		action++;
		if ( action == 8 )
		{
			action = 0;
			frame = 2;
		} else
			if ( action < 3 ) frame++;
			else
				if ( action < 7 ) frame--;
				else
					if ( action < 8 ) frame++;
	}
}
