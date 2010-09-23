#ifndef __RAISED_H__
#define __RAISED_H__

#include <windows/region.h>
#include <cc/colour.h>

class RaisedRegion : public Region {
	public :
		RaisedRegion() {}
		RaisedRegion( Region *parent, int x, int y, int w, int h );
		virtual ~RaisedRegion();
		virtual unsigned init( Region *parent, int x, int y, int w, int h );

		virtual bool redraw();
		virtual bool display();
		virtual bool partialDisplay( int x, int y, int w, int h );

		bool indented;
		Colour colour;
};
#endif
