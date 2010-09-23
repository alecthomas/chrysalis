#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "groot.h"

@class Page;

#define MOV						(1 << 24)
#define MASK					(2 << 24)
#define XOR						(3 << 24)
#define OR						(4 << 24)
#define AND						(5 << 24)
#define TRANSLUCENT				(6 << 24)
#define TINT( C, F )			(7 << 24) | (C) | ((F) << 8)
#define TINTBACK( C, F )		(8 << 24) | (C) | ((F) << 8)
#define SINGLE( C )				(9 << 24) | (unsigned char)(C)
#define SOLID( C )				(10 << 24) | (unsigned char)(C)
#define FADE( C )				(11 << 24) | (signed char)(C)+15
#define ALIAS					(12 << 24)
#define SHADE( C )				(13 << 24) | (signed char)(C)+15
#define TRANSLUCENTFADE( C )	( 14 << 24 ) | (signed char)(C)+15
#define FLIPPED					(64 << 24)
#define REVERSED				(32 << 24)
#define OUTLINE					(127 << 24)

@interface Image : GraphicRoot
{
	int width, height;
	char *buffer;
	unsigned int mode;
}
-init;
-init:(int)width :(int)height;
-free;

+setMode:(unsigned int)mode;

-put:(int)x :(int)y;

-load:(FILE*)aFile;
-loadFile:(char*)fileName;
-save:(FILE*)aFile;
-saveFile:(char*)fileName;

-setMode:(unsigned int)mode;
@end

#endif
