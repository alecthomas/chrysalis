#define __ASSEMBLY__

.text

TBL_vblit: 	.long	MOVBLIT, MASKBLIT, XORBLIT, ORBLIT, ANDBLIT, \
	TRANSLUCENTBLIT, TINTBLIT, TINTBACKBLIT, SINGLEBLIT, SOLIDBLIT, \
	FADEBLIT, ALIASBLIT, SHADEBLIT, TRANSLUCENTFADEBLIT

#ifdef __elf__
#define _TBL_translucent TBL_translucent
#define _TBL_shade TBL_shade
#define _V_blit V_blit
#endif
/* Table of 256*256 bytes with colour matches */
.extern _TBL_translucent

/* Table of 32*256 bytes with colours to shade to */
.extern _TBL_shade

.global _V_blit

/*
   void V_blit( char *source, char *target, int len, int mode )

    Valid modes are:
        MOV (moves the data straight there)
        MASK (moves all but colour 0)
        XOR, AND, OR, etc. (obvious)
        TRANSLUCENT (uses the translucency table to get pictures)
        TINT( C, F ) (tint Colour with image and Fades it)
        TINTBACK( C, F ) (tint background)
        SINGLE( C )
        SOLID( C )
        FADE( V )
*/

_V_blit: 

#define source 8(%ebp)
#define target 12(%ebp)
#define len 16(%ebp)
#define mode 20(%ebp)

    pushl %ebp
    movl %esp, %ebp
	pushl %esi
    pushl %edi
    pushl %ebx
    pushl %ecx
	pushf

/* reset direction flag */
	cld

    movl source, %esi
    movl target, %edi
    movl len, %ecx
    movl mode, %ebx
    shrl $24, %ebx
    decl %ebx
	shll $2, %ebx
	jmp TBL_vblit(%ebx)

MOVBLIT:
    shrl $1, %ecx
    jnc wordMOV
    movsb
wordMOV:
    shrl $1, %ecx
    jnc dwordMOV
    movsw
dwordMOV:
    rep
	movsl
    jmp exitblit

MASKBLIT:
	movb (%esi), %al
	cmpb $0, %al
	je maskLOOP
	movb %al, (%edi)
maskLOOP:
	incl %edi
	incl %esi
	decl %ecx
	jnz MASKBLIT
	jmp exitblit


XORBLIT:
    movb (%esi), %al
    cmpb $0, %al
    je skipXOR
    xorb %al, (%edi)
skipXOR:
    incl %esi
    incl %edi
    decl %ecx
    jnz XORBLIT
    jmp exitblit

ORBLIT:
    movb (%esi), %al
    cmpb $0, %al
    je skipOR
    orb %al, (%edi)
skipOR:
    incl %esi
    incl %edi
    decl %ecx
    jnz ORBLIT
    jmp exitblit

ANDBLIT:
    movb (%esi), %al
    cmpb $0, %al
    je skipAND
    andb %al, (%edi)
skipAND:
    incl %esi
    incl %edi
    decl %ecx
    jnz ANDBLIT
    jmp exitblit

TRANSLUCENTBLIT:
    movl _TBL_translucent, %eax
loopTRANS:
    movb (%esi), %al
    cmpb $0, %al
    je skipTRANS
    movb (%edi), %ah
    movb (%eax), %al
    movb %al, (%edi)
skipTRANS:
    incl %esi
    incl %edi
    decl %ecx
    jnz loopTRANS
    jmp exitblit

ALIASBLIT:
    movl _TBL_translucent, %ebx
    movl $0, %eax
loopALIAS:
    movl (%esi), %ax
    cmpb $0, %al
    je zeroALIAS
    cmpb $0, %ah
    jne skiprightALIAS
    movb 1(%edi), %ah
    movb (%ebx,%eax), %al
skiprightALIAS:
    movb -1(%esi), %ah
    cmpb $0, %ah
    jne skipleftALIAS
    movb -1(%edi), %ah
    movb (%ebx,%eax), %al
skipleftALIAS:
    movb %al, (%edi)
zeroALIAS:
    incl %esi
    incl %edi
    decl %ecx
    jnz loopALIAS
    jmp exitblit

TINTBLIT:
    jmp exitblit

TINTBACKBLIT:
    jmp exitblit

SINGLEBLIT:
    movl mode, %eax
singleLOOP:
    testb $0xff, (%esi)
    jz singleSKIP
    movb %al, (%edi)
singleSKIP:
    incl %edi
    incl %esi
    decl %ecx
    jnz singleLOOP
    jmp exitblit

FADEBLIT:
    movl mode, %eax
    andl $0x1f, %eax
	cmpl $15, %eax
	je MASKBLIT
    shll $8, %eax
    addl _TBL_shade, %eax
	movb $0, %al
fadeLOOP:
    movb (%esi), %al
    cmpb $0, %al
    je fadeLOOP2
    movb (%eax), %al
    movb %al, (%edi)
fadeLOOP2:
    incl %edi
	incl %esi
    decl %ecx
    jnz fadeLOOP
    jmp exitblit

TRANSLUCENTFADEBLIT:
    movl mode, %ebx
    andl $31, %ebx
	cmpl $15, %ebx
	je TRANSLUCENTBLIT
	pushl %edx
	movl _TBL_translucent, %edx
	movl $0, %eax
    shll $8, %ebx
    addl _TBL_shade, %ebx
transfadeLOOP:
    movb (%esi), %dh
    cmpb $0, %dh
    je transfadeLOOP2
	movb (%edi), %bl
	movb (%ebx), %dl
    movb (%edx), %al
    movb %al, (%edi)
transfadeLOOP2:
    incl %edi
	incl %esi
    decl %ecx
    jnz transfadeLOOP
exittransfade:
	popl %edx
    jmp exitblit

SHADEBLIT:
    movl mode, %eax
    andl $0x1f, %eax
    shll $8, %eax
    addl _TBL_shade, %eax
shadeLOOP:
    movb (%esi), %al
    cmpb $0, %al
    je shadeLOOP2
    movb (%edi), %al
    movb (%eax), %al
    movb %al, (%edi)
shadeLOOP2:
    incl %edi
	incl %esi
    decl %ecx
    jnz shadeLOOP
    jmp exitblit

/*
	SOLIDBLIT SHOULD ALWAYS BE LAST, BECAUSE IT NEEDS TO BE REAL FAST
*/
SOLIDBLIT:
    movl mode, %eax
    movb %al, %ah
    movw %ax, %bx
    shll $16, %eax
    movw %bx, %ax
solidLOOP:
    shrl $1, %ecx
    jnc nosolidB
    movb %al, (%edi)
    incl %edi
nosolidB:
    shrl $1, %ecx
    jnc nosolidW
    movw %ax, (%edi)
    incl %edi
    incl %edi
nosolidW:
    rep
	stosl

exitblit:
	popf
    popl %ecx
    popl %ebx
    popl %edi
    popl %esi
    movl %ebp, %esp
    popl %ebp
    ret

.end
