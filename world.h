/*****************************************************************************
    Controls all queues and movements of the layers

  Author: Alec Thomas, Eric Sambach
  Update: Nov '94
  Comments:
*****************************************************************************/

#ifndef __WORLD_H__
#define __WORLD_H__

class WORLD;
class PLAYER;
class CREATURE;

// Global definitions
extern WORLD *world;

#include <stdio.h>
#include "queue.h"
#include "lib.h"
#include "layer.h"

#define WORLDWIDTH 			320	// The width of the world window
#define	WORLDHEIGHT 		200 // The height of the world window

#define	WORLDSCREENX 		(160 - WORLDWIDTH / 2)
#define	WORLDSCREENY 		0

#define	WORLDSCREENOFFSET	( WORLDSCREENX + WORLDSCREENY * 320 )

class EVENT;
class OBJECT;
class IMAGE;

class WORLD {
	friend class LAYER;
		int nameindex( char *name );
		char *namelist[ MAXIMAGE ];
		int nameindices;
		LAYER *layerlist[ 4 ];
		VPAGESTRUCT *page;
		LIB *lib;
		IMAGE *image;
		int layerindex, level, startpos;
	public :
		static int firstlevel, layerswitch[ 4 ];
		LAYER *layer, *playlayer;
		PLAYER *actualplayer;
		CREATURE *player;
		WORLD();
		~WORLD();
		OBJECT *insertobject( int x, int y, int p, char *n ) { return layer->insertobject( x, y, p, n ); }
		OBJECT *insertobject( int x, int y, int p, int n ) { return layer->insertobject( x, y, p, n ); }
		OBJECT *insertobject( int p, char *o, FILE *io ) { return layer->insertobject( p, o, io ); }
		OBJECT *insertobject( int p, OBJECT *o ) { return layer->insertobject( p, o ); }
		CREATURE *inserttemporarycreature( int p, CREATURE *c ) { return layer->inserttemporarycreature( p, c ); }
		SCENERY *inserttemporaryscenery( int p, SCENERY *s ) { return layer->inserttemporaryscenery( p, s ); }
		
		void load( int level );
		
		int getposition() { return layer->worldposition; }
		int getlayer() { return layerindex; }

		LAYER *setlayer( int l )
		/*  ===================
			Set the active layer
		*/
		{
			return ( layer = layerlist[ ( layerindex = l ) ] );
		}
		
		void usepage()
		/*  ===========
			Set the current layers main page as the active page
		*/
		{
			layer->usepage();
		}
		
		void go();
		void display();
		void save( int level );

		void createplayer();

		QUEUE <EVENT> *event;

		OBJECT *getobjectfromname( char *n );
		CREATURE *getcreaturefromname( char *n );
};
#endif
