#include <windows/manager.h>
#include <windows/window.h>
#include <windows/button.h>
#include <windows/raisedregion.h>
#include <cc/video.h>
#include <iostream.h>

class Root : public BufferedRegion {
	public :
		Root( Manager *m ) : 
			BufferedRegion( m, 0, 0, m->getWidth(), m->getHeight() ) {}

		bool mousePress( unsigned button, int x, int y )
		{
			page->filledBox( x, y, 20, 20, Colour( 255, 255, 255 ) );
			manager->sendMessage( Region::PartialDisplay, x, y, 20, 20 );
			return false;
		}
};

Manager *manager;
Video *video;

void main( int argc, char **argv )
{
	if ( argc < 2 )
		video = new Video( "640x480x256" );
	else
		video = new Video( argv[ 1 ] );
	manager = new Manager( video );
	manager->setRoot( new Root( manager ) );
	manager->flags.autoRaise = true;
	manager->addRegion( new Window( manager, 30, 30, 100, 100, "Little" ) );
	manager->addRegion( new Window( manager, 40, 40, 300, 200, "Big" ) );
	manager->addRegion( new Window( manager, 20, 20, 500, 200, "Huge" ) );
	manager->addRegion( new Window( manager, 100, 20, 100, 300, "Long" ) );
	manager->go();
}
