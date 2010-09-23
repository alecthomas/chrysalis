/*****************************************************************************
  Control the attributes and movement of creatures

   Author: Alec Thomas, Eric Sambach
   Update: Nov '94
   Comments:
*****************************************************************************/

#ifndef __CREATURE_H__
#define __CREATURE_H__

#include "object.h"

/*
	Notes to myself
	===============
	- go() is NOT called when a creature is possessed, instead, performaction()
	  is called directly
*/

// actions to perform
typedef struct {
	int
		shot : 1,
		special : 1,
		left : 1,
		right : 1,
		up : 1,
		down : 1;
} ACTION;
		
class CREATURE : virtual public OBJECT {
	public :
		typedef struct {
			unsigned int
				invisible : 1,      // invisible
				intangible : 1,     // move through things
				hyper : 1,          // super-fast movement, firing, etc.
				speed : 4,				// speed of object (0-15)
				firerate : 4,			// frame delay between fires (0-15)
				offense : 7,			// offensive energy 0%-100% value
				defense : 7;			// defensive energy 0%-100% value
		} ABILITY;

		// creature data
		ABILITY ability;
		ACTION activity;

		// public functions
		CREATURE() {}
		CREATURE( fixed cx, fixed cy, int o ) { x = cx; y = cy; id = o; }
		~CREATURE() {}
		
		void load( FILE *f );
		void save( FILE *f );
		virtual void performaction( ACTION *a );
		void reset_data();				// initial reset of data to 0
		virtual void reset();			// set values up ready to go

		void display_icon( int x, int y, int mode ) { image->put( x, y, id, image->getentries( id ) - 1, mode ); }
};

#endif
