#include "page.h"

/*
	Implementation of basic page class.
*/
@implementation Page
-init
{
	return [self init:320 :200];
}

-init:(int)w :(int)h
{
	[super init];
	width = w; height = h;
	size = w * h;
	return self;
}

-write:(char*)source
{
	memcpy( buffer, source, size );
	return self;
}

-write:(char*)source :(int)y
{
	memcpy( rowTable[ y ], source, width );
	return self;
}

-write:(char*)source :(int)x :(int)y :(int)w
{
	memcpy( rowTable[ y ] + x, source, w );
	return self;
}

-read:(char*)target
{
	memcpy( target, buffer, size );
	return self;
}

-read:(char*)target :(int)y
{
	memcpy( target, rowTable[ y ], width );
	return self;
}

-read:(char*)target :(int)x :(int)y :(int)w
{
	memcpy( target, rowTable[ y ] + x, w );
	return self;
}

-(char**)getRowTable
{
	return rowTable;
}

-(char*)getBuffer
{
	return buffer;
}

-(int)getWidth
{
	return width;
}

-(int)getHeight
{
	return height;
}

-(int)getSize
{
	return size;
}

-sync
{
	return self;
}
@end



/*
	Implementation of a dirty page class, which handles dirty updating of 
	various types.
*/
@implementation DirtyPage
-init
{
	[self init:320 :200];
	return self;
}

-init:(int)w :(int)h
{
	[super init :w :h];
	return self;
}

-flush
{
	[self problem:Mild :"dirty page messages not implemented yet"];
	return self;
}

-setDirtyMode:(DirtyPageMode)mode
{
	return self;
}

-setDirtyGridSize:(int)width :(int)height
{
	return self;
}

-dirty:(int)x :(int)y :(int)width :(int)height
{
	return self;
}

-clean:(int)x :(int)y :(int)width :(int)height
{
	return self;
}
@end
