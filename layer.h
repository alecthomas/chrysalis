#ifndef __LAYER_H__
#define __LAYER_H__

#include "object.h"

class WORLD;
class SCENERY;
class CREATURE;

class LAYER {
	public :
		int edgewidth, position, worldposition, objectwrap;
		QUEUE <OBJECT>
			*naturalQ,                       	// objects in natural order
			*creatureQ,							// creature objects (x ordered)
			*sceneryQ,						 	// scenery objects (x ordered)
			*Q;                               	// world objects (x ordered)
		VPAGESTRUCT *page;
		LAYER( int layer );
		~LAYER();
		void scroll( int dir );
		OBJECT *insertobject( int x, int y, int priority, char *objectname );
		OBJECT *insertobject( int x, int y, int priority, int n );
		OBJECT *insertobject( int priority, char *objectname, FILE *io );
		OBJECT *insertobject( int priority, OBJECT *object );
		CREATURE *inserttemporarycreature( int priority, CREATURE *creature );
		SCENERY *inserttemporaryscenery( int priority, SCENERY *scenery );
		OBJECT *checkscenerycollisions( OBJECT *creature );
		OBJECT *checkcreaturecollisions( OBJECT *creature );
		OBJECT *updatexcollision( OBJECT *o );
		OBJECT *updateycollision( OBJECT *o );
		void display();
		void displayall();
		void displayobjects();
		void updateobjects();
		void reset();
		void save( FILE *io );
		int load( FILE *io );
		void usepage();
		void go();
};
#endif
