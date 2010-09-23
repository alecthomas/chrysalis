#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <windows/region.h>
#include <windows/window.h>
#include <windows/button.h>
#include <cc/colour.h>
#include <cc/page.h>

class Window : public BufferedRegion {
	public :
		Window( Region *parent, int left, int top, int width, int height );
		Window( Region *parent, int left, int top, int width, int height, char *title );
		Window();
		virtual unsigned init( Region *parent, int left, int top, int width, int height );

		virtual bool redraw();
		virtual bool partialRedraw( int x, int y, int w, int h );
		virtual bool mousePress( unsigned button, int x, int y );
		virtual bool mouseRelease( unsigned button, int x, int y );
		virtual bool mouseMove( int oldX, int oldY, int x, int y );
		virtual bool focus();
		virtual bool unFocus();

		static Colour face, border, titleText, widget, inFocus;
		inline char *getTitle() { return title; }
		struct {
			bool
				horizontalScroll,
				verticalScroll,
				closeButton,
				resizeButton;
		} flags;
		Button *titleBar, *closeButton, *explodeButton;
	protected :
		char *title;
		struct {
		} action;
		class CloseButton : public ImageButton {
			public :
				CloseButton() {}
				CloseButton( Window *window, int x, int y, int w, int h );
				~CloseButton();
				
				virtual void triggered();
			protected :
				static char imageData[];
		};
		class TitleBar : public TextButton {
			public :
				TitleBar() {}
				TitleBar( Window *window, int x, int y, int w, int h );

				virtual bool mousePress( unsigned button, int x, int y );
				virtual bool mouseRelease( unsigned button, int x, int y );
				virtual bool mouseMove( int oldX, int oldY, int x, int y );
				virtual bool redraw();

				virtual void triggered();
			protected :
				Window *attachedTo;
				int moveDisplaceX, moveDisplaceY;
		};
};
#endif
