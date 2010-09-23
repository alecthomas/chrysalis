#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "video.h"

const VideoDriver *driver;
id video;

int main( int argc, char **argv )
{
	[Root autoContinue:'n'];

	// Initialise video
	video = [[Video alloc] init];
	if ( [video problems] ) return 1;
	driver = [video getDriver];

	// Set default behaviours of classes
	[GraphicRoot setPage:video];
	[GraphicRoot setDriver:[video getDriver]];
	[Image setMode:MASK];
	return 0;
}
