/*****************************************************************************
  Holds information about the player (of which there may be 2)

   Author: Alec and Eric
   Update: Nov '94
   Notes:
*****************************************************************************/

#ifndef __PLAYER_H__
#define __PLAYER_H__

class PLAYER;

#include "creature.h"
#include "morph.h"

class PLAYER : public CREATURE {
		CREATURE *shape[ 4 ], *selectedshape, *targetshape;
		char *shapename[ 4 ];
		int
			shapeid[ 4 ],
			actualid,
			selectedindex,				// selected shape index
			score,                      // players current score
			lives;                      // number of lives left
		char **imagelist;
		MORPH *morph;
	public:
		PLAYER( int x, int y, int o );
		~PLAYER();
		void go();
		void performaction( ACTION *p );
		void display( int xadj );
		void possess( CREATURE *o, int index );
		void changeshape( int index );
		void setshape( int index );
		void display_shapeicon( int x, int y, int index );
		fixed getx() { if ( action == 1 || selectedshape == this ) return x; return selectedshape->x; }
		fixed gety() { if ( action == 1 || selectedshape == this ) return y; return selectedshape->y; }
		int getw() { if ( action == 1 ) return morph->getw(); return image->getw( selectedshape->id, selectedshape->frame ); }
		int geth() { if ( action == 1 ) return morph->geth(); return image->geth( selectedshape->id, selectedshape->frame ); }
};

#endif
