#include "include.h"
#include "morph.h"

#ifdef __WATCOMC__
int count_pixels( char *image, int size );
/* ========================================
	Count the number of pixels in an image
*/
#pragma aux count_pixels = \
        "mov ebx, 0" \
        "sub ecx, 8" \
        "add esi, 8" \
    "countloop:" \
        "mov al, [esi]" \
        "inc esi" \
        "cmp al, 0" \
        "je nocount" \
        "inc ebx" \
    "nocount:" \
        "dec ecx" \
        "jnz countloop" \
    parm [esi] [ecx] \
    modify [eax ebx] \
    value [ebx]

void calc_points( MORPH::PIXEL *list, char *image, int w, int h );
/* ===========================================================================
	Calculate the (x,y,colour) points of every pixel in an image
*/
#pragma aux calc_points = \
        "mov eax, 0" /* y position */ \
        "add esi, 8" \
    "yloop:" \
        "push edx" \
        "mov edx, 0" \
        "mov ecx, 0" /* x position */ \
    "xloop:" \
        "mov dl, [esi]" \
        "cmp dl, 0" \
        "je xskip" \
        "mov [edi], ecx" \
        "mov [edi+4], eax" \
        "mov [edi+8], edx" \
        "add edi, 12" \
    "xskip:" \
        "inc esi" \
        "inc ecx" \
        "cmp ecx, ebx" \
        "jne xloop" \
    "finishedx:" \
        "pop edx" \
        "inc eax" \
        "cmp eax, edx" \
        "jne yloop" \
    "finishedy:" \
    parm [edi] [esi] [ebx] [edx] \
    modify [eax ecx]

void morph_points_to_white( MORPH::PIXEL *list, MORPH::PIXEL *list2, int length );
/* ======================================================================================================
	Morph to it's final shape, while fading to white
*/
#pragma aux morph_points_to_white = \
    "addloop:" \
        "mov eax, [esi]" \
        "add [edi], eax" \
        "mov eax, [esi+4]" \
        "add [edi+4], eax" \
        "mov eax, [edi+8]" \
        "and eax, 0fh" \
        "cmp eax, 0fh" \
        "je skipcolour" \
        "inc dword ptr [edi+8]" \
        "mov eax, [edi+8]" \
        "and eax, 0fh" \
        "cmp eax, 0fh" \
        "je skipcolour" \
        "inc dword ptr [edi+8]" \
        "mov eax, [edi+8]" \
        "and eax, 0fh" \
        "cmp eax, 0fh" \
        "je skipcolour" \
        "inc dword ptr [edi+8]" \
    "skipcolour:" \
        "add edi, 12" \
        "add esi, 12" \
        "dec ecx" \
        "jnz addloop" \
    parm [edi] [esi] [ecx] \
    modify [eax]

void morph_points_switch( MORPH::PIXEL *list, MORPH::PIXEL *list2, MORPH::PIXEL *list3, int length );
/* ====================================================================================================================================
	Morph to final shape while transferring to final image
*/
#pragma aux morph_points_switch = \
    "addloop:" \
        "mov eax, [esi]" \
        "add [edi], eax" \
        "mov eax, [esi+4]" \
        "add [edi+4], eax" \
        "mov eax, [ebx+8]" \
        "or eax, 0fh" \
        "mov [edi+8], eax" \
        "add edi, 12" \
        "add esi, 12" \
        "add ebx, 12" \
        "dec ecx" \
        "jnz addloop" \
    parm [edi] [esi] [ebx] [ecx] \
    modify [eax]

void morph_points_from_white( MORPH::PIXEL *list, MORPH::PIXEL *list2, MORPH::PIXEL *list3, int length );
/* ========================================================================================================================================
	Morph to final shape and fade down to final shape colours
*/
#pragma aux morph_points_from_white = \
    "addloop:" \
        "mov eax, [esi]" \
        "add [edi], eax" \
        "mov eax, [esi+4]" \
        "add [edi+4], eax" \
        "mov eax, [edi+8]" \
        "cmp eax, [ebx+8]" \
        "jbe skipcolour" \
        "dec dword ptr [edi+8]" \
        "mov eax, [edi+8]" \
        "cmp eax, [ebx+8]" \
        "jbe skipcolour" \
        "dec dword ptr [edi+8]" \
        "mov eax, [edi+8]" \
        "cmp eax, [ebx+8]" \
        "jbe skipcolour" \
        "dec dword ptr [edi+8]" \
    "skipcolour:" \
        "add edi, 12" \
        "add esi, 12" \
        "add ebx, 12" \
        "dec ecx" \
        "jnz addloop" \
    parm [edi] [esi] [ebx] [ecx] \
    modify [eax]

void dump_points( MORPH::PIXEL *ms, char **ytable, int length );
/* =========================================================================
	Dump all points in an a MORPH to an image
*/
#pragma aux dump_points = \
    "dumploop:" \
        "mov eax, [esi+4]" \
		"shr eax, 8" \
        "mov edi, [ebx+eax*4]" \
		"mov eax, [esi]" \
		"shr eax, 8" \
        "add edi, eax" \
        "mov eax, [esi+8]" \
        "mov [edi], al" \
        "add esi, 12" \
        "dec ecx" \
        "jnz dumploop" \
    parm [esi] [ebx] [ecx] \
    modify [eax edi]

void calc_vectors( MORPH::PIXEL *list, MORPH::PIXEL *source, MORPH::PIXEL *target, MORPH::PIXEL *vector, int pixels );
/* ====================================================================================================================
	Calculate vectors for morph between two images
*/
#pragma aux calc_vectors = \
	"vectorloop:" \
		"mov eax, [esi]" /* x point */ \
		"shl eax, 8" \
		"mov [edi], eax" \
		"mov eax, [ebx]" /* x vector */ \
		"sub eax, [esi]" \
		"shl eax, 5" \
		"mov [edx], eax" \
		"mov eax, [esi+4]" /* y point */ \
		"shl eax, 8" \
		"mov [edi+4], eax" \
		"mov eax, [ebx+4]" /* y vector */ \
		"sub eax, [esi+4]" \
		"shl eax, 5" \
		"mov [edx+4], eax" \
		"mov eax, [esi+8]" /* colour */ \
		"mov [edi+8], eax" \
		"add edi, 12" \
		"add esi, 12" \
		"add ebx, 12" \
		"add edx, 12" \
		"dec ecx" \
		"jnz vectorloop" \
	parm [edi] [esi] [ebx] [edx] [ecx] \
	modify [eax]
#endif

#ifdef __linux__
extern "C" {
void calc_vectors( MORPH::PIXEL *list, MORPH::PIXEL *source, MORPH::PIXEL *target, MORPH::PIXEL *vector, int pixels );
int count_pixels( char *image, int size );
void calc_points( MORPH::PIXEL *list, char *image, int w, int h );
void morph_points_to_white( MORPH::PIXEL *list, MORPH::PIXEL *list2, int length );
void morph_points_switch( MORPH::PIXEL *list, MORPH::PIXEL *list2, MORPH::PIXEL *list3, int length );
void morph_points_from_white( MORPH::PIXEL *list, MORPH::PIXEL *list2, MORPH::PIXEL *list3, int length );
void dump_points( MORPH::PIXEL *ms, char **ytable, int length );
}
#endif

MORPH::MORPH( char *source,  char *target )
/* ==================================================
    Set up morphing stuff ;)
*/
{
int newsize, sw, sh, tw, th, l, l1, l2, l3, curpixel;
char *sp, *tp, colour, *msource, *mtarget;

    step = 0;
    sw = ((int*)source)[ 0 ]; sh = ((int*)source)[ 1 ];
    tw = ((int*)target)[ 0 ]; th = ((int*)target)[ 1 ];
    if ( sw > tw ) w = sw; else w = tw;
    if ( sh > th ) h = sh; else h = th;
    newsize = w * h + 8;
    msource = new char[ newsize ];
    memset( msource, 0, newsize );
    ((int*)msource)[ 0 ] = w; ((int*)msource)[ 1 ] = h;
    mtarget = new char[ newsize ];
    memset( mtarget, 0, newsize );
    ((int*)mtarget)[ 0 ] = w; ((int*)mtarget)[ 1 ] = h;
    // resize images
    for ( l = 0; l != sh; l++ ) memcpy( msource + 8 + w * ( l + ( h - sh ) / 2 ) + ( w - sw ) / 2, source + 8 + sw * l, sw );
    for ( l = 0; l != th; l++ ) memcpy( mtarget + 8 + w * ( l + ( h - th ) / 2 ) + ( w - tw ) / 2, target + 8 + tw * l, tw );
    sourcepixels = count_pixels( msource, newsize );
    targetpixels = count_pixels( mtarget, newsize );
    if ( sourcepixels > targetpixels ) pixels = sourcepixels;
        else pixels = targetpixels;
    spoint = new PIXEL[ pixels ];
    tpoint = new PIXEL[ pixels ];
    point = new PIXEL[ pixels ];
    vector = new PIXEL[ pixels ];
    calc_points( spoint, msource, w, h );
    // propogate source points so they wrap at end
    memcpy( &spoint[ sourcepixels ], spoint,
        ( pixels - sourcepixels ) * sizeof( PIXEL ) );
    calc_points( tpoint, mtarget, w, h );
    // propogate target points so they wrap at end
    memcpy( &tpoint[ targetpixels ], tpoint,
        ( pixels - targetpixels ) * sizeof( PIXEL ) );
	// calculate point vectors
	calc_vectors( point, spoint, tpoint, vector, pixels );
    // create working images (scratch image)
    workimage = new char[ newsize ];
	((int*)workimage)[ 0 ] = w;
	((int*)workimage)[ 1 ] = h;
    ytable = new char*[ h ];
    for ( l = 0; l != h; l++ ) ytable[ l ] = workimage + 8 + l * w;
    size = newsize;
    delete msource;
    delete mtarget;
}

int MORPH::go()
/* =============
    Morph to next step
*/
{
int l, o;

	if ( step > 8 ) return FALSE;
    memset( workimage + 8, 0, size - 8 );
    if ( step < 4 )
        morph_points_to_white( point, vector, pixels );
	else
        if ( step == 4 )
            morph_points_switch( point, vector, tpoint, pixels );
		else
            morph_points_from_white( point, vector, tpoint, pixels );
    step++;
    if ( step > 8 ) return FALSE;
    dump_points( point, ytable, pixels );
    return TRUE;
}

MORPH::~MORPH()
/* =============
	Destructor for morph class
*/
{
	delete workimage;
	delete tpoint;
	delete spoint;
	delete point;
	delete ytable;
}
