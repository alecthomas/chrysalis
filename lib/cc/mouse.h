#ifndef __MOUSE_H__
#define __MOUSE_H__

class Mouse {
		int width, height;
	public :
		Mouse( int width, int height );
		~Mouse();
		void setPosition( int x, int y );
		void update();
		inline bool ok() { return !failed; }
		inline int getX() { return x; }
		inline int getY() { return y; }
		inline int getButton() { return button; }
		inline int getLastX() { return lastX; }
		inline int getLastY() { return lastY; }
		inline int getLastButton() { return lastButton; }
		inline int buttonPressedSinceLast() { return ( ( lastButton ^ button ) & button ); }
		inline int buttonReleasedSinceLast() { return ( ( lastButton ^ button ) & lastButton ); }
		inline bool moveChange() { return ( lastX != x || lastY != y ); }
		inline bool buttonChange() { return ( lastButton != button ); }
		inline bool eventAvailable() { return ( moveChange() || buttonChange() ); }
		enum {
			Left = 1,
			Right = 2,
			Middle = 4
		};
	private :
		static int active;
		bool failed;
		int x, y, lastX, lastY;
		unsigned button, lastButton;
};

#endif
