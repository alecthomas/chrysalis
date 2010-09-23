#include <stdlib.h>
#include "image.h"
#include "page.h"

extern void V_blit( char *source, char *target, unsigned len, unsigned mode );

static int defaultMode = MASK;

@implementation Image
+setMode:(unsigned int)m
{
	defaultMode = m;
	return self;
}

-init
{
	width = height = 0;
	buffer = NULL;
	[self setMode:defaultMode];
	return [super init];
}

-init:(int)w :(int)h
{
	width = w; height = h;
	buffer = (char*)malloc( width * height + sizeof( int ) * 2 );
	((int*)buffer)[ 0 ] = width;
	((int*)buffer)[ 1 ] = height;
	[self setMode:defaultMode];
	return [super init];
}

-free
{
	if ( buffer ) free( buffer );
	return [super free];
}

-load:(FILE*)aFile
{
	if ( buffer ) free( buffer );
	fread( &width, sizeof( int ), 1, aFile );
	fread( &height, sizeof( int ), 1, aFile );
	if ( width >= 0xffff )
	{
		[self problem:Mild :"possible 16-bit image, converting to 32-bit"];
		height = width >> 16;
		width &= 0xffff;
	}
	if ( width * height > 0xfffff )
		[self problem:Nasty :"image > 1M, possibly corrupt file"];
	buffer = (char*)malloc( width * height + sizeof( int ) * 2 );
	fread( buffer + sizeof( int ) * 2, width * height, 1, aFile );
	((int*)buffer)[ 0 ] = width;
	((int*)buffer)[ 1 ] = width;
	[self setMode:defaultMode];
	return self;
}

-loadFile:(char*)fileName
{
FILE *f;

	if ( ( f = fopen( fileName, "rb" ) ) == NULL )
		[self problem:Nasty :"could not open image file"];
	[self load:f];
	fclose( f );
	return self;
}

-save:(FILE*)aFile
{
	fwrite( &width, sizeof( int ), 1, aFile );
	fwrite( &height, sizeof( int ), 1, aFile );
	fwrite( buffer + sizeof( int ) * 2, width * height, 1, aFile );
	return self;
}

-saveFile:(char*)fileName
{
FILE *f;

	if ( ( f = fopen( fileName, "ab" ) ) == NULL ) 
		[self problem:Nasty :"could not save image"];
	[self save:f];
	fclose( f );
	return self;
}

-setMode:(unsigned int)m
{
	mode = m;
	return self;
}

-put:(int)x :(int)y
{
int l, w, h;
char *image, **rowTable;

	image = buffer;
	w = width;
	h = height;
	image += 8;
	if ( w < 0 )
	{ 
		w = -w + 1; 
		x -= w - 1; 
	} 
	if ( h < 0 ) 
	{ 
		h = -h + 1; 
		y -= h - 1; 
	} 
	if ( y < 0 ) 
		{ h += y; image -= y * w; y = 0; } 
	if ( x < 0 ) 
		{ w += x; image -= x; x = 0; } 
	if ( x + w - 1 >= [page getWidth] ) 
		w = [page getWidth] - x; 
	if ( y + h - 1 >= [page getHeight] ) 
		h = [page getHeight] - y; 
	if ( w <= 0 || h <= 0 ) return self;

//	if ( ( mode >> 24 ) == TRANSLUCENT && TBL_translucent == NULL ) mode = MASK;

	rowTable = [page getRowTable];
	for ( l = 0; l != h; l++ )
		V_blit( image + l * width, rowTable[ l + y ] + x, w, mode );
	return self;
}
@end
