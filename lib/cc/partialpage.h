#ifndef __PARTIALPAGE_H__
#define __PARTIALPAGE_H__

#include <cc/page.h>

class PartialPage : virtual public Page {
	public :
		PartialPage( Page *page, int x, int y, int w, int h );
		~PartialPage();

		virtual char *read( int row );
		virtual char *read( int column, int row, int width );
		virtual void write( char *source, int row );
		virtual void write( char *source, int column, int row, int width );
		virtual void blit( char *source, int column, int row, int width, unsigned mode );
		virtual void clear( Colour const &colour );

		virtual void filledBox( int column, int row, int width, int height, Colour colour );
		virtual void putPixel( int column, int row, Colour colour );

		virtual void setSingle( Colour colour );

	protected :
		Page *parent;
		int left, top;
};
#endif
