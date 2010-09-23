#include <stdlib.h>
#include <stdio.h>
#include "video.h"

#ifdef __X11__
#include <mitshm.h>
#endif

char *videoPtr = NULL, **videoRowTable = NULL;
static bool inVideo = false;
static int videoWidth = 320, videoHeight = 200, videoSize = 320 * 200;
static char defaultPalette[] = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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

char *readPage( char *r )
{
	return memcpy( r, videoPtr, videoSize );
}

char *readRow( char *r, int y )
{
	return memcpy( r, videoRowTable[ y ], videoWidth );
}

char *readSpan( char *r, int x, int y, int width )
{
	return memcpy( r, videoRowTable[ y ] + x, width );
}

void writePage( char *r )
{
	memcpy( videoPtr, r, videoSize );
}

void writeRow( char *r, int y )
{
	memcpy( videoRowTable[ y ], r, videoWidth );
}

void writeSpan( char *r, int x, int y, int width )
{
	memcpy( videoRowTable[ y ] + x, r, width );
}

static void autoExitVideo()
/* =========================
	Just so nothing bad happens when the program exits, like leaving the 
	system in graphics mode for instance (witness SVGALib).
*/
{
#ifdef __X11__
	Xuninitialize();
#endif
}

@implementation Video
// initialise graphics mode
-init
{
	return [self init:320 :200];
}

-init:(int)w :(int)h
{
int i;

	if ( [super init:w :h] == nil ) return nil;
	if ( inVideo ) 
		[Video problem:Nasty :"Re-initialization of video mode while in video mode"];
#ifdef __X11__
	Xinitialize( w, h );
	atexit( autoExitVideo );
	video = shared_mem;
	videoWidth = w; videoHeight = h;
	videoSize = videoWidth * videoHeight;
	rowTable = (char**)malloc( sizeof( char* ) * videoHeight );
	for ( i = 0; i != videoHeight; i++ ) rowTable[ i ] = video + videoWidth * i;
	// initialise static class variables
	videoPtr = video;
	videoRowTable = rowTable;
	[self setPalette:defaultPalette];
	driver.read = readPage;
	driver.readRow = readRow;
	driver.readSpan = readSpan;
	driver.write = writePage;
	driver.writeRow = writeRow;
	driver.writeSpan = writeSpan;
#else
	[self problem:"non-X graphics not implemented yet" :Fatal ];
	exit( 1 );
#endif
	buffer = video;
	return self;
}

-free
{
	if ( !inVideo )
		[self problem:Mild :"deconstruction of Video, when not in video mode?!?!"];
	inVideo = false;
	Xuninitialize();
	if ( buffer != video ) free( buffer );
	return [super free];
}

-(const VideoDriver*)getDriver
{
	return &driver;
}

// write directly to video from buffer
-write:(char*)source
{
	memcpy( buffer, source, videoWidth );
	return self;
}

-write:(char*)source :(int)y
{
	memcpy( rowTable[ y ], source, videoWidth );
	return self;
}

-write:(char*)source :(int)x :(int)y :(int)w
{
	memcpy( rowTable[ y ] + x, source, w );
	return self;
}


// read directly from video into a buffer
-read:(char*)target
{
	memcpy( target, buffer, videoSize );
	return self;
}

-read:(char*)target :(int)y
{
	memcpy( target, rowTable[ y ], videoWidth );
	return self;
}

-read:(char*)target :(int)x :(int)y :(int)w
{
	memcpy( target, rowTable[ y ] + x, w );
	return self;
}


// display screen
-sync
{
#ifdef __X11__
	if ( buffer != video ) memcpy( video, buffer, 64000 );
	updateScreen();
#endif
	return self;
}

-setPalette:(char*)palette
{
int i;

#ifdef __X11__
	for ( i = 0; i != 256; i++ )
		setColor( i, (int)palette[ i * 3 ] * 4, 
			(int)palette[ i * 3 + 1 ] * 4, 
			(int)palette[ i * 3 + 2 ] * 4 );
#endif
	return self;
}

-(char*)getBuffer
{
	return buffer;
}

@end
