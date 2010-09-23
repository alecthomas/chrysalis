/* Copyright (c) 1996, Brian Spilsbury. All right reservered */
/* May be used with permission of author. */
/* This includes Forge Software */
#ifdef X_LOCAL
extern "C" {
static void GetShmPixmap ();
extern int XShmQueryExtension (Display * dpy);
static int haderror;
static int (*origErrorHandler) (Display *, XErrorEvent *);
};
#endif

/* global */
void Xinitialize (int width, int height);
void Xuninitialize ();
void setColor(int index, int r, int g, int b);
void updateScreen();
void updateKeyboard();
void initKeyboard();
int queryKeyboard(int ind);
char *queryKeylinear();
void initKeylinear();

#define xPixel(x,y) *(shared_mem+x+y*xWidth)

#ifdef X_LOCAL
int xWidth;		/* hmm, might as well be global */
int xHeight;
char *shared_mem;	/* <---- draw into this one... */
static char keymap[128];
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
#else
extern char *shared_mem;
extern int xWidth;
extern int xHeight;
#endif
