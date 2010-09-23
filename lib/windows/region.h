#ifndef __REGION_H__
#define __REGION_H__

#include <cc/page.h>
#include <cc/font.h>
#include <cc/colour.h>
#include <general.h>
#include <queue.h>

class Manager;

class Region {
	friend Manager;
	public :

		typedef enum {
			Focus,
			UnFocus,
			Run,
			KeyPress,
			KeyRelease,
			MousePosition,
			MouseMove,
			MousePress,
			MouseRelease,
			Redraw,
			PartialRedraw,
			Display,
			PartialDisplay,
			ReverseDisplay,
			ReversePartialDisplay,
			Close,
			Ping
		} MessageType;

		Region();
		Region( Region *parent, int x, int y, int w, int h ) { init( parent, x, y, w, h ); }
		virtual ~Region();
		virtual unsigned init( Region *parent, int x, int y, int w, int h );
		virtual void deInit();
		virtual bool sendMessage( MessageType type );
		virtual bool sendMessage( MessageType type, int a );
		virtual bool sendMessage( MessageType type, int a, int b );
		virtual bool sendMessage( MessageType type, int a, int b, int c );
		virtual bool sendMessage( MessageType type, int a, int b, int c, int d );
		virtual void dumpStats();
		virtual bool addRegion( Region *region );

		// Message handling methods
		virtual bool focus();
		virtual bool unFocus();
		virtual bool run();
		virtual bool keyPress( unsigned key );
		virtual bool keyRelease( unsigned key );
		virtual bool mousePosition( int x, int y );
		virtual bool mouseMove( int oldX, int oldY, int x, int y );
		virtual bool mousePress( unsigned button, int x, int y );
		virtual bool mouseRelease( unsigned button, int x, int y );
		virtual bool redraw();
		virtual bool partialRedraw( int x, int y, int w, int h );
		virtual bool display();
		virtual bool partialDisplay( int x, int y, int w, int h );
		virtual bool reverseDisplay();
		virtual bool reversePartialDisplay( int x, int y, int w, int h );
		virtual bool close();
		virtual bool ping( int x, int y );

		virtual void setManager( Manager *manager );

		// Region control methods
		virtual bool setPosition( int x, int y );

		// Region information methods 
		inline unsigned getId() { return id; }
		inline int getType() { return type; }
		inline char *getName() { return name; }
		inline int getX() { return left; }
		inline int getY() { return top; }
		int getAbsX();
		int getAbsY();
		inline int getWidth() { return width; }
		inline int getHeight() { return height; }
		inline Page *getPage() { return page; }
		Font font;
		Colour textColour;
	protected :
		Page *page;
		static unsigned idCount;
		int top, left, width, height, type;
		unsigned id;
		char *name;
		QUEUE <Region> *regions;
		Region *parent;
		Manager *manager;
};

// Just a region that actually allocates a buffer, and also handles Display and PartialDisplay messages
class BufferedRegion : public Region {
	public :
		BufferedRegion() {}
		BufferedRegion( Region *parent, int x, int y, int w, int h ) { init( parent, x, y, w, h ); }
		virtual ~BufferedRegion();
		virtual unsigned init( Region *parent, int x, int y, int w, int h );
		virtual bool display();
		virtual bool partialDisplay( int x, int y, int w, int h );
		virtual bool redraw();
		virtual bool partialRedraw( int x, int y, int w, int h );
		Colour colour;
	protected :
};
#endif
