#ifdef __SDL__
/*
	SDL2 frontend. Implements the frontend interface declared in mitshm.h
	(originally the X11 MIT-SHM frontend) so the rest of the library is
	unchanged: the game draws 8-bit indexed pixels into shared_mem, and
	updateScreen() converts them through the palette to the window.
*/

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keymappings.h"
#include "mitshm.h"

#define SCALE 3
// The game advances one simulation step per displayed frame, so presenting
// must be capped at the VGA mode 13h refresh rate the game was tuned for.
#define FRAMERATE 20

int xWidth;
int xHeight;
char *shared_mem;
char Xkeypressed;
char Xkeyreleased;
char keymap[128];
int Xmousebuttons = 0;
int Xmousex = 0, Xmousey = 0;

static char keylinear[513];
static int keylinearind = -1;
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static Uint32 colours[256];
static Uint32 *pixels;
static Uint32 lastpump;
static Uint64 lastframe;

static int scancodeToKey(SDL_Scancode s)
{
	switch (s) {
		case SDL_SCANCODE_ESCAPE: return kbESC;
		case SDL_SCANCODE_1: return kb1;
		case SDL_SCANCODE_2: return kb2;
		case SDL_SCANCODE_3: return kb3;
		case SDL_SCANCODE_4: return kb4;
		case SDL_SCANCODE_5: return kb5;
		case SDL_SCANCODE_6: return kb6;
		case SDL_SCANCODE_7: return kb7;
		case SDL_SCANCODE_8: return kb8;
		case SDL_SCANCODE_9: return kb9;
		case SDL_SCANCODE_0: return kb0;
		case SDL_SCANCODE_MINUS: return kbMINUS;
		case SDL_SCANCODE_EQUALS: return kbEQUALS;
		case SDL_SCANCODE_BACKSPACE: return kbBACKSPACE;
		case SDL_SCANCODE_TAB: return kbTAB;
		case SDL_SCANCODE_Q: return kbQ;
		case SDL_SCANCODE_W: return kbW;
		case SDL_SCANCODE_E: return kbE;
		case SDL_SCANCODE_R: return kbR;
		case SDL_SCANCODE_T: return kbT;
		case SDL_SCANCODE_Y: return kbY;
		case SDL_SCANCODE_U: return kbU;
		case SDL_SCANCODE_I: return kbI;
		case SDL_SCANCODE_O: return kbO;
		case SDL_SCANCODE_P: return kbP;
		case SDL_SCANCODE_LEFTBRACKET: return kbSQUAREBRACKETOPEN;
		case SDL_SCANCODE_RIGHTBRACKET: return kbSQUAREBRACKETCLOSE;
		case SDL_SCANCODE_RETURN: return kbENTER;
		case SDL_SCANCODE_LCTRL:
		case SDL_SCANCODE_RCTRL: return kbCTRL;
		case SDL_SCANCODE_A: return kbA;
		case SDL_SCANCODE_S: return kbS;
		case SDL_SCANCODE_D: return kbD;
		case SDL_SCANCODE_F: return kbF;
		case SDL_SCANCODE_G: return kbG;
		case SDL_SCANCODE_H: return kbH;
		case SDL_SCANCODE_J: return kbJ;
		case SDL_SCANCODE_K: return kbK;
		case SDL_SCANCODE_L: return kbL;
		case SDL_SCANCODE_SEMICOLON: return kbSEMICOLON;
		case SDL_SCANCODE_APOSTROPHE: return kbFORWARDAPOSTROPHE;
		case SDL_SCANCODE_GRAVE: return kbBACKWARDAPOSTROPHE;
		case SDL_SCANCODE_LSHIFT: return kbLSHIFT;
		case SDL_SCANCODE_BACKSLASH: return kbBACKSLASH;
		case SDL_SCANCODE_Z: return kbZ;
		case SDL_SCANCODE_X: return kbX;
		case SDL_SCANCODE_C: return kbC;
		case SDL_SCANCODE_V: return kbV;
		case SDL_SCANCODE_B: return kbB;
		case SDL_SCANCODE_N: return kbN;
		case SDL_SCANCODE_M: return kbM;
		case SDL_SCANCODE_COMMA: return kbCOMMA;
		case SDL_SCANCODE_PERIOD: return kbPOINT;
		case SDL_SCANCODE_SLASH: return kbFORWARDSLASH;
		case SDL_SCANCODE_RSHIFT: return kbRSHIFT;
		case SDL_SCANCODE_KP_MULTIPLY: return kbMUL;
		case SDL_SCANCODE_LALT:
		case SDL_SCANCODE_RALT: return kbALT;
		case SDL_SCANCODE_SPACE: return kbSPACE;
		case SDL_SCANCODE_CAPSLOCK: return kbCAPS;
		case SDL_SCANCODE_F1: return kbF1;
		case SDL_SCANCODE_F2: return kbF2;
		case SDL_SCANCODE_F3: return kbF3;
		case SDL_SCANCODE_F4: return kbF4;
		case SDL_SCANCODE_F5: return kbF5;
		case SDL_SCANCODE_F6: return kbF6;
		case SDL_SCANCODE_F7: return kbF7;
		case SDL_SCANCODE_F8: return kbF8;
		case SDL_SCANCODE_F9: return kbF9;
		case SDL_SCANCODE_F10: return kbF10;
		case SDL_SCANCODE_F11: return kbF11;
		case SDL_SCANCODE_F12: return kbF12;
		case SDL_SCANCODE_NUMLOCKCLEAR: return kbNUM;
		case SDL_SCANCODE_SCROLLLOCK: return kbSCROLL;
		case SDL_SCANCODE_HOME:
		case SDL_SCANCODE_KP_7: return kbpHOME;
		case SDL_SCANCODE_UP:
		case SDL_SCANCODE_KP_8: return kbpUP;
		case SDL_SCANCODE_PAGEUP:
		case SDL_SCANCODE_KP_9: return kbpPGUP;
		case SDL_SCANCODE_KP_MINUS: return kbpMINUS;
		case SDL_SCANCODE_LEFT:
		case SDL_SCANCODE_KP_4: return kbpLEFT;
		case SDL_SCANCODE_KP_5: return kbpMIDDLE;
		case SDL_SCANCODE_RIGHT:
		case SDL_SCANCODE_KP_6: return kbpRIGHT;
		case SDL_SCANCODE_KP_PLUS: return kbpPLUS;
		case SDL_SCANCODE_END:
		case SDL_SCANCODE_KP_1: return kbpEND;
		case SDL_SCANCODE_DOWN:
		case SDL_SCANCODE_KP_2: return kbpDOWN;
		case SDL_SCANCODE_PAGEDOWN:
		case SDL_SCANCODE_KP_3: return kbpPGDN;
		case SDL_SCANCODE_INSERT:
		case SDL_SCANCODE_KP_0: return kbpINS;
		case SDL_SCANCODE_DELETE:
		case SDL_SCANCODE_KP_PERIOD: return kbpDEL;
		default: return kbNULL;
	}
}

static int buttonMask(Uint8 button)
{
	switch (button) {
		case SDL_BUTTON_LEFT: return 1;
		case SDL_BUTTON_RIGHT: return 2;
		case SDL_BUTTON_MIDDLE: return 4;
		case SDL_BUTTON_X1: return 8;
		case SDL_BUTTON_X2: return 16;
		default: return 0;
	}
}

void setTitleX(char *s)
{
	SDL_SetWindowTitle(window, s);
}

void setMouse(int x, int y)
{
	int wx, wy;

	SDL_RenderLogicalToWindow(renderer, (float)x, (float)y, &wx, &wy);
	SDL_WarpMouseInWindow(window, wx, wy);
	Xmousex = x;
	Xmousey = y;
}

void initKeylinear()
{
	keylinearind = 0;
}

char *queryKeylinear()
{
	keylinear[keylinearind] = 0;
	keylinearind = -1;
	return keylinear;
}

int queryKeyboard(int key)
{
	return keymap[key];
}

void initKeyboard()
{
	memset(keymap, 0, sizeof(keymap));
	keylinearind = -1;
}

static void clampMouse()
{
	if (Xmousex < 0) Xmousex = 0;
	if (Xmousey < 0) Xmousey = 0;
	if (Xmousex >= xWidth) Xmousex = xWidth - 1;
	if (Xmousey >= xHeight) Xmousey = xHeight - 1;
}

void updateKeyboard()
{
	SDL_Event ev;
	Uint32 now;
	int key;

	// The game polls input inside tight loops (every K_index() call), and
	// pumping the macOS event loop is expensive, so pump at most once per
	// millisecond.
	now = SDL_GetTicks();
	if (now == lastpump) return;
	lastpump = now;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
			case SDL_QUIT:
				exit(0);
			case SDL_MOUSEBUTTONDOWN:
				Xmousebuttons |= buttonMask(ev.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				Xmousebuttons &= ~buttonMask(ev.button.button);
				break;
			case SDL_MOUSEMOTION:
				Xmousex = ev.motion.x;
				Xmousey = ev.motion.y;
				clampMouse();
				break;
			case SDL_WINDOWEVENT:
				if (ev.window.event == SDL_WINDOWEVENT_LEAVE)
					Xmousebuttons = 0;
				break;
			case SDL_KEYDOWN:
				if (ev.key.repeat) break;
				if ((key = scancodeToKey(ev.key.keysym.scancode)) == kbNULL) break;
				keymap[(int)(Xkeypressed = key)] = 1;
				if (keylinearind != -1 && keylinearind < 512)
					keylinear[keylinearind++] = key;
				break;
			case SDL_KEYUP:
				if ((key = scancodeToKey(ev.key.keysym.scancode)) == kbNULL) break;
				keymap[(int)(Xkeyreleased = key)] = 0;
				break;
		}
	}
}

void updateScreen()
{
	Uint64 frame, now;
	int l;

	for (l = 0; l != xWidth * xHeight; l++)
		pixels[l] = colours[(unsigned char)shared_mem[l]];
	SDL_UpdateTexture(texture, NULL, pixels, xWidth * sizeof(Uint32));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	frame = SDL_GetPerformanceFrequency() / FRAMERATE;
	now = SDL_GetPerformanceCounter();
	if (now - lastframe < frame)
		SDL_Delay((Uint32)((frame - (now - lastframe)) * 1000 / SDL_GetPerformanceFrequency()));
	lastframe = SDL_GetPerformanceCounter();
	updateKeyboard();
}

void updateScreenClipped(int x, int y, int w, int h)
{
	(void)x; (void)y; (void)w; (void)h;
	updateScreen();
}

void setColor(int index, int r, int g, int b)
{
	colours[index] = 0xff000000 | (r << 16) | (g << 8) | b;
}

void Xuninitialize()
{
	static char uninitialized = 0;

	if (uninitialized) return;
	uninitialized = 1;
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Xinitialize(int width, int height)
{
	xWidth = width;
	xHeight = height;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL: could not initialise video: %s\n", SDL_GetError());
		exit(1);
	}
	window = SDL_CreateWindow("Chrysalis", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, width * SCALE, height * SCALE,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	if (window == NULL) {
		fprintf(stderr, "SDL: could not create window: %s\n", SDL_GetError());
		exit(1);
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL) {
		fprintf(stderr, "SDL: could not create renderer: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_RenderSetLogicalSize(renderer, width, height);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, width, height);
	shared_mem = (char *)calloc(width * height, 1);
	pixels = (Uint32 *)calloc(width * height, sizeof(Uint32));
	// The game and editor draw their own pointer.
	SDL_ShowCursor(SDL_DISABLE);
	atexit(Xuninitialize);
}

#endif /* __SDL__ */
