#include <stdlib.h>
#include <objc/video.h>
#include <objc/image.h>

int main( int argc, char **argv )
{
id video, image;

	video = [[Video alloc] init];
	[Image setPage:video];
	[Image setMode:MASK];
	image = [[[Image alloc] init] loadFile:"triffid"];
	[image put:-10 :0];
	[video sync];
	getchar();
	[video free];
	return 0;
}
