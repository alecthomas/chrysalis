#ifndef __SCROLLBAR_H__
#define __SCROLLBAR_H__

#include <windows/region.h>

class Button;

class ScrollBar : public Region {
	public :
		ScrollBar( Region *parent, int x, int y, int w, int h, bool horizontal );
		~ScrollBar();
		unsigned init( Region *parent, int x, int y, int w, int h );
		void setMax( int max ) { maximum = max; }
		void setCurrent( int cur ) { current = cur; }

		virtual bool display();
		Colour colour;
	protected :
		static char upArrow[], leftArrow[], rightArrow[], downArrow[];
		int maximum, current;
		Button *back, *forward, *slide;
		Image *left, *right, *up, *down;
		bool horizontal;
};
#endif
