#include "page.h"
#include "groot.h"
#include "video.h"

static id classPage = nil;
static const VideoDriver *classDriver = NULL;

@implementation GraphicRoot
-init
{
	page = classPage;
	driver = classDriver;
	return [super init];
}

+setPage:aPage
{
	if ( ![aPage conformsTo:@protocol( DirectPageProtocol )] )
		[GraphicRoot problem:Fatal :"page passed to %s does not conform to DirectPageProtocol", [self name] ];
	classPage = aPage;
	return self;
}

+setDriver:(const VideoDriver*)aDriver
{
	classDriver = aDriver;
	return self;
}

-setPage:aPage
{
	if ( ![aPage conformsTo:@protocol( DirectPageProtocol )] )
		[self problem:Fatal :"page passed to %s does not conform to DirectPageProtocol", [self name] ];
	page = aPage;
	return self;
}

-setDriver:(const VideoDriver*)aDriver
{
	driver = aDriver;
	return self;
}
@end
