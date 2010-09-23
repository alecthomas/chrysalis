#ifdef __WATCOMC__
#include <i86.h>
#include <malloc.h>
#endif

#ifdef __X11__
#include "mitshm.h"
#endif

#ifdef __SVGA__
#include <vga.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "graphics.h"
#include "general.h"

int
	G_screenwidth = 320,
	G_screenheight = 200,
	G_cursorposition[ 16 ];

char
	*G_oldscreen,
	G_oldmode = 0,
	**font,
	*G_screen,
	**G_ytable,
	fadepalette[ 768 ],
	palette[ 768 ] = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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

void G_deinit( void )
/* ===================
	De-initialise graphics mode
*/
{
	if ( !G_oldmode ) return;
#ifdef __WATCOMC__
	REGPACK r;
	memset( &r, 0, sizeof( REGPACK ) );
	r.h.ah = 0x0f;
	intr( 0x10, &r );
	if ( r.h.al == G_oldmode ) return;
	r.x.eax = G_oldmode;
	intr( 0x10, &r );
	memcpy( (char*)0xb0000, G_oldscreen, 65536 );
	memcpy( (char*)0x450, G_cursorposition, 16 );
#endif
#ifdef __X11__
	Xuninitialize();
#endif
#ifdef __SVGA__
	REPORT( "returning to text mode" );
#endif
	G_oldmode = 0;
}

	// Initialise graphics mode, setting palette, etc.
int G_init( void )
{
int l;

	if ( G_oldmode ) return TRUE;
	atexit( G_deinit );
	REPORT( "entering 320x200x256" );

	// Platform specific stuff
#ifdef __WATCOMC__
	REGPACK r;
	G_oldscreen = new char[ 65536 ];
	memcpy( G_oldscreen, (char*)0xb0000, 65536 );
	memcpy( G_cursorposition, (char*)0x450, 16 );
	G_screen = (char*)0xa0000;
	memset( &r, 0, sizeof( REGPACK ) );
	r.h.ah = 0x1a;
	r.h.al = 0;
	intr( 0x10, &r );
	if ( r.h.bl != 8 ) return FALSE;
	r.h.ah = 0x0f;
	intr( 0x10, &r );
	G_oldmode = r.h.al;
	if ( G_oldmode != 0x13 )
	{
		r.x.eax = 0x13;
		intr( 0x10, &r );
	}
#endif

#ifdef __X11__
	Xinitialize( 320, 200 );
	G_screen = shared_mem;
	G_screenwidth = 320;
	G_screenheight = 200;
	G_oldmode = 1;
#endif

#ifdef __SVGA__
	vga_disabledriverreport();
	if ( vga_hasmode( G320x200x256 ) == -1 ) return 0;
	vga_setmode( G320x200x256 );
	G_screen = (char*)vga_getgraphmem();
	G_oldmode = 1;
	G_screenwidth = vga_getxdim();
	G_screenheight = vga_getydim();
#endif

	G_setpalette( 0, 255, palette );
	G_ytable = (char**)malloc( sizeof( char* ) * 200 );
	for ( l = 0; l != 200; l++ ) G_ytable[ l ] = G_screen + l * 320;
	return TRUE;
}

int G_setmode( int mode )
/* =======================
	Change graphics mode, using SVGAlib
*/
{
	if ( !G_oldmode ) return FALSE;
#ifdef __SVGA__
	if ( vga_hasmode( mode ) == -1 ) return FALSE;
	vga_setmode( mode );
	G_screen = (char*)vga_getgraphmem();
	G_screenwidth = vga_getxdim();
	G_screenheight = vga_getydim();
	G_setpalette( 0, 255, palette );
	free( G_ytable );
	G_ytable = (char**)malloc( sizeof( char* ) * G_screenheight );
	for ( int l = 0; l != G_screenheight; l++ ) G_ytable[ l ] = G_screen + l * G_screenwidth;
	printf( "%ix%i\n", G_screenwidth, G_screenheight );
	return TRUE;
#endif
#if defined( __WATCOMC__ ) || defined( __X11__ )
	return FALSE;
#endif
}

// Loads an image, first two DWORDS are width and height of image
char *G_loadimage( FILE *handle )
{
unsigned int w, h;
char *buffer;

	if ( fread( (void*)&w, 1, sizeof( int ), handle ) != sizeof( int ) ) return NULL;
	if ( fread( (void*)&h, 1, sizeof( int ), handle ) != sizeof( int ) ) return NULL;
	if ( ( buffer = (char*)malloc( w * h + sizeof( int ) * 2 ) ) == NULL ) return NULL;
	((int*)buffer)[ 0 ] = w;
	((int*)buffer)[ 1 ] = h;
	if ( fread( (void*)(buffer + sizeof( int ) * 2), 1, w * h, handle ) != w * h )
	{
		free( buffer );
		return NULL;
	}
	return buffer;
}

void G_skipimage( FILE *handle )
{
int w, h;

	fread( (void*)&w, 1, sizeof( int ), handle );
	fread( (void*)&h, 1, sizeof( int ), handle );
	fseek( handle, w * h, SEEK_CUR );
}

// Loads an image, first two DWORDS are width and height of image
char *G_loadimage16( FILE *handle )
{
unsigned short w, h;
char *buffer;

	if ( fread( (void*)&w, 1, sizeof( short int ), handle ) != sizeof( short int ) ) return NULL;
	if ( fread( (void*)&h, 1, sizeof( short int ), handle ) != sizeof( short int ) ) return NULL;
	buffer = (char*)malloc( w * h + 8 );
	((int*)buffer)[ 0 ] = w;
	((int*)buffer)[ 1 ] = h;
	if ( fread( (void*)(buffer + 8), 1, w * h, handle ) != w * h )
	{
		free( buffer );
		return NULL;
	}
	return buffer;
}

// Saves an image (duh!)
void G_saveimage( char *image, FILE *handle )
{
	fwrite( image, 1, ((int*)image)[ 0 ] * ((int*)image)[ 1 ] + 8, handle );
}

int G_loadfont( char **font, FILE *handle )
{
	int l;

	for ( l = 32; l != 128; l++ )
		if ( ( font[ l ] = G_loadimage( handle ) ) == NULL ) return FALSE;
	return TRUE;
}

void G_setfont( char **f )
{
	font = f;
}

#ifdef __X11__
void G_setpalette( int start, int finish, char *pal )
{
	for ( int l = 0; l != ( finish - start ) + 1; l++ )
		setColor( start + l, 
			(int)pal[ ( l + start ) * 3 ] * 4, 
			(int)pal[ ( l + start ) * 3 + 1 ] * 4, 
			(int)pal[ ( l + start ) * 3 + 2 ] * 4 );
}

void G_getpalette( int start, int finish, char *pal )
{
}

// maybe i'll insert some sort of delay thing in here.....later
void G_waitretrace()
{
}
#endif

#ifdef __SVGA__
/* Kludges to convert stoopid linux vgalib to standard pc palette format */
void G_setpalette( int start, int finish, char *pal )
{
int l;

	for ( l = 0; l != ( finish - start ) + 1; l++ )
		vga_setpalette( start + l, pal[ l * 3 ], pal[ l * 3 + 1 ], pal[ l * 3 + 2 ] );
}

void G_getpalette( int start, int finish, char *pal )
{
int l;
int r, g, b;

	for ( l = 0; l != ( finish - start ) + 1; l++ )
	{
		vga_getpalette( start + l, &r, &g, &b );
		pal[ l * 3 ] = r;
		pal[ l * 3 + 1 ] = g;
		pal[ l * 3 + 2 ] = b;
	}
}

void G_waitretrace()
{
	vga_waitretrace();
}
#endif
