#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "page.h"

typedef struct {
	char *(*read)( char *target );
	char *(*readRow)( char *target, int y );
	char *(*readSpan)( char *target, int x, int y, int width );
	void (*write)( char *source );
	void (*writeRow)( char *source, int y );
	void (*writeSpan)( char *source, int x, int y, int width);
} VideoDriver;

@interface Video : DirtyPage
{
	char *video;
	id *videoObject;
	VideoDriver driver;
}

-init;
-init:(int)width :(int)height;

-free;

-write:(char*)source; 
-write:(char*)source :(int)y; 
-write:(char*)source :(int)x :(int)y :(int)width; 

-read:(char*)target; 
-read:(char*)target :(int)y; 
-read:(char*)target :(int)x :(int)y :(int)width; 

-(const VideoDriver*)getDriver;

-sync;
-setPalette:(char*)palette;

-(char*)getBuffer;

@end

#endif
