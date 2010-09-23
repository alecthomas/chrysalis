#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <windows/region.h>

class Button : public Region {
	public :
		Button() {}
		Button( Region *parent, int x, int y, int w, int h );
		~Button();
		virtual unsigned init( Region *parent, int x, int y, int w, int h );
		virtual void deInit();

		virtual void triggered();

		virtual bool mousePress( unsigned button, int x, int y );
		virtual bool mouseRelease( unsigned button, int x, int y );
		virtual bool mouseMove( int oldX, int oldY, int x, int y );
		virtual bool redraw();
		virtual bool display();
		virtual bool partialDisplay( int x, int y, int w, int h );

		Colour colour;
	protected :
		struct {
			bool 
				drawPressed,
				pressed;
		} action;
};

class TextButton : public Button {
	public :
		TextButton() {}
		TextButton( Region *parent, int x, int y, char *text );
		virtual unsigned init( Region *parent, int x, int y, int w, int h );

		virtual bool redraw();

		Colour textColour;
	protected :
		char *text;
};

class Image;

class ImageButton : public Button {
	public :
		ImageButton() {}
		ImageButton( Region *parent, int x, int y, Image *image );

		virtual bool redraw();

	protected :
		Image *image;
};
#endif
