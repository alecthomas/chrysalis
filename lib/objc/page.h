#ifndef __PAGE_H__
#define __PAGE_H__

#include "root.h"

typedef enum {
	Grid
} DirtyPageMode;

/* Various protocols. Useful for checking if an object can do funky stuff :) */
@protocol PageProtocol
-sync;
-(int)getWidth;
-(int)getHeight;
@end

@protocol WriteablePageProtocol
-write:(char*)source;
-write:(char*)source :(int)y;
-write:(char*)source :(int)x :(int)y :(int)width;
@end

@protocol ReadablePageProtocol
-read:(char*)target;
-read:(char*)target :(int)y;
-read:(char*)target :(int)x :(int)y :(int)width;
@end

@protocol DirtyPageProtocol
-flush;
-setDirtyMode:(DirtyPageMode)mode;
-setDirtyGridSize:(int)width :(int)height;
-dirty:(int)x :(int)y :(int)width :(int)height;
-clean:(int)x :(int)y :(int)width :(int)height;
@end

@protocol DirectPageProtocol
-(char*)getBuffer;
-(char**)getRowTable;
@end

@interface Page : Root <PageProtocol, DirectPageProtocol, ReadablePageProtocol, 
	WriteablePageProtocol>
{
	int width, height, size;
@public
	char *buffer, **rowTable;
}
-init;
-init:(int)width :(int)height;

-sync;
-(char*)getBuffer;
-(char**)getRowTable;

-read:(char*)target;
-read:(char*)target :(int)y;
-read:(char*)target :(int)x :(int)y :(int)width;

-write:(char*)source;
-write:(char*)source :(int)y;
-write:(char*)source :(int)x :(int)y :(int)width;
@end

@interface DirtyPage : Page <DirtyPageProtocol>
{
}

-init;
-init:(int)width :(int)height;

-flush;
-setDirtyMode:(DirtyPageMode)mode;
-setDirtyGridSize:(int)width :(int)height;
-dirty:(int)x :(int)y :(int)width :(int)height;
-clean:(int)x :(int)y :(int)width :(int)height;
@end

#endif
