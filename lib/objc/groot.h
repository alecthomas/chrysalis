#ifndef __GROOT_H__
#define __GROOT_H__

#include "root.h"
#include "video.h"

@interface GraphicRoot : Root
{
	id page;
	const VideoDriver *driver;
}
-init;
+setPage:aPage;
+setDriver:(const VideoDriver*)aDriver;
-setPage:aPage;
-setDriver:(const VideoDriver*)aDriver;
@end
#endif
