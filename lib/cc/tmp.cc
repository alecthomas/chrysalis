#include "video.h"
#include "mouse.h"
#include "image.h"

int main()
{
Video *video;
Mouse *mouse;
Image *image;
int x, y, l;

	video = new Video( "320x200" );
	for ( l = 0; l != 200; l++ ) video->filledBox( 0, l, 320, 1, Colour( l ) );
	mouse = new Mouse( 320, 200 );
	while ( !mouse->getButton() )
	{
		mouse->update();
		x = mouse->getX(); y = mouse->getY();
		image = video->saveBox( x, y, 20, 20 );
		video->filledBox( x, y, 20, 20, Colour( 255, 255, 255 ) );
		video->sync();
		image->put( x, y, Page::MOV );
		delete image;
	}
	delete video;
	return 1;
}
