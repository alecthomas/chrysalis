#ifndef __EVENT_H__
#define __EVENT_H__

#include "queue.h"

class OBJECT;
class CREATURE;

class EVENT {
	public :
		// information about data types...for the editor
		typedef struct {
			char *name;
			int type;
		} EVENTINFO;
		enum {
			DT_NONE = 0,
			DT_LINE = 1, 
			DT_POINT = 2, 
			DT_CHAR = 4, 
			DT_OBJECT = 8,
			DT_NUMBER = 16,
			DT_NUMBERS = 32,
			DT_VECTOR = 64
		};
		enum {
			TT_REGION,
			TT_PROXIMITY
		};
		EVENT( int x, int y, int w, int h, int eventindex );
		EVENT( int x, int y, int radius, int eventindex );
		~EVENT();
		int active( CREATURE *creature );
		int touching( int x, int y );
		inline void set( char *message ) { messagedata = message; }
		inline void set( fixed x ) { xdata = x; }
		inline void set( fixed x, fixed y ) { xdata = x; ydata = y; }
		inline void set( QUEUE <OBJECT> *objectlist ) { objectlistdata = objectlist; }
		void display( int xadj, char colour );
		void load( FILE *io );
		void save( FILE *io );

		// queries
		static inline int getmaxevents() { return eventcount; }
		static inline char *geteventname( int index ) { return eventinfo[ index ].name; }
		static inline int geteventtype( int index ) { return eventinfo[ index ].type; }
		inline char *getname() { return eventinfo[ eventtype ].name; }
		inline int gettype() { return eventinfo[ eventtype ].type; }
		inline fixed geteventx() { return xdata; }
		inline fixed geteventy() { return ydata; }
		inline char *geteventmessage() { return messagedata; }
		inline QUEUE <OBJECT> *geteventobjectlist() { return objectlistdata; }
		inline int gettriggerx() { return x; }
		inline int gettriggery() { return y; }
		inline int gettriggertype() { return triggertype; }
	private :
		void reset_data();

		// data for event
		QUEUE <OBJECT> *objectlistdata;
		fixed xdata, ydata;
		char *messagedata;

		// what data does the event need? ie. vector? line? etc.
		int eventtype, triggertype, eventactive;

		int x, y, 
			w, h,			// for area triggers
			proximityradius,// for proximity triggers
			radius;

		static EVENTINFO eventinfo[];
		static int eventcount;
};

#endif
