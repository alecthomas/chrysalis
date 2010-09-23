/*	Morph class
	===========
	Morphs between two images

	Author: Alec Thomas
	Date: Wed Jun 14th, '95
	Comments: It's cool ;)
*/
#ifndef __MORPH_H__
#define __MORPH_H__

class MORPH {
	public :
		MORPH( char *source, char *target );
		~MORPH();
		int go();
		char *image() { return workimage; }
		int getw() { return w; }
		int geth() { return h; }
		typedef struct {
			int x, y,
				colour;
		} PIXEL;
	private :
		char *workimage;
		int step, w, h, size, sourcepixels, targetpixels, pixels;
		char **ytable;
		PIXEL *spoint, *tpoint, *point, *vector;
};

#endif
