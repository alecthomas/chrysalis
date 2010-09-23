// ============================================================================
//	Assembler routines for linux version of Chrysalis
// ============================================================================

.text

// Declare these symbols as external
.extern TBL_translucent

// Declare these symbols as globl
.global translucentbox
.global __blit4__
.global remove_accessed
.global PLAYER_copy_reversed_image 

.global calc_vectors
.global count_pixels
.global calc_points
.global morph_points_to_white
.global morph_points_switch
.global morph_points_from_white
.global dump_points

.global __fixed_div
.global __fixed_mul

/* Template for a function 
translucentbox:
#define target 8(%ebp)
#define width 12(%ebp)
#define height 16(%ebp)
#define colour 20(%ebp)
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %edx
	pushl %ebx
	mov edi, target
	mov ecx, width
	mov ebx, height
	mov eax, colour
	
	popl %ebx
	popl %edx
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret
*/

/* 
int remove_accessed( int index, int *list, int entries );
	Remove a reference to an index in the access history list
	modify [esi] \
	parm [eax] [edi] [ecx] \
	value [ecx]
*/
#define raindex 8(%ebp)
#define ralist 12(%ebp)
#define raentries 16(%ebp)
remove_accessed:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushf
	cld
	movl raindex, %eax
	movl ralist, %edi
	mov raentries, %ecx
	cmpl $0, %ecx
	je  raquit
	repne
	scasl
	jecxz  raquit
	movl %edi, %esi
	subl $4, %edi
	rep
	movsl
	incl %ecx
raquit:
	movl %ecx, %eax
	popf
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret

/*
void _blit4__( char *targ, char *src, int len );
	parm [edi] [esi] [ecx] \
	modify [eax ebx edx]
*/
#define b4targ 8(%ebp)
#define b4src 12(%ebp)
#define b4len 16(%ebp)
__blit4__:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushf

	cld

	movl b4targ, %edi
	movl b4src, %esi
	movl b4len, %ecx

	movl %ecx, %edx
	andl $3, %edx
	movl $0, %eax
	subl %eax, %eax
	xchgl %edi, %esi
	subl %edi, %esi
	shrl $2, %ecx
maskLOOP:
	repe
	scasl
	jz  exitblit
	subl $4, %edi
	incl %ecx
maskLOOP2:
	movl (%edi), %eax
	cmpl $0, %eax
	je  maskLOOP
	movl %edi, %ebx
	addl %esi, %ebx
	cmpb $0, %al
	je  s1
	movb %al, (%ebx)
s1:
	incl %ebx
	cmpb $0, %ah
	je  s2
	movb %ah, (%ebx)
s2:
	incl %ebx
	shrl $16, %eax
	cmpb $0, %al
	je  s3
	movb %al, (%ebx)
s3:
	cmpb $0, %ah
	je  s4
	incl %ebx
	movb %ah, (%ebx)
s4:
	addl $4, %edi
	decl %ecx
	jnz  maskLOOP2
exitblit:
	cmpl $0, %edx
	je  superexit
eloop:
	movb (%edi), %al
	cmpb $0, %al
	je  s5
	movb %al, (%edi,%esi)
s5:
	incl %edi
	decl %edx
	jnz  eloop
superexit:
	popf
	popl %ebx
	popl %edx
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret


/* 
int count_pixels( char *image, int size );
	Count the number of pixels in an image
	parm [esi] [ecx] \
	modify [eax ebx] \
	value [ebx]
*/
#define cpimage 8(%ebp)
#define cpsize 12(%ebp)
count_pixels:
	pushl %ebp
	mov %esp, %ebp
	pushl %esi
	pushl %ecx
	pushl %ebx

	movl cpimage, %esi
	movl cpsize, %ecx

	movl $0, %ebx
	subl $8, %ecx
	addl $8, %esi
countloop:
	movb (%esi), %al
	incl %esi
	cmpb $0, %al
	je  nocount
	incl %ebx
nocount:
	decl %ecx
	jnz  countloop
	movl %ebx, %eax
	popl %ebx
	popl %ecx
	popl %esi
	mov %ebp, %esp
	popl %ebp
	ret

/* 
void calc_points( MORPH::PIXEL *list, char *image, int w, int h );
	Calculate the (x,y,colour) points of every pixel in an image
	parm [edi] [esi] [ebx] [edx] \
	modify [eax ecx]
*/
#define cptslist 8(%ebp)
#define cptsimage 12(%ebp)
#define cptsw 16(%ebp)
#define cptsh 20(%ebp)
calc_points:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %edx
	pushl %ebx

	movl cptslist, %edi
	movl cptsimage, %esi
	movl cptsw, %ebx
	movl cptsh, %edx

	movl $0, %eax
	addl $8, %esi
cpyloop:
	pushl %edx
	movl $0, %edx
	movl $0, %ecx
cpxloop:
	movb (%esi), %dl
	cmpb $0, %dl
	je  xskip
	movl %ecx, (%edi)
	movl %eax, 4(%edi)
	movl %edx, 8(%edi)
	addl $12, %edi
xskip:
	incl %esi
	incl %ecx
	cmpl %ebx, %ecx
	jne  cpxloop
	popl %edx
	incl %eax
	cmpl %edx, %eax
	jne  cpyloop
finishedy:
	popl %ebx
	popl %edx
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret

/* 
void morph_points_to_white( MORPH::PIXEL *list, MORPH::PIXEL *list2, int length );
	Morph to it's final shape, while fading to white
	parm [edi] [esi] [ecx] \
	modify [eax]
*/
#define mptwlist 8(%ebp)
#define mptwlist2 12(%ebp)
#define mptwlength 16(%ebp)
morph_points_to_white:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	movl mptwlist, %edi
	movl mptwlist2, %esi
	movl mptwlength, %ecx
p2waddloop:
	movl (%esi), %eax
	addl %eax, (%edi)
	movl 4(%esi), %eax
	addl %eax, 4(%edi)
	movl 8(%edi), %eax
	andl $0x0f, %eax
	cmpl $0x0f, %eax
	je  skipcolour
	incl 8(%edi)
	movl 8(%edi), %eax
	andl $0x0f, %eax
	cmpl $0x0f, %eax
	je  skipcolour
	incl 8(%edi)
	movl 8(%edi), %eax
	andl $0x0f, %eax
	cmpl $0x0f, %eax
	je  skipcolour
 	incl 8(%edi)
skipcolour:
	addl $12, %edi
	addl $12, %esi
	decl %ecx
	jnz  p2waddloop
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret

/* 
void morph_points_switch( MORPH::PIXEL *list, MORPH::PIXEL *list2, MORPH::PIXEL *list3, int length );
	Morph to final shape while transferring to final image
	parm [edi] [esi] [ebx] [ecx] \
	modify [eax]
*/
#define mpswlist 8(%ebp)
#define mpswlist2 12(%ebp)
#define mpswlist3 16(%ebp)
#define mpswlength 20(%ebp)
morph_points_switch:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %ebx
	movl mpswlist, %edi
	movl mpswlist2, %esi
	movl mpswlist3, %ebx
	movl mpswlength, %ecx
psaddloop:
	movl (%esi), %eax
	addl %eax, (%edi)
	movl 4(%esi), %eax
	addl %eax, 4(%edi)
	movl 8(%ebx), %eax
	orl $0x0f, %eax
	movl %eax, 8(%edi)
	addl $12, %edi
	addl $12, %esi
	addl $12, %ebx
	decl %ecx
	jnz  psaddloop
	popl %ebx
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret

/* 
void morph_points_from_white( MORPH::PIXEL *list, MORPH::PIXEL *list2, MORPH::PIXEL *list3, int length );
	Morph to final shape and fade down to final shape colours
	parm [edi] [esi] [ebx] [ecx] \
	modify [eax]
*/
#define mpfwlist 8(%ebp)
#define mpfwlist2 12(%ebp)
#define mpfwlist3 16(%ebp)
#define mpfwlength 20(%ebp)
morph_points_from_white:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %ebx
	movl mpfwlist, %edi
	movl mpfwlist2, %esi
	movl mpfwlist3, %ebx
	movl mpfwlength, %ecx
pfwaddloop:
	movl (%esi), %eax
	addl %eax, (%edi)
	movl 4(%esi), %eax
	addl %eax, 4(%edi)
	movl 8(%edi), %eax
	cmpl 8(%ebx), %eax
	jbe  pfskipcolour
	decl 8(%edi) 
	movl 8(%edi), %eax
	cmpl 8(%ebx), %eax
	jbe  pfskipcolour
	decl 8(%edi)
	movl 8(%edi), %eax
	cmpl 8(%ebx), %eax
	jbe  pfskipcolour
	decl 8(%edi)
pfskipcolour:
	addl $12, %edi
	addl $12, %esi
	addl $12, %ebx
	decl %ecx
	jnz  pfwaddloop
	popl %ebx
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret

/* 
void dump_points( MORPH::PIXEL *ms, char **ytable, int length );
	Dump all points in an a MORPH to an image
	parm [esi] [ebx] [ecx] \
	modify [eax edi]
*/
#define dpms 8(%ebp)
#define dpytable 12(%ebp)
#define dplength 16(%ebp)
dump_points:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %ebx
	movl dpms, %esi
	movl dpytable, %ebx
	movl dplength, %ecx
dumploop:
	movl 4(%esi), %eax
	shrl $8, %eax
	movl (%ebx,%eax,4), %edi
	movl (%esi), %eax
	shrl $8, %eax
	addl %eax, %edi
	movl 8(%esi), %eax
	movb %al, (%edi)
	addl $12, %esi
	decl %ecx
	jnz  dumploop
	popl %ebx
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret

/* 
void calc_vectors( MORPH::PIXEL *list, MORPH::PIXEL *source, MORPH::PIXEL *target, MORPH::PIXEL *vector, int pixels );
	Calculate vectors for morph between two images
	parm [edi] [esi] [ebx] [edx] [ecx] \
	modify [eax]
*/
#define cvlist 8(%ebp)
#define cvsource 12(%ebp)
#define cvtarget 16(%ebp)
#define cvvector 20(%ebp)
#define cvpixels 24(%ebp)
calc_vectors:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl cvlist, %edi
	movl cvsource, %esi
	movl cvtarget, %ebx
	movl cvvector, %edx
	movl cvpixels, %ecx
vectorloop:
	movl (%esi), %eax
	shll $8, %eax
	movl %eax, (%edi)
	movl (%ebx), %eax
	subl (%esi), %eax
	shll $5, %eax
	movl %eax, (%edx)
	movl 4(%esi), %eax
	shll $8, %eax
	movl %eax, 4(%edi)
	movl 4(%ebx), %eax
	subl 4(%esi), %eax
	shll $5, %eax
	movl %eax, 4(%edx)
	movl 8(%esi), %eax
	movl %eax, 8(%edi)
	addl $12, %edi
	addl $12, %esi
	addl $12, %ebx
	addl $12, %edx
	decl %ecx
	jnz  vectorloop
	popl %ebx
	popl %edx
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret

/*
void translucentbox( char *ptr, int w, int h, int colour );
	modify [edx] \
	parm [edi] [ecx] [ebx] [eax]
*/
#define tbptr 8(%ebp)
#define tbw 12(%ebp)
#define tbh 16(%ebp)
#define tbcolour 20(%ebp)
translucentbox:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl tbptr, %edi
	movl tbw, %ecx
	movl tbh, %ebx
	movl tbcolour, %eax
	cmpl $0, %ecx
	je  tbquit
	cmpl $0, %ebx
	je  tbquit
	shll $8, %eax
	addl TBL_translucent, %eax
	movl %eax, %edx
	pushl %edi
	pushl %ecx
	incb %ah
	incb %ah
toprow:
	movb (%edi), %al
	movb (%eax), %al
	movb %al, (%edi)
	incl %edi
	decl %ecx
	jnz  toprow
	popl %ecx
	popl %edi
	movb %dh, %ah
	decl %ebx
tbyloop:
	pushl %edi
	pushl %ecx
	decl %ecx
	incb %ah
	incb %ah
tbxloop:
	movb (%edi), %al
	movb (%eax), %al
	movb %dh, %ah
	movb %al, (%edi)
	incl %edi
	decl %ecx
	jnz  tbxloop
	decb %ah
	decb %ah
	movb (%edi), %al
	movb (%eax), %al
	movb %al, (%edi)
	movb %dh, %ah
	popl %ecx
	popl %edi
	addl $320, %edi
	decl %ebx
	jnz  tbyloop
	decb %ah
	decb %ah
bottomrow:
	movb (%edi), %al
	movb (%eax), %al
	movb %al, (%edi)
	incl %edi
	decl %ecx
	jnz  bottomrow
tbquit:
	popl %ebx
	popl %edx
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret

/* 
void PLAYER_copy_reversed_image( char *source, char *target );
	This function copies a source image to a target image, flipping it
	horizontally
	modify [eax ebx ecx edx] \
	parm [esi] [edi]
*/
#define pcrisource 8(%ebp)
#define pcritarget 12(%ebp)
PLAYER_copy_reversed_image:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl pcrisource, %esi
	movl pcritarget, %edi
	movl (%esi), %edx
	movl 4(%esi), %ecx
	addl $8, %esi
	movl %edx, (%edi)
	movl %ecx, 4(%edi)
	addl $8, %edi
priyloop:
	pushl %ecx
	addl %edx, %edi
	pushl %edi
	decl %edi
	movl %edx, %ecx
prixloop:
	movb (%esi), %al
	movb %al, (%edi)
	decl %edi
	incl %esi
	decl %ecx
	jnz  prixloop
	popl %edi
	popl %ecx
	decl %ecx
	jnz  priyloop
	popl %ebx
	popl %edx
	popl %ecx
	popl %esi
	popl %edi
	mov %ebp, %esp
	popl %ebp
	ret

/*
int _fixed_div( int left, int right )
	parm [edx] [ebx] \
	value [eax]
*/
#define fmleft 8(%ebp)
#define fmright 12(%ebp)
__fixed_mul:
	pushl %ebp
	mov %esp, %ebp
	pushl %edx
	pushl %ebx

	movl fmleft, %eax
	movl fmright, %edx
	imul %edx
	shrd $16, %edx, %eax
	
	popl %ebx
	popl %edx
	mov %ebp, %esp
	popl %ebp
	ret

/*
int _fixed_div( int left, int right );
	parm [edx] [ebx] \
	value [eax]
*/
#define fdleft 8(%ebp)
#define fdright 12(%ebp)
__fixed_div:
	pushl %ebp
	mov %esp, %ebp
	pushl %edx
	pushl %ebx

	movl fdleft, %eax
	movl fdright, %edx
	movl $0, %eax
	shrdl $16, %edx, %eax
	sarl $16, %edx
	idivl %ebx

	popl %ebx
	popl %edx
	mov %ebp, %esp
	popl %ebp
	ret
.end
