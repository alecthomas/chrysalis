#ifndef __MONSTERS_H__
#define __MONSTERS_H__

class TRIFFID;

#include "creature.h"
#include "data.h"

class TRIFFIDSPORE;

class TRIFFID : virtual public CREATURE {
	friend class TRIFFIDSPORE;
		int sporex;
	public :
		TRIFFID( fixed cx, fixed cy, int o );
		void reset();
		void go();
		void display( int xadj );
		void performaction( ACTION *a );
};

class TRIFFIDSPORE : virtual public CREATURE {
	public :
		TRIFFIDSPORE( fixed cx, fixed cy, TRIFFID *parent );
		void reset();
		void go();
};

class SLIMER : virtual public CREATURE {
	public :
		SLIMER( fixed x, fixed y, int o );
		void reset();
		void go();
		void performaction( ACTION *a );
};

class WINGEDMONKEY : virtual public CREATURE {
	public :
		WINGEDMONKEY( fixed x, fixed y, int o );
		void reset();
		void go();
		void performaction( ACTION *a );
};
#endif
