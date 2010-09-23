/*****************************************************************************
  Base class for objects, inherited by just about everything ;)

  Author: Alec Thomas
  Updates: Dec 15 '94
  Comments:
*****************************************************************************/
#ifndef __OBJECT_H__
#define __OBJECT_H__

class OBJECT;
class CREATURE;
class SCENERY;
class WORLD;
class LAYER;

extern int layershade[ 4 ];
extern int layertranslucentshade[ 4 ];

#include <stdio.h>
#include "image.h"
#include "fixed.h"

class OBJECT {
	public :
		typedef struct {
			int
				scenery : 1,			// is scenery
				dynamic : 1,			// will change (animate/move)
				possessable : 1,		// can be possessed
				alive : 1,				// is alive
				solid : 1,				// is solid
				possessed : 1,			// has been possessed
				sceneryghost : 1,		// can pass through scenery
				creatureghost : 1,		// can pass through creatures
				temporary : 1,			// deleted when dead
				dying : 1,				// dying
				canstick : 1;			// get stuck in scenery?
		} ATTRIBUTE;
		int
			fade,				// fade value for object
			count,				// counter ;) I'll use it for something
			priority,			// priority of object
			damage,				// amount of damage an object does
			health,				// amount of health an object has
			id,					// object type, eg. creature, scenery, etc.
			frame,				// frame index to use to display
			action,				// die, move, etc.
			subaction;			// die:1, die:2, die:3, etc.
		fixed
			x, y,				// position
			xdir, ydir,			// movement
			xdisp, ydisp;		// displacement
		ATTRIBUTE attribute;	// flags of object
		OBJECT
			*scenerycollision,			// scenery collision any direction
			*creaturecollision;			// creature collision (or NULL)

// object functions (totally public)
		OBJECT();
		OBJECT( fixed x, fixed y, int speed );
		virtual ~OBJECT();
		inline char *getimage() { return image->get_data( id, frame ); }
		virtual void go();
		virtual void reset_data();
		virtual void setposition( fixed _x, fixed _y );
		virtual fixed getx();
		virtual fixed gety();
		virtual int getw();
		virtual int geth();
		virtual int touching( fixed x, fixed y );
		virtual int checkcollision( OBJECT *o );
		inline void resetcollision() { creaturecollision = scenerycollision = NULL; }
		virtual void setfade( int f );
		inline int getfade() { return ( fade & 0xff ) - 15; }
		virtual void display( int xadj );
		virtual void save( FILE *f );
		virtual void load( FILE *f );
};

#endif
