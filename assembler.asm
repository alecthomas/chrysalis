// ============================================================================
//	Assembler routines for linux version of Chrysalis
// ============================================================================

.text

// Declare these symbols as external
.extern _TBL_translucent

// Declare these symbols as globl
.globl _translucentbox
.globl ___blit4__
.globl _remove_accessed
.globl _PLAYER_copy_reversed_image 

.globl _calc_vectors
.globl _count_pixels
.globl _calc_points
.globl _morph_points_to_white
.globl _morph_points_switch
.globl _morph_points_from_white
.globl _dump_points

_translucentbox:
//
//#define target 8(ebp)
//#define width 12(ebp)
//#define height 16(ebp)
//#define colour 20(ebp)
//
//	pushl ebp
//	mov ebp, esp
//	pushl edi
//	pushl esi
//	pushl ecx
//	pushl edx
//	pushl ebx
//
//	mov edi, target
//	mov ecx, width
//	mov ebx, height
//	mov eax, colour
//
//	
//	popl ebx
//	popl edx
//	popl ecx
//	popl esi
//	popl edi
//	mov ebp, esp
//	popl ebp
//	ret

// 
//int remove_accessed( int index, int *list, int entries );
//	Remove a reference to an index in the access history list
//	modify [esi] \
//	parm [eax] [edi] [ecx] \
//	value [ecx]
//*/
_remove_accessed:
	cmp ecx, 0
	je raquit
	repne
	scasd
	jecxz raquit
	mov esi, edi
	sub edi, 4
	rep
	movsd
	inc ecx
raquit:

///*
//void __blit4__( char *targ, char *src, int len );
//	parm [edi] [esi] [ecx] \
//	modify [eax ebx edx]
//*/
___blit4__:
	mov edx, ecx
	and edx, 3
	mov eax, 0
	sub eax, eax
	xchg esi, edi
	sub esi, edi
	shr ecx, 2
maskLOOP:
	repe
	scasd
	jz exitblit
	sub edi, 4
	inc ecx
maskLOOP2:
	mov eax, [edi]
	cmp eax, 0
	je maskLOOP
	mov ebx, edi
	add ebx, esi
	cmp al, 0
	je s1
	mov [ebx], al
s1:
	inc ebx
	cmp ah, 0
	je s2
	mov [ebx], ah
s2:
	inc ebx
	shr eax, 16
	cmp al, 0
	je s3
	mov [ebx], al
s3:
	cmp ah, 0
	je s4
	inc ebx
	mov [ebx], ah
s4:
	add edi, 4
	dec ecx
	jnz maskLOOP2
exitblit:
	cmp edx, 0
	je superexit
eloop:
	mov al, [edi]
	cmp al, 0
	je s5
	mov [edi+esi], al
s5:
	inc edi
	dec edx
	jnz eloop
superexit:


///* 
//int count_pixels( char *image, int size );
//Count the number of pixels in an image
//parm [esi] [ecx] \
//modify [eax ebx] \
//value [ebx]
//*/
_count_pixels:
	mov ebx, 0
	sub ecx, 8
	add esi, 8
countloop:
	mov al, [esi]
	inc esi
	cmp al, 0
	je nocount
	inc ebx
nocount:
	dec ecx
	jnz countloop

///* 
//void calc_points( MORPH::PIXEL *list, char *image, int w, int h );
//Calculate the (x,y,colour) points of every pixel in an image
//parm [edi] [esi] [ebx] [edx] \
//modify [eax ecx]
//*/
_calc_points:
	mov eax, 0
	add esi, 8
cpyloop:
	push edx
	mov edx, 0
	mov ecx, 0
cpxloop:
	mov dl, [esi]
	cmp dl, 0
	je xskip
	mov [edi], ecx
	mov [edi+4], eax
	mov [edi+8], edx
	add edi, 12
xskip:
	inc esi
	inc ecx
	cmp ecx, ebx
	jne cpxloop
finishedx:
	pop edx
	inc eax
	cmp eax, edx
	jne cpyloop
finishedy:

///* 
//void morph_points_to_white( MORPH::PIXEL *list, MORPH::PIXEL *list2, int length );
//Morph to it's final shape, while fading to white
//parm [edi] [esi] [ecx] \
//modify [eax]
//*/
_morph_points_to_white:
p2waddloop:
	mov eax, [esi]
	add [edi], eax
	mov eax, [esi+4]
	add [edi+4], eax
	mov eax, [edi+8]
	and eax, 0x0f
	cmp eax, 0x0f
	je skipcolour
	inc dword ptr [edi+8]
	mov eax, [edi+8]
	and eax, 0x0f
	cmp eax, 0x0f
	je skipcolour
	inc dword ptr [edi+8]
	mov eax, [edi+8]
	and eax, 0x0f
	cmp eax, 0x0f
	je skipcolour
	inc dword ptr [edi+8]
skipcolour:
	add edi, 12
	add esi, 12
	dec ecx
	jnz p2waddloop

///* 
//void morph_points_switch( MORPH::PIXEL *list, MORPH::PIXEL *list2, MORPH::PIXEL *list3, int length );
//Morph to final shape while transferring to final image
//parm [edi] [esi] [ebx] [ecx] \
//modify [eax]
//*/
_morph_points_switch:
psaddloop:
	mov eax, [esi]
	add [edi], eax
	mov eax, [esi+4]
	add [edi+4], eax
	mov eax, [ebx+8]
	or eax, 0x0f
	mov [edi+8], eax
	add edi, 12
	add esi, 12
	add ebx, 12
	dec ecx
	jnz psaddloop

///* 
//void morph_points_from_white( MORPH::PIXEL *list, MORPH::PIXEL *list2, MORPH::PIXEL *list3, int length );
//Morph to final shape and fade down to final shape colours
//parm [edi] [esi] [ebx] [ecx] \
//modify [eax]
//*/
_morph_points_from_white:
pfwaddloop:
	mov eax, [esi]
	add [edi], eax
	mov eax, [esi+4]
	add [edi+4], eax
	mov eax, [edi+8]
	cmp eax, [ebx+8]
	jbe skipcolour
	dec dword ptr [edi+8]
	mov eax, [edi+8]
	cmp eax, [ebx+8]
	jbe skipcolour
	dec dword ptr [edi+8]
	mov eax, [edi+8]
	cmp eax, [ebx+8]
	jbe skipcolour
	dec dword ptr [edi+8]
skipcolour:
	add edi, 12
	add esi, 12
	add ebx, 12
	dec ecx
	jnz pfwaddloop

///* 
//void dump_points( MORPH::PIXEL *ms, char **ytable, int length );
//Dump all points in an a MORPH to an image
//parm [esi] [ebx] [ecx] \
//modify [eax edi]
//*/
_dump_points:
dumploop:
	mov eax, [esi+4]
	shr eax, 8
	mov edi, [ebx+eax*4]
	mov eax, [esi]
	shr eax, 8
	add edi, eax
	mov eax, [esi+8]
	mov [edi], al
	add esi, 12
	dec ecx
	jnz dumploop

///* 
//void calc_vectors( MORPH::PIXEL *list, MORPH::PIXEL *source, MORPH::PIXEL *target, MORPH::PIXEL *vector, int pixels );
//Calculate vectors for morph between two images
//parm [edi] [esi] [ebx] [edx] [ecx] \
//modify [eax]
//*/
_calc_vectors:
vectorloop:
	mov eax, [esi]
	shl eax, 8
	mov [edi], eax
	mov eax, [ebx]
	sub eax, [esi]
	shl eax, 5
	mov [edx], eax
	mov eax, [esi+4]
	shl eax, 8
	mov [edi+4], eax
	mov eax, [ebx+4]
	sub eax, [esi+4]
	shl eax, 5
	mov [edx+4], eax
	mov eax, [esi+8]
	mov [edi+8], eax
	add edi, 12
	add esi, 12
	add ebx, 12
	add edx, 12
	dec ecx
	jnz vectorloop

///*
//void translucentbox( char *ptr, int w, int h, int colour );
//modify [edx] \
//parm [edi] [ecx] [ebx] [eax]
//*/
_translucentbox:
	cmp ecx, 0
	je tbquit
	cmp ebx, 0
	je tbquit
	shl eax, 8
	add eax, _TBL_translucent
	mov edx, eax
	push edi
	push ecx
	inc ah
	inc ah
toprow:
	mov al, [edi]
	mov al, [eax]
	mov [edi], al
	inc edi
	dec ecx
	jnz toprow
	pop ecx
	pop edi
	mov ah, dh
	dec ebx
tbyloop:
	push edi
	push ecx
	dec ecx
	inc ah
	inc ah
tbxloop:
	mov al, [edi]
	mov al, [eax]
	mov ah, dh
	mov [edi], al
	inc edi
	dec ecx
	jnz tbxloop
	dec ah
	dec ah
	mov al, [edi]
	mov al, [eax]
	mov [edi], al
	mov ah, dh
	pop ecx
	pop edi
	add edi, 320
	dec ebx
	jnz tbyloop
	dec ah
	dec ah
bottomrow:
	mov al, [edi]
	mov al, [eax]
	mov [edi], al
	inc edi
	dec ecx
	jnz bottomrow
tbquit:

///* 
//void PLAYER_copy_reversed_image( char *source, char *target );
//This function copies a source image to a target image, flipping it
//horizontally
//modify [eax ebx ecx edx] \
//parm [esi] [edi]
//*/
_PLAYER_copy_reversed_image:
	mov edx, [esi]
	mov ecx, [esi+4]
	add esi, 8
	mov [edi], edx
	mov [edi+4], ecx
	add edi, 8
priyloop:
	push ecx
	add edi, edx
	push edi
	dec edi
	mov ecx, edx
prixloop:
	mov al, [esi]
	mov [edi], al
	dec edi
	inc esi
	dec ecx
	jnz prixloop
	pop edi
	pop ecx
	dec ecx
	jnz priyloop

