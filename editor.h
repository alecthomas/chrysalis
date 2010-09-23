#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <stdlib.h>
#include "data.h"
#include "queue.h"
#include "world.h"
#include "mouse.h"
#include "image.h"

#define ICON_LAYER		button[ 4 ][ 0 ]
#define ICON_SOLID		button[ 1 ][ 1 ]
#define ICON_BACK		button[ 2 ][ 1 ]
#define ICON_TYPE		button[ 2 ][ 0 ]
#define ICON_MODE		button [ 3 ][ 0 ]
#define ICON_PLACE		(button[ 3 ][ 0 ] == 0)
#define ICON_MARKED		(button[ 3 ][ 0 ] == 1)
#define ICON_EVENT		(button[ 3 ][ 0 ] == 2)
#define ICON_TRIGGER	button[ 3 ][ 1 ]
#define ICON_TRIGGERREGION	(button[ 3 ][ 1 ] == 0)
#define ICON_TRIGGERPROXIMITY (button[ 3 ][ 1 ] == 1)

#define EDITOR_LOAD     0
#define EDITOR_CANCEL   1
#define EDITOR_FLUSH    2
#define EDITOR_HELP     3
#define EDITOR_TYPE     4
#define EDITOR_PLANE    5
#define EDITOR_SAVE     6
#define EDITOR_SCENBACK 7
#define EDITOR_SCENFORW 8
#define EDITOR_SCENLAYE 9
#define EDITOR_SOLIDSCN 10
#define EDITOR_EVENT	11
#define EDITOR_MODE		12
#define EDITOR_ATTRIBMARKED	13


typedef struct {
	int x, y, activeatstart;
} INIWINDOW;

class WINDOW;

// Functions
int valid_object( int l );
void scroll( int direction );
void load_data();
void display_all();
void display_till_no_mouse();
void shadebox( int x, int y, int w, int h, int shade );
void fbox3D( int x, int y, int w, int h, int indent );
void display_world();
void display_to_video();
void display_windows();
void display_image( int x, int y );
int image_x( int x );
int image_y( int y );
int parse_INI_window_settings( INIWINDOW &windata, char *setting );
int parse_INI_command( char *command, char *setting );
int executable();
void title();
void untitle();
void reset_all();
OBJECT *insert_object( int id, int x, int y );

// Data
extern OBJECT::ATTRIBUTE attribute;
extern int objectfade;
extern VPAGESTRUCT *editorpage;
extern OBJECT *currento, *activeobject;
extern char *mouse;
extern int mousex, mousey, mousebutton, touched, onwindow, currentobject,
	button[ 10 ][ 2 ], handle_position, quit,
	eventx, eventy, eventw, eventh, mouseactive,
	display_boxes;
extern IMAGE *eimage;
extern WINDOW *loadwindow, *savewindow, *eventwindow, *markedsetwindow;
extern QUEUE <OBJECT> *marked, *undo;
extern QUEUE <WINDOW> *windows, *windowlist;
extern EVENT *activeevent;
extern INIWINDOW
	INI_icon,
	INI_information,
	INI_objectviewer,
	INI_scrollbar,
	INI_attribute;

int editor();
#endif
