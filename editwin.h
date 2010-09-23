#ifndef __EDITWIN_H__
#define __EDITWIN_H__

#include "object.h"

#define ATTRIB_display_tick_box( _y, t, n )\
	V_print( x + 2, y + _y, n, SINGLE( 250 ) );\
	V_box( x + 100, y + _y, 8, 8, 255 );\
	if ( activeobject )\
		if ( activeobject->attribute.t ) V_fbox( x + 102, y + _y + 2, 4, 4, 255 );\
	if ( activeobject == NULL )\
		if ( attribute.t ) V_fbox( x + 102, y + _y + 2, 4, 4, 255 );

#define MAS_set_attribute( t ) \
	if ( button == 1 )\
	{\
		if ( !userset.t ) \
			userreset.t = userset.t;\
		userset.t = !userset.t;\
	} else\
	{\
		if ( !userreset.t ) \
			userset.t = userreset.t;\
		userreset.t = !userreset.t;\
	}

// this is used down the bottom for setting attributes in ATTRIB window
#define ATTRIB_set_tick_box( t )\
	if ( activeobject != NULL )\
		activeobject->attribute.t = !activeobject->attribute.t;\
	else\
		attribute.t = !attribute.t;

#define MAS_display_tick_box( _y, t, n ) \
	V_print( x + 2, y + _y, n, SINGLE( 250 ) ); \
	V_box( x + 80, y + _y, 8, 8, 255 ); \
	if ( userreset.t ) c = 28; else \
		if ( userset.t ) c = 255; else \
			if ( set.t ) c = 248; else c = 0; \
	if ( c ) V_fbox( x + 82, y + _y + 2, 4, 4, c );

class WINDOW {
	protected :
		int bar, close, background;
	public :
		static int border;
		int x, y, w, h;
		char *title;
		WINDOW() { bar = close = TRUE; background = FALSE; }
		virtual void display( int top )
		{
			if ( bar )
			{
				V_fbox( x, y - 7, w, 7, 215 + ( top << 6 ) );
				V_fbox( x + 4, y + 1 - 7, w - 8, 1, 218 + ( top << 6 ) );
				V_fbox( x + 4, y + 3 - 7, w - 8, 1, 218 + ( top << 6 ) );
				V_fbox( x + 4, y + 5 - 7, w - 8, 1, 218 + ( top << 6 ) );
				V_box( x, y - 7, w, 7, 214 + ( top << 6 ) );
				V_box( x, y - 7, w, 1, 216 + ( top << 6 ) );
				V_box( x, y - 7, 1, 7, 216 + ( top << 6 ) );
				if ( close )
				{
					V_fbox( x + 1, y + 1 - 7, 6, 6, 215 + ( top << 6 ) );
					V_fbox( x + 2, y + 1 - 7, 4, 5, 218 + ( top << 6 ) );
				}
			}
			if ( border )
				V_box( x, y, w, h, 218 + ( top << 6 ) );
		}
		int touching( int x, int y )
		{
			if ( bar )
			{
				if ( close )
					if ( ::touching( x, y, WINDOW::x + 2, WINDOW::y + 1 - 7, 4, 5 ) )
						return 3;
				if ( ::touching( x, y, WINDOW::x, WINDOW::y - 7, w, 8 ) )
					return 2;
			}
			if ( background ) return FALSE;
			return ::touching( x, y, WINDOW::x, WINDOW::y, w, h );
		}
		virtual void press( int x, int y, int button ) {};
};

// Classes
class ICONS : public WINDOW {
		int iconindex[ 10 ][ 2 ];
	public :
		ICONS();
		void display( int top );
		void press( int x, int y, int button );
};

class OBJECTVIEWER : public WINDOW {
	public :
		OBJECTVIEWER();
		void display( int top );
		void press( int x, int y, int button );
};

class SCROLLBAR : public WINDOW {
		int barpos, barwidth;
	public :
		SCROLLBAR();
		void display( int top );
		void press( int x, int y, int button );
};

class INFO : public WINDOW {
	public :
		INFO();
		void display( int top );
		void press( int x, int y, int button );
};

class WINDOWLIST : public WINDOW {
	public :
		WINDOWLIST();
		void display( int top );
		void press( int x, int y, int button );
};

class HELP : public WINDOW {
	public :
		HELP();
		void display( int top );
		void press( int x, int y, int button );
};

class LOAD : public WINDOW {
	public :
		LOAD();
		void display( int top );
		void press( int x, int y, int button );
};

class SAVE : public WINDOW {
	public :
		SAVE();
		void display( int top );
		void press( int x, int y, int button );
};

class EVENTLIST : public WINDOW {
		int displaymode, entrypos, entry, finished;
		char entryA[ 128 ], entryB[ 128 ];
	public :
		EVENTLIST();
		void display( int top );
		void press( int x, int y, int button );
};

class EXIT : public WINDOW {
	public :
		EXIT();
		void display( int top );
		void press( int x, int y, int button );
};

class MARKEDATTRIBSET : public WINDOW {
		OBJECT::ATTRIBUTE set, userset, userreset;
		int fade;
	public :
		MARKEDATTRIBSET();
		void display( int top );
		void press( int x, int y, int button );
};

class ATTRIB : public WINDOW {
	public :
		ATTRIB();
		void display( int top );
		void press( int x, int y, int button );
};

class OBJECTLIST : public WINDOW {
		int onindex, onx, ony;
	public :
		OBJECTLIST();
		void display( int top );
		void press( int x, int y, int button );
};

#endif
