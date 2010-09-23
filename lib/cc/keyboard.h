#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#ifdef __SVGA__
void KeyboardEventHandler( int scancode, int press );
#endif

class Keyboard {
#ifdef __SVGA__
		friend void KeyboardEventHandler( int scancode, int press );
#endif
	public :
		Keyboard();
		~Keyboard();
		static void update();
		static unsigned scanCodeToASCII( unsigned scanCode );
		inline static bool keyDown( unsigned index ) { return keyState[ index ]; }
		inline static bool keyUp( unsigned index ) { return !keyState[ index ]; }
		inline static bool eventAvailable() { return ( readPos != writePos ); }
		inline static void clearEvent() { if ( readPos == writePos ) return; ++readPos &= 0xff; }
		inline static unsigned getScanCode() { return eventList[ readPos ].scanCode; }
		inline static unsigned getKey() { return eventList[ readPos ].ascii; }
		inline static bool keyPress() { return eventList[ readPos ].press; }
		inline static bool keyRelease() { return !eventList[ readPos ].press; }
		typedef enum {
			kbNULL, kbESC, kb1, kb2, kb3, kb4, kb5, kb6, kb7, kb8, kb9, kb0, kbMINUS, kbEQUALS, kbBACKSPACE, kbTAB, kbQ,
			kbW, kbE, kbR, kbT, kbY, kbU, kbI, kbO, kbP, kbSQUAREBRACKETOPEN, kbSQUAREBRACKETCLOSE, kbENTER, kbCTRL,
			kbA, kbS, kbD, kbF, kbG, kbH, kbJ, kbK, kbL, kbSEMICOLON, kbFORWARDAPOSTROPHE, kbBACKWARDAPOSTROPHE, kbLSHIFT,
			kbBACKSLASH, kbZ, kbX, kbC, kbV, kbB, kbN, kbM, kbCOMMA, kbPOINT, kbFORWARDSLASH, kbRSHIFT, kbMUL, kbALT, kbSPACE,
			kbCAPS, kbF1, kbF2, kbF3, kbF4, kbF5, kbF6, kbF7, kbF8, kbF9, kbF10, kbNUM, kbSCROLL, kbpHOME, kbpUP, kbpPGUP,
			kbpMINUS, kbpLEFT, kbpMIDDLE, kbpRIGHT, kbpPLUS, kbpEND, kbpDOWN, kbpPGDN, kbpINS, kbpDEL, kbF12 = 85, kbF11 = 87
		} KeyScanCode;
	private :
		static char keyState[ 128 ], keyTable[ 128 ], shiftKeyTable[ 128 ];
		typedef struct {
			bool press;
			unsigned scanCode, ascii;
		} keyEvent;
		static keyEvent eventList[ 256 ];
		static int readPos, writePos, active;
		static bool failed;
};
#endif
