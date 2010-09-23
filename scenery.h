/*****************************************************************************
  Control each item of scenery

  Author: Alec Thomas
  Update: 9th Dec '94
  Comments:
    This baby handles each item of scenery. Scenery events come into play
    when a piece of scenery is hit by either
      a) Chrysalis the ship or
      b) Chrysalis' firepower
*****************************************************************************/
#ifndef __SCENERY_H__
#define __SCENERY_H__

class SCENERY;

#include <stdio.h>
#include <string.h>
#include "object.h"

class SCENERY : virtual public OBJECT {
	public :
		SCENERY() {}
		SCENERY( fixed cx, fixed cy, int o )
		{
			reset_data();
			attribute.scenery = TRUE;
			x = cx;
			y = cy;
			xdir = ydir = 0;
			id = o;
		}
};

class WATER : virtual public SCENERY {
	public :
		WATER( fixed cx, fixed cy, int o );
		//void display( int xadj );
		void go();
		void setfade( int f ) { fade = TRANSLUCENTFADE( f ); }
};

class SOLIDWATER : virtual public SCENERY {
		fixed bubblex, bubbley;
		int bubbleid, bubbleframe, bubbleaction, bubbleactive;
	public :
		SOLIDWATER( fixed cx, fixed cy, int o );
		void display( int xadj );
		void go();
		void setfade( int f ) { fade = TRANSLUCENTFADE( f ); }
};

class SLIME : virtual public SCENERY {
	public :
		SLIME( fixed cx, fixed cy, int o );
		//void display( int xadj );
		void setfade( int f ) { fade = TRANSLUCENTFADE( f ); }
};

class SWINGINGVINE : virtual public SCENERY {
	public :
		SWINGINGVINE( fixed cx, fixed cy, int o );
		void go();
		void setfade( int f ) { fade = TRANSLUCENTFADE( f ); }
};

#endif
