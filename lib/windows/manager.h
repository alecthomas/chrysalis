#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <windows/region.h>
#include <cc/video.h>
#include <cc/mouse.h>
#include <cc/keyboard.h>
#include <cc/image.h>
#include <cc/font.h>
#include <cc/colour.h>

void __ManagerCleanUp__();

class Window;

class Manager : public Region {
	friend void __ManagerCleanUp__();
	public :
		Manager( Video *video );
		~Manager();

		void showMouse();
		void hideMouse();
		int go();
		void halt( int retval, char *reason );
		bool removeRegion( Region *region );

/*		bool sendMessage( MessageType type );
		bool sendMessage( MessageType type, int a );
		bool sendMessage( MessageType type, int a, int b );
		bool sendMessage( MessageType type, int a, int b, int c );
		bool sendMessage( MessageType type, int a, int b, int c, int d );*/

		bool redraw();
		bool display();
		bool partialDisplay( int x, int y, int w, int h );

		bool lockFocus( Region *region );
		bool unlockFocus( Region *region );

		void generateVisibilityMap();

		bool setRoot( Region *region );

		static void setBuffered( bool buffer ) { buffered = buffer; }

		struct {
			bool
				autoRaise;
		} flags;
		Colour colour;
		Page *buffer;
		Video *video;
	protected :
		Region *root;
		void reFocus();
		QUEUE <Region> *removedRegions;
		class ScanLine {
			public :
				ScanLine();
				~ScanLine();
				void insert( int x, int w );
				void invert( int width );
				class Span {
					public :
						Span( int _x, int _w ) { x = _x; w = _w; }
						int x, w;
				};
				QUEUE <Span> *spans;
		};
		ScanLine **scanLines;
		int quit, hideDepth;
		char *quitReason;
		static char mouseCursor[], systemFont[];
		Region *focused, *lockedFocus;
		static Mouse *mouse;
		static Keyboard *keyboard;
		Image *cursor, *cursorBack, **fontImage;
		static bool buffered;
};
#endif
