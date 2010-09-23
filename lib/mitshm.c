#ifdef __X11__
#define _MITSHM_H_
/* Copyright (c) 1996, Brian Spilsbury. All right reservered */
/* May be used with permission of author. */
/* This includes Forge Software */

#define _X_LOCAL_
#include "keymappings.h"
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include "nocursorm.h"

void updateScreenClipped( int x, int y, int w, int h );

static void GetShmPixmap ();
extern int XShmQueryExtension (Display * dpy);
static int haderror;
static int (*origErrorHandler) (Display *, XErrorEvent *);

int xWidth;		/* hmm, might as well be global */
int xHeight;
char *shared_mem;	/* <---- draw into this one... */
char Xkeypressed;
char Xkeyreleased;
char keymap[128];
int Xmousebuttons = 0;
int Xmousex = 0, Xmousey = 0;
static char keylinear[513];
static int keylinearind = -1;
static XShmSegmentInfo shminfo;
static int shared_len;
static Display *dp;
static Pixmap pixmap;
static int screen;
static Window wi;
static Colormap colours;
static XColor xcol;
static Pixmap cursor;

void setTitleX(char *s)
{
	XStoreName(dp, wi, s);
}

/*
void queryMouse(int *x, int *y, unsigned int *state)
{
	static int dummy;
	static Window dw, ch;
	static unsigned int nb;

	XQueryPointer(dp, wi, &ch, &dw, &dummy, &dummy, x, y, &nb);

	if (ch != wi && dw != wi) {
		*x = *y = 20;
		*state = 0;
		return;
		}

	*state = 0;
	if (nb & Button1Mask) *state |= 1;
	if (nb & Button2Mask) *state |= 4;
	if (nb & Button3Mask) *state |= 2;
	if (nb & Button4Mask) *state |= 8;
	if (nb & Button5Mask) *state |= 16;
}
*/

void setMouse(int x, int y)
{
	XWarpPointer(dp, None, wi, 0, 0, 0, 0, x, y);
}

void initKeylinear()
{
	keylinearind = 0;
}

char *queryKeylinear()
{
	keylinear[keylinearind] = 0;
	keylinearind = -1;
	return(keylinear);
}

int queryKeyboard(int key)
{
	return(keymap[key]);
}

void initKeyboard()
{
	int i;
	for(i=0;i<=kbpDEL;i++) {
		keymap[i] = 0;
		}
	keylinearind = -1;
}

void updateKeyboard()
{
	XEvent xv;
	KeySym key;

	while(XPending(dp)) {
		XNextEvent(dp, &xv);
		switch(xv.type) {
			case Expose:
			case GraphicsExpose:
				updateScreenClipped(
				xv.xexpose.x,
				xv.xexpose.y,
				xv.xexpose.width,
				xv.xexpose.height);
			break;
			case ButtonPress:
				switch(xv.xbutton.button) {
					case 1: Xmousebuttons |= 1;
						break;
					case 2: Xmousebuttons |= 4;
						break;
					case 3: Xmousebuttons |= 2;
						break;
						break;
					case 4: Xmousebuttons |= 8;
						break;
					case 5: Xmousebuttons |= 16;
						break;
					}
				break;
			case ButtonRelease:
				switch(xv.xbutton.button) {
					case 1: Xmousebuttons &= ~1;
						break;
					case 2: Xmousebuttons &= ~4;
						break;
					case 3: Xmousebuttons &= ~2;
						break;
						break;
					case 4: Xmousebuttons &= ~8;
						break;
					case 5: Xmousebuttons &= ~16;
						break;
					}
				break;
			case MotionNotify:
				Xmousex = xv.xmotion.x;
				Xmousey = xv.xmotion.y;
				break;
			case LeaveNotify :
				Xmousebuttons = 0;
				break;
			case KeyPress:
				key = XLookupKeysym(&xv.xkey, 0);
#ifdef __DEBUG__
				printf("Down:%x\n", key);
#endif
#define d(x) keymap[ ( Xkeypressed = x ) ] = 1; \
		if (keylinearind != -1) { 		\
		if (keylinearind < 512) {		\
			keylinear[keylinearind++] = x;\
			}				\
		}
				switch(key) {
					case XK_Escape: d(kbESC); break;
					case XK_1: d(kb1); break;
					case XK_2: d(kb2); break;
					case XK_3: d(kb3); break;
					case XK_4: d(kb4); break;
					case XK_5: d(kb5); break;
					case XK_6: d(kb6); break;
					case XK_7: d(kb7); break;
					case XK_8: d(kb8); break;
					case XK_9: d(kb9); break;
					case XK_0: d(kb0); break;
					case XK_minus: d(kbMINUS); break;
					case XK_BackSpace: d(kbBACKSPACE); break;
					case XK_Tab: d(kbTAB); break;
					case XK_a:
					case XK_A: d(kbA); break;
					case XK_b:
					case XK_B: d(kbB); break;
					case XK_c:
					case XK_C: d(kbC); break;
					case XK_d:
					case XK_D: d(kbD); break;
					case XK_e:
					case XK_E: d(kbE); break;
					case XK_f:
					case XK_F: d(kbF); break;
					case XK_g:
					case XK_G: d(kbG); break;
					case XK_h:
					case XK_H: d(kbH); break;
					case XK_i:
					case XK_I: d(kbI); break;
					case XK_j:
					case XK_J: d(kbJ); break;
					case XK_k:
					case XK_K: d(kbK); break;
					case XK_l:
					case XK_L: d(kbL); break;
					case XK_m:
					case XK_M: d(kbM); break;
					case XK_n:
					case XK_N: d(kbN); break;
					case XK_o:
					case XK_O: d(kbO); break;
					case XK_p:
					case XK_P: d(kbP); break;
					case XK_q:
					case XK_Q: d(kbQ); break;
					case XK_r:
					case XK_R: d(kbR); break;
					case XK_s:
					case XK_S: d(kbS); break;
					case XK_t:
					case XK_T: d(kbT); break;
					case XK_u:
					case XK_U: d(kbU); break;
					case XK_v:
					case XK_V: d(kbV); break;
					case XK_w:
					case XK_W: d(kbW); break;
					case XK_x:
					case XK_X: d(kbX); break;
					case XK_y:
					case XK_Y: d(kbY); break;
					case XK_z:
					case XK_Z: d(kbZ); break;
					case XK_bracketleft: d(kbSQUAREBRACKETOPEN); break;
					case XK_bracketright: d(kbSQUAREBRACKETCLOSE); break;
					case XK_Return: d(kbENTER); break;
					case XK_Control_R:
					case XK_Control_L: d(kbCTRL); break;
					case XK_semicolon: d(kbSEMICOLON); break;
					case XK_apostrophe: d(kbFORWARDAPOSTROPHE); break;
					case XK_quoteleft: d(kbBACKWARDAPOSTROPHE); break;
					case XK_Shift_L: d(kbLSHIFT); break;
					case XK_backslash: d(kbBACKSLASH); break;
					case XK_comma: d(kbCOMMA); break;
					case XK_period: d(kbPOINT); break;
					case XK_slash: d(kbFORWARDSLASH); break;
					case XK_Shift_R: d(kbRSHIFT); break;
					case XK_multiply: d(kbMUL); break;
					case XK_Alt_L:
					case XK_Alt_R: d(kbALT); break;
					case XK_space: d(kbSPACE); break;
					case XK_Caps_Lock: d(kbCAPS); break;
					case XK_F1: d(kbF1); break;
					case XK_F2: d(kbF2); break;
					case XK_F3: d(kbF3); break;
					case XK_F4: d(kbF4); break;
					case XK_F5: d(kbF5); break;
					case XK_F6: d(kbF6); break;
					case XK_F7: d(kbF7); break;
					case XK_F8: d(kbF8); break;
					case XK_F9: d(kbF9); break;
					case XK_F10: d(kbF10); break;
					case XK_F11: d(kbF11); break;
					case XK_F12: d(kbF12); break;
					case XK_Num_Lock: d(kbNUM); break;
					case XK_Scroll_Lock: d(kbSCROLL); break;
					case XK_Home:
					case XK_KP_Home: d(kbpHOME); break;
					case XK_Up:
					case XK_KP_Up: d(kbpUP); break;
					case XK_Page_Up:
					case XK_KP_Page_Up: d(kbpPGUP); break;
					case XK_KP_Subtract: d(kbpMINUS); break;
					case XK_Left:
					case XK_KP_Left: d(kbpLEFT); break;
					case XK_KP_5: d(kbpMIDDLE); break;
					case XK_Right:
					case XK_KP_Right: d(kbpRIGHT); break;
					case XK_KP_Add: d(kbpPLUS); break;
					case XK_End:
					case XK_KP_End: d(kbpEND); break;
					case XK_Down:
					case XK_KP_Down: d(kbpDOWN); break;
					case XK_Page_Down:
					case XK_KP_Page_Down: d(kbpPGDN); break;
					case XK_Insert:
					case XK_KP_Insert: d(kbpINS); break;
					case XK_Delete:
					case XK_KP_Delete: d(kbpINS); break;
					}
				break;
			case KeyRelease:
				key = XLookupKeysym(&xv.xkey, 0);
#undef d
#define d(x) keymap[ ( Xkeyreleased = x ) ] = 0
				switch(key) {
					case XK_Escape: d(kbESC); break;
					case XK_1: d(kb1); break;
					case XK_2: d(kb2); break;
					case XK_3: d(kb3); break;
					case XK_4: d(kb4); break;
					case XK_5: d(kb5); break;
					case XK_6: d(kb6); break;
					case XK_7: d(kb7); break;
					case XK_8: d(kb8); break;
					case XK_9: d(kb9); break;
					case XK_0: d(kb0); break;
					case XK_minus: d(kbMINUS); break;
					case XK_BackSpace: d(kbBACKSPACE); break;
					case XK_Tab: d(kbTAB); break;
					case XK_a:
					case XK_A: d(kbA); break;
					case XK_b:
					case XK_B: d(kbB); break;
					case XK_c:
					case XK_C: d(kbC); break;
					case XK_d:
					case XK_D: d(kbD); break;
					case XK_e:
					case XK_E: d(kbE); break;
					case XK_f:
					case XK_F: d(kbF); break;
					case XK_g:
					case XK_G: d(kbG); break;
					case XK_h:
					case XK_H: d(kbH); break;
					case XK_i:
					case XK_I: d(kbI); break;
					case XK_j:
					case XK_J: d(kbJ); break;
					case XK_k:
					case XK_K: d(kbK); break;
					case XK_l:
					case XK_L: d(kbL); break;
					case XK_m:
					case XK_M: d(kbM); break;
					case XK_n:
					case XK_N: d(kbN); break;
					case XK_o:
					case XK_O: d(kbO); break;
					case XK_p:
					case XK_P: d(kbP); break;
					case XK_q:
					case XK_Q: d(kbQ); break;
					case XK_r:
					case XK_R: d(kbR); break;
					case XK_s:
					case XK_S: d(kbS); break;
					case XK_t:
					case XK_T: d(kbT); break;
					case XK_u:
					case XK_U: d(kbU); break;
					case XK_v:
					case XK_V: d(kbV); break;
					case XK_w:
					case XK_W: d(kbW); break;
					case XK_x:
					case XK_X: d(kbX); break;
					case XK_y:
					case XK_Y: d(kbY); break;
					case XK_z:
					case XK_Z: d(kbZ); break;
					case XK_bracketleft: d(kbSQUAREBRACKETOPEN); break;
					case XK_bracketright: d(kbSQUAREBRACKETCLOSE); break;
					case XK_Return: d(kbENTER); break;
					case XK_Control_R:
					case XK_Control_L: d(kbCTRL); break;
					case XK_semicolon: d(kbSEMICOLON); break;
					case XK_apostrophe: d(kbFORWARDAPOSTROPHE); break;
					case XK_quoteleft: d(kbBACKWARDAPOSTROPHE); break;
					case XK_Shift_L: d(kbLSHIFT); break;
					case XK_backslash: d(kbBACKSLASH); break;
					case XK_comma: d(kbCOMMA); break;
					case XK_period: d(kbPOINT); break;
					case XK_slash: d(kbFORWARDSLASH); break;
					case XK_Shift_R: d(kbRSHIFT); break;
					case XK_multiply: d(kbMUL); break;
					case XK_Alt_L:
					case XK_Alt_R: d(kbALT); break;
					case XK_space: d(kbSPACE); break;
					case XK_Caps_Lock: d(kbCAPS); break;
					case XK_F1: d(kbF1); break;
					case XK_F2: d(kbF2); break;
					case XK_F3: d(kbF3); break;
					case XK_F4: d(kbF4); break;
					case XK_F5: d(kbF5); break;
					case XK_F6: d(kbF6); break;
					case XK_F7: d(kbF7); break;
					case XK_F8: d(kbF8); break;
					case XK_F9: d(kbF9); break;
					case XK_F10: d(kbF10); break;
					case XK_F11: d(kbF11); break;
					case XK_F12: d(kbF12); break;
					case XK_Num_Lock: d(kbNUM); break;
					case XK_Scroll_Lock: d(kbSCROLL); break;
					case XK_Home:
					case XK_KP_Home: d(kbpHOME); break;
					case XK_Up:
					case XK_KP_Up: d(kbpUP); break;
					case XK_Page_Up:
					case XK_KP_Page_Up: d(kbpPGUP); break;
					case XK_KP_Subtract: d(kbpMINUS); break;
					case XK_Left:
					case XK_KP_Left: d(kbpLEFT); break;
					case XK_KP_5: d(kbpMIDDLE); break;
					case XK_Right:
					case XK_KP_Right: d(kbpRIGHT); break;
					case XK_KP_Add: d(kbpPLUS); break;
					case XK_End:
					case XK_KP_End: d(kbpEND); break;
					case XK_Down:
					case XK_KP_Down: d(kbpDOWN); break;
					case XK_Page_Down:
					case XK_KP_Page_Down: d(kbpPGDN); break;
					case XK_Insert:
					case XK_KP_Insert: d(kbpINS); break;
					case XK_Delete:
					case XK_KP_Delete: d(kbpINS); break;
				}
				break;
			}
		}
}

void updateScreen()
{
	XClearWindow(dp, wi);
	XSync(dp, 0);
}

void updateScreenClipped( int x, int y, int w, int h )
{
	XClearArea(dp, wi, x, y, w, h, 0);
	XSync( dp, 0 );
}

void setColor(int index, int r, int g, int b)
{
	static XColor xcol;

	xcol.red = r<<8;
	xcol.green = g<<8;
	xcol.blue = b<<8;
	xcol.pixel = index;
	xcol.flags = DoRed|DoGreen|DoBlue;

	XStoreColor(dp, colours, &xcol);
}

static int shmErrorHandler(Display *d, XErrorEvent *e)
{
  if (e->error_code == BadAccess) fprintf (stderr, "X: shared memory didn't attach\n");
  else (*origErrorHandler) (d, e);

  printf("Dying Horribly\n");
  exit(666);
}

void XSignalErrorHandler (int num)
{
	if (shmdt (shminfo.shmaddr) < 0)
		perror ("XSignalErrorHandler:shmdt()");
	if (shmctl (shminfo.shmid, IPC_RMID, 0) < 0)
		perror ("XSignalErrorHandler:shmctl(rmid) error");
	printf ("fatal signal %i, wrapping up\n", num);

	XUnmapWindow (dp, wi);
	XFreePixmap (dp, pixmap);
	XSync (dp, True);
	XSync (dp, False);
	XShmDetach (dp, &shminfo);
	XSync (dp, True);
	XSync (dp, False);	/* don't leave events on the queue after we invalidate the pixmap */
      	XCloseDisplay (dp);
	_exit (0);
}

static void GetShmPixmap ()
{
  shared_len = xWidth*xHeight;
  shminfo.shmid = shmget (IPC_PRIVATE, shared_len, IPC_CREAT | 0777);
  if (shminfo.shmid < 0)
    {
      fprintf (stderr, "Couldn't get X shared memory.\n");
      exit (-1);
    }
/* We need to protect our shared memory against orphanage if we
   are killed in this function.  */

  shminfo.shmaddr = (char *) shmat (shminfo.shmid, 0, 0);
  shared_mem = (char *) shminfo.shmaddr;
  if (shminfo.shmaddr == (char *) -1)
    {
      fprintf (stderr, "Couldn't attach to X shared memory.\n");
      shmctl (shminfo.shmid, IPC_RMID, 0);
      exit (-1);
    }
  shminfo.readOnly = False;

  /* Now try to attach it to the X Server */
  haderror = False;
  origErrorHandler = XSetErrorHandler (shmErrorHandler);
  if (!XShmAttach (dp, &shminfo))
    {
      printf ("attach failed");
    }
  XSync (dp, True);		/* wait for error or ok */
  XSync (dp, False);		/* wait for error or ok */
  XSetErrorHandler (origErrorHandler);
  if (haderror)
    {
      if (shmdt (shminfo.shmaddr) < 0)
	perror ("X shmdt() error");
      if (shmctl (shminfo.shmid, IPC_RMID, 0) < 0)
	perror ("get:X shmctl rmid() error");
      exit (-1);
    }
  /*memset((void *)backbuf, 0, shared_len); */
  memset ((void *) shared_mem, 0, shared_len);

  /* Create the shared memory Pixmap */
  pixmap = (XShmCreatePixmap (dp, wi, shared_mem, &shminfo, xWidth, xHeight, 8));
  signal (SIGHUP, XSignalErrorHandler);
  signal (SIGINT, XSignalErrorHandler);
  signal (SIGTRAP, XSignalErrorHandler);
  signal (SIGABRT, XSignalErrorHandler);
  signal (SIGSEGV, XSignalErrorHandler);
  signal (SIGQUIT, XSignalErrorHandler);
  signal (SIGFPE, XSignalErrorHandler);
  signal (SIGTERM, XSignalErrorHandler);
  signal (SIGBUS, XSignalErrorHandler);
  signal (SIGIOT, XSignalErrorHandler);
  signal (SIGILL, XSignalErrorHandler);
}

void Xuninitialize ()
{
static char uninitialized = 0;

	if ( uninitialized ) return;
	uninitialized = 1;
	XUnmapWindow (dp, wi);
	XFreePixmap (dp, pixmap);
	XSync (dp, True);
	XSync (dp, False);
	XShmDetach (dp, &shminfo);
	XSync (dp, True);
	XSync (dp, False);	/* don't leave events on the queue after we invalidate the pixmap */
	if (shmdt (shminfo.shmaddr) < 0)
		perror ("Xuninitialize:shmdt()");
	if (shmctl (shminfo.shmid, IPC_RMID, 0) < 0)
		perror ("Xuninitialize:shmctl(rmid)");
      	XCloseDisplay (dp);
#ifdef __DEBUG__
      	printf ("xshm: MITSHM down ok\n");
#endif
}

void Xinitialize (int width, int height)
{
	XVisualInfo vinfo_return;
	XSetWindowAttributes wa;
	Pixmap mask, cur;
	XColor black, white;

	black.red = black.green = black.blue = 0;
	/* buggered if I care, its just for an invisible cursor :] */
	white.red = white.green = white.blue = 0;

	xWidth = width;
	xHeight = height;

#ifdef __DEBUG__
	printf ("xshm: connecting to X server\n");
#endif

	if ((dp = XOpenDisplay (0)) == 0) {
		printf ("xshm: could not open X display\n");
		exit (0);
		}

	XSetCloseDownMode (dp, DestroyAll);
	screen = DefaultScreen (dp);

	if (XMatchVisualInfo (dp, DefaultScreen (dp),
		8, PseudoColor, &vinfo_return) == False) {
		printf ("X: Screen doesn't support PseudoColor!\n");
		exit(666);
		}

	/* Make sure all is destroyed if killed off */
	/* Make sure we can do PsuedoColor colormap stuff */

	if (!XShmQueryExtension (dp)) {
		/* Check to see if the extensions are supported */
		fprintf (stderr, "X server doesn't support MITSHM extension.\n");
		exit(666);
	} else if (!XShmPixmapFormat (dp)) {
		fprintf (stderr, "X server doesn't do shared memory pixmaps.\n");
		exit(666);
	} 
#ifdef __DEBUG__
	else printf ("xshm: MITSHM present\n");
#endif


	wi = XCreateSimpleWindow (dp, RootWindow (dp, screen), 50, 50, xWidth, xHeight, 0, 0, 0);
        colours = XCreateColormap (dp, wi, DefaultVisual (dp, XDefaultScreen (dp)),
                                      AllocAll);
        XSetWindowColormap (dp, wi, colours);

        cur = XCreatePixmapFromBitmapData (dp, wi, (char *) nocursorm_bits,
                nocursorm_width, nocursorm_height, (unsigned long) 1,
                (unsigned long) 0, (unsigned int) 1);
        mask = XCreatePixmapFromBitmapData (dp, wi, (char *) nocursorm_bits,
                nocursorm_width, nocursorm_height, (unsigned long) 1,
                (unsigned long) 0, (unsigned int) 1);
	cursor = XCreatePixmapCursor (dp, cur, mask, &black, &white, 0, 0);

	XFreePixmap (dp, cur);
	XFreePixmap (dp, mask);

	XDefineCursor (dp, wi, cursor);

	XMapWindow(dp, wi);

	GetShmPixmap();
	XSetWindowBackgroundPixmap (dp, wi, pixmap);

	XSelectInput(dp, wi, KeyPressMask|KeyReleaseMask|ButtonPressMask|
		ButtonReleaseMask|PointerMotionMask|LeaveWindowMask|ExposureMask);
	atexit( Xuninitialize );
}


#endif /* __X11__ */
