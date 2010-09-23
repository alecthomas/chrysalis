#include <string.h>
#include <iostream.h>
#include <cc/font.h>
#include <cc/video.h>
#ifdef __SVGA__
#include <vga.h>
#include <vgagl.h>
#endif
#ifdef __X11__
#include <mitshm.h>
#include <stdlib.h>
#endif

char Video::readBuffer[ 1024 ], *Video::actual = NULL;
int Video::palette[ 768 ] = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 63, 63, 63, 63, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	6, 0, 0, 11, 0, 0, 17, 0, 0, 23, 0, 0, 28, 0, 0, 34,
	0, 0, 40, 0, 0, 46, 0, 0, 51, 0, 0, 57, 0, 0, 63, 0,
	0, 63, 10, 10, 63, 22, 22, 63, 33, 33, 63, 45, 45, 63, 57, 57,
	0, 6, 0, 0, 11, 0, 0, 17, 0, 0, 23, 0, 0, 28, 0, 0,
	34, 0, 0, 40, 0, 0, 46, 0, 0, 51, 0, 0, 57, 0, 0, 63,
	0, 10, 63, 10, 22, 63, 22, 33, 63, 33, 45, 63, 45, 57, 63, 57,
	0, 0, 6, 0, 0, 11, 0, 0, 17, 0, 0, 23, 0, 0, 28, 0,
	0, 34, 0, 0, 40, 0, 0, 46, 0, 0, 51, 0, 0, 57, 0, 0,
	63, 10, 10, 63, 22, 22, 63, 33, 33, 63, 45, 45, 63, 57, 57, 63,
	6, 6, 0, 11, 11, 0, 17, 17, 0, 23, 23, 0, 28, 28, 0, 34,
	34, 0, 40, 40, 0, 46, 46, 0, 51, 51, 0, 57, 57, 0, 63, 63,
	0, 63, 63, 10, 63, 63, 22, 63, 63, 33, 63, 63, 45, 63, 63, 57,
	0, 0, 0, 0, 11, 11, 0, 17, 17, 0, 23, 23, 0, 28, 28, 0,
	34, 34, 0, 40, 40, 0, 46, 46, 0, 51, 51, 0, 57, 57, 0, 63,
	63, 10, 63, 63, 22, 63, 63, 33, 63, 63, 45, 63, 63, 57, 63, 63,
	8, 3, 1, 10, 4, 1, 12, 5, 2, 15, 7, 3, 17, 8, 4, 20,
	10, 5, 22, 12, 6, 24, 14, 7, 27, 16, 9, 29, 18, 10, 32, 20,
	12, 38, 26, 19, 43, 33, 26, 49, 40, 33, 54, 48, 42, 60, 56, 52,
	1, 3, 1, 2, 6, 2, 4, 10, 5, 6, 13, 7, 8, 17, 9, 10,
	20, 11, 11, 24, 14, 13, 27, 16, 15, 31, 19, 17, 34, 22, 19, 38,
	25, 25, 42, 31, 32, 47, 37, 39, 52, 44, 48, 57, 51, 57, 62, 59,
	4, 2, 0, 8, 4, 0, 12, 7, 0, 17, 10, 1, 21, 12, 2, 25,
	15, 3, 29, 18, 5, 34, 21, 7, 38, 24, 9, 42, 27, 12, 47, 31,
	15, 50, 37, 22, 53, 43, 29, 56, 49, 38, 59, 55, 47, 63, 61, 57,
	3, 4, 0, 6, 8, 0, 10, 13, 0, 14, 17, 0, 18, 22, 1, 22,
	27, 2, 25, 31, 3, 30, 36, 5, 33, 40, 7, 38, 45, 9, 42, 50,
	11, 45, 52, 18, 48, 54, 25, 51, 56, 33, 54, 58, 42, 58, 60, 51,
	3, 4, 4, 6, 8, 8, 10, 13, 13, 14, 18, 18, 17, 22, 22, 21,
	27, 27, 25, 32, 32, 28, 37, 37, 32, 41, 41, 35, 46, 46, 39, 51,
	51, 42, 52, 52, 45, 54, 54, 49, 56, 56, 53, 58, 58, 57, 60, 60,
	4, 4, 6, 8, 8, 11, 12, 12, 17, 16, 16, 23, 21, 21, 28, 26,
	26, 34, 30, 30, 40, 35, 35, 46, 40, 40, 51, 45, 45, 57, 50, 50,
	63, 52, 52, 63, 54, 54, 63, 57, 57, 63, 59, 59, 63, 62, 62, 63,
	6, 5, 2, 11, 10, 4, 17, 14, 7, 23, 19, 9, 28, 24, 12, 34,
	29, 14, 40, 34, 17, 46, 38, 19, 51, 43, 22, 57, 48, 24, 63, 53,
	27, 63, 56, 33, 63, 59, 40, 63, 61, 46, 63, 62, 53, 63, 63, 60,
	1, 2, 4, 2, 4, 8, 4, 7, 12, 6, 9, 16, 7, 12, 20, 9,
	15, 24, 11, 18, 28, 12, 21, 32, 14, 24, 36, 16, 27, 41, 18, 31,
	45, 24, 36, 48, 31, 41, 52, 39, 47, 55, 48, 53, 59, 57, 60, 63,
	6, 4, 0, 11, 7, 0, 17, 10, 0, 23, 13, 0, 28, 15, 1, 34,
	17, 1, 40, 18, 2, 46, 19, 4, 51, 20, 5, 57, 21, 7, 63, 21,
	9, 63, 29, 18, 63, 37, 28, 63, 45, 37, 63, 52, 47, 63, 59, 57,
	0, 0, 0, 3, 3, 3, 7, 7, 7, 12, 12, 12, 16, 16, 16, 20,
	20, 20, 24, 24, 24, 29, 29, 29, 33, 33, 33, 37, 37, 37, 41, 41,
	41, 46, 46, 46, 50, 50, 50, 54, 54, 54, 58, 58, 58, 63, 63, 63 };

#ifdef __SVGA__
Video::Mode Video::modeList[] = {
	G320x200x256, "320x200",
	G640x480x256, "640x480",
	G800x600x256, "800x600",
	G1024x768x256, "1024x768",

	G1280x1024x256, "1280x1024",
	-1, ""
};
#endif

Video::Video()
/* ============
	Default to 320x200x256
*/
{
int l;
#ifdef __SVGA__
	vga_disabledriverreport();
	vga_setmode( G320x200x256 );
	mode = G320x200x256;
	width = vga_getxdim();
	height = vga_getydim();
	gl_setcontextvga( G320x200x256 );
	actual = vga_getgraphmem();
#endif
#ifdef __X11__
	width = 320; height = 200;
	Xinitialize( width, height );
	buffer = actual = shared_mem;
	yTable = new char*[ height ];
	for ( l = 0; l != height; l++ ) yTable[ l ] = actual + l * width;
#endif
	for ( l = 0; l != 768; l += 3 ) setPalette( l / 3, palette[ l ], palette[ l + 1 ], palette[ l + 2 ] );
	return;
}

Video::Video( char *modename )
/* ========================
	Initialise a video mode
*/
{
#ifdef __SVGA__
	vga_disabledriverreport();
	if ( modename == NULL )
	{
		cerr << "can't set a NULL mode!?!?" << endl;
		exit( 1 );
	}
	for ( mode = 0; modeList[ mode ].mode != -1; mode++ )
		if ( !strcmp( modeList[ mode ].name, modename ) )
		{
			if ( !vga_hasmode( modeList[ mode ].mode ) )
			{
				cerr << "mode " << modename << " not supported by video card " << endl;
				exit( 1 );
			}
			vga_setmode( modeList[ mode ].mode );
			width = vga_getxdim();
			height = vga_getydim();
			for ( int l = 0; l != 768; l += 3 ) setPalette( l / 3, palette[ l ], palette[ l + 1 ], palette[ l + 2 ] );
			gl_setcontextvga( modeList[ mode ].mode );
			actual = vga_getgraphmem();
			buffer = NULL; yTable = NULL;
			return;
		}
	if ( modeList[ mode ].mode == -1 )
	{
		cerr << "unknown mode " << modename << endl;
		exit( 1 );
	}
#endif
#ifdef __X11__
int l;
	if ( !strchr( modename, 'x' ) )
	{
		cerr << "unknown mode " << modename << endl;
		exit( 1 );
	}
	width = atoi( modename );
	height = atoi( strchr( modename, 'x' ) + 1 );
	if ( width <= 0 || height <= 0 )
	{
		cerr << "unknown mode " << modename << endl;
		exit( 1 );
	}
	Xinitialize( width, height );
	buffer = actual = shared_mem;
	yTable = new char*[ height ];
	for ( l = 0; l != height; l++ ) yTable[ l ] = actual + l * width;
	for ( l = 0; l != 768; l += 3 ) setPalette( l / 3, palette[ l ], palette[ l + 1 ], palette[ l + 2 ] );
#endif
}

Video::~Video()
{
#ifdef __X11__
	Xuninitialize();
	buffer = NULL;
#endif
}

void Video::setPalette( int index, int r, int g, int b )
/* ======================================================
	Set a palette entry colour.
*/
{
	palette[ index * 3 ] = r;
	palette[ index * 3 + 1 ] = g;
	palette[ index * 3 + 2 ] = b;
#ifdef __SVGA__
	vga_setpalette( index, r, g, b );
#endif
#ifdef __X11__
	setColor( index, r * 4, g * 4, b * 4 );
#endif
}

void Video::waitRetrace()
/* =======================
	Wait for a vertical retrace
*/
{
#ifdef __SVGA__
	vga_waitretrace();
#endif
}

#ifdef __SVGA__
void Video::write( char *source, int row )
/* =======================================
	Blit to video memory
*/
{
	vga_drawscanline( row, (unsigned char*)source );
}

void Video::write( char *source, int column, int row, int width )
/* ===================================================
	Blit a section to video memory
*/
{
	vga_drawscansegment( (unsigned char*)source, column, row, width );
}

void Video::blit( char *source, int column, int row, int width, unsigned mode )
/* =============================================================================
	Special mode transfer to video memory
*/
{
int l;

	switch ( mode )
	{
		case MOV :
			vga_drawscansegment( (unsigned char*)source, column, row, width );
		break;
		case MASK :
			for ( l = 0; l != width; l++ )
				if ( source[ l ] )
				{
					vga_setcolor( source[ l ] );
					vga_drawpixel( column + l, row );
				}
		break;
		case SINGLE :
			vga_setcolor( singleColour.actual );
			for ( l = 0; l != width; l++ )
				if ( source[ l ] ) vga_drawpixel( column + l, row );
		break;
	}
}

char *Video::read( int row )
{
	if ( mode == 0 ) return actual + row * 320;
	for ( int l = 0; l != width; l++ ) readBuffer[ l ] = gl_getpixel( l, row );
	return readBuffer;
}

char *Video::read( int column, int row, int width )
{
	if ( mode == 0 ) return actual + row * 320 + column;
	for ( int l = 0; l != width; l++ ) readBuffer[ l ] = gl_getpixel( column + l, row );
	return readBuffer;
}

void Video::clear( Colour const &colour )
{
}

void Video::filledBox( int x, int y, int w, int h, Colour colour )
{
char *i;

	if ( !clip( i, x, y, w, h, width, height ) ) return;
	gl_fillbox( x, y, w, h, colour.actual );
}

void Video::box( int column, int row, int width, int height, Colour colour )
{
	gl_line( column, row, column + width - 1, row, colour.actual );
	gl_line( column, row, column, row + height - 1, colour.actual );
	gl_line( column + width - 1, row, column + width - 1, row + height - 1, colour.actual );
	gl_line( column, row + height - 1, column + width - 1, row + height - 1, colour.actual );
}

void Video::line( int column, int row, int ecolumn, int erow, Colour colour )
{
	gl_line( column, row, ecolumn, erow, colour.actual );
}

void Video::circle( int column, int row, int radius, Colour colour )
{
	gl_circle( column, row, radius, colour.actual );
}

void Video::putPixel( int column, int row, Colour colour )
{
	gl_setpixel( column, row, colour.actual );
}

void Video::sync()
{
}

void Video::syncPartial( int x, int y, int w, int h )
{
}
#endif

#ifdef __X11__
void Video::sync()
{
	updateScreen();
}

void Video::syncPartial( int x, int y, int w, int h )
{
	updateScreenClipped( x, y, w, h );
}
#endif
