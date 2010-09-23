include data.hdr

TBL_vblit_     dd MOVBLIT
               dd MASKBLIT
               dd XORBLIT
               dd ORBLIT
               dd ANDBLIT
               dd TRANSLUCENTBLIT
               dd TINTBLIT
               dd TINTBACKBLIT
               dd SINGLEBLIT
               dd SOLIDBLIT
               dd FADEBLIT
               dd ALIASBLIT
               dd SHADEBLIT
			   dd TRANSLUCENTFADEBLIT

; Table of 256*256 bytes with colour matches
extrn _TBL_translucent : dword

; Interpolation table
;extrn _RGB_interpolatetable : dword

; Table of 32*256 bytes with colours to shade to
extrn _TBL_shade : dword
include data.ftr

public _V_blit

include code.hdr
   ;
   ; void V_blit( char *source, char *target, int len, int mode )
   ;
   ; Valid modes are:
   ;     MOV (moves the data straight there)
   ;     MASK (moves all but colour 0)
   ;     XOR, AND, OR, etc. (obvious)
   ;     TRANSLUCENT (uses the translucency table to get pictures)
   ;     TINT( C, F ) (tint Colour with image and Fades it)
   ;     TINTBACK( C, F ) (tint background)
   ;     SINGLE( C )
   ;     SOLID( C )
   ;     FADE( V )
   ;
_V_blit PROC NEAR

source EQU [ebp+8]
target EQU [ebp+12]
len    EQU [ebp+16]
mode   EQU [ebp+20]

    push ebp
    mov ebp, esp
    push esi
    push edi
    push ebx
    push ecx

    mov esi, source
    mov edi, target
    mov ecx, len
    mov ebx, mode
    shr ebx, 24
    dec ebx
    jmp dword ptr TBL_vblit_[ebx * 4]

MOVBLIT:
    shr ecx, 1
    jnc wordMOV
    movsb
wordMOV:
    shr ecx, 1
    jnc dwordMOV
    movsw
dwordMOV:
    rep movsd
    jmp exitblit

MASKBLIT:
    mov eax, 0
    xchg esi, edi
    sub esi, edi
maskLOOP:
    mov al, [edi]
    cmp al, 0
    je maskLOOP2
    mov [edi+esi], al
maskLOOP2:
    inc edi
    dec ecx
    jnz maskLOOP
    jmp exitblit


XORBLIT:
    mov al, [esi]
    cmp al, 0
    je skipXOR
    xor [edi], al
skipXOR:
    inc esi
    inc edi
    dec ecx
    jnz XORBLIT
    jmp exitblit

ORBLIT:
    mov al, [esi]
    cmp al, 0
    je skipOR
    or [edi], al
skipOR:
    inc esi
    inc edi
    dec ecx
    jnz ORBLIT
    jmp exitblit

ANDBLIT:
    mov al, [esi]
    cmp al, 0
    je skipAND
    and [edi], al
skipAND:
    inc esi
    inc edi
    dec ecx
    jnz ANDBLIT
    jmp exitblit

TRANSLUCENTBLIT:
    mov eax, _TBL_translucent
loopTRANS:
    mov ah, [esi]
    cmp ah, 0
    je skipTRANS
    mov al, [edi]
    mov al, [eax]
    mov [edi], al
skipTRANS:
    inc esi
    inc edi
    dec ecx
    jnz loopTRANS
    jmp exitblit

ALIASBLIT:
    mov ebx, _TBL_translucent
    mov eax, 0
loopALIAS:
    mov ax, [esi]
    cmp al, 0
    je zeroALIAS
    cmp ah, 0
    jne skiprightALIAS
    mov ah, [edi+1]
    mov al, [ebx+eax]
skiprightALIAS:
    mov ah, [esi-1]
    cmp ah, 0
    jne skipleftALIAS
    mov ah, [edi-1]
    mov al, [ebx+eax]
skipleftALIAS:
    mov [edi], al
zeroALIAS:
    inc esi
    inc edi
    dec ecx
    jnz loopALIAS
    jmp exitblit

TINTBLIT:
;    push edx
;    mov edx, RGB_interpolatetable_
;    mov eax, mode
;    and eax, 0ffh
;    mov edx, [edx+eax*4]
;    mov ebx, mode
;    shr ebx, 8
;    and ebx, 0ffh
;loopTINT:
;    movzx eax, BYTE PTR [esi]
;    shl eax, 3
;    add eax, ebx
;    mov al, [edx+eax]
;    cmp al, 0
;    je skipTINT
;    mov [edi], al
;skipTINT:
;    inc esi
;    inc edi
;    dec ecx
;    jnz loopTINT
;    pop edx
    jmp exitblit

TINTBACKBLIT:
;    push edx
;    mov edx, RGB_interpolatetable_
;    mov eax, mode
;    and eax, 0ffh
;    mov edx, [edx+eax*4]
;    mov ebx, mode
;    shr ebx, 8
;    and ebx, 0ffh
;loopTINTBACK:
;    movzx eax, BYTE PTR [esi]
;    cmp al, 0
;    je skipTINTBACK
;    mov al, [edi]
;    shl eax, 3
;    add eax, ebx
;    mov al, [edx+eax]
;    cmp al, 0
;    je skipTINTBACK
;    mov [edi], al
;skipTINTBACK:
;    inc esi
;    inc edi
;    dec ecx
;    jnz loopTINTBACK
;    pop edx
    jmp exitblit

SINGLEBLIT:
    mov eax, mode
singleLOOP:
    test BYTE PTR [esi], 0ffh
    jz singleSKIP
    mov [edi], al
singleSKIP:
    inc edi
    inc esi
    dec ecx
    jnz singleLOOP
    jmp exitblit

FADEBLIT:
    mov eax, mode
    and eax, 11111b
	cmp eax, 15
	je MASKBLIT
    xchg esi, edi
    shl eax, 8
    add eax, _TBL_shade
	mov al, 0
    sub esi, edi
fadeLOOP:
    mov al, [edi]
    cmp al, 0
    je fadeLOOP2
    mov al, [eax]
    mov [edi+esi], al
fadeLOOP2:
    inc edi
    dec ecx
    jnz fadeLOOP
    jmp exitblit

TRANSLUCENTFADEBLIT:
    mov ebx, mode
    and ebx, 11111b
	cmp ebx, 15
	je TRANSLUCENTBLIT
	push edx
	mov edx, _TBL_translucent
	mov eax, 0
    xchg esi, edi
    shl ebx, 8
    add ebx, _TBL_shade
    sub esi, edi
transfadeLOOP:
    mov dh, [edi]
    cmp dh, 0
    je transfadeLOOP2
	mov bl, [edi+esi]
	mov dl, [ebx]			; fade
    mov al, [edx]			; translucent
    mov [edi+esi], al
transfadeLOOP2:
    inc edi
    dec ecx
    jnz transfadeLOOP
exittransfade:
	pop edx
    jmp exitblit

SHADEBLIT:
    xchg esi, edi
    mov eax, mode
    and eax, 11111b
    shl eax, 8
    add eax, _TBL_shade
    sub esi, edi
shadeLOOP:
    mov al, [edi]
    cmp al, 0
    je shadeLOOP2
    mov al, [edi+esi]
    mov al, [eax]
    mov [edi+esi], al
shadeLOOP2:
    inc edi
    dec ecx
    jnz shadeLOOP
    jmp exitblit

;
; SOLIDBLIT SHOULD ALWAYS BE LAST, BECAUSE IT NEEDS TO BE REAL FAST
;
SOLIDBLIT:
    mov eax, mode
    mov ah, al
    mov bx, ax
    shl eax, 16
    mov ax, bx
solidLOOP:
    shr ecx, 1
    jnc nosolidB
    mov [edi], al
    inc edi
nosolidB:
    shr ecx, 1
    jnc nosolidW
    mov [edi], ax
    inc edi
    inc edi
nosolidW:
    rep stosd

exitblit:
    pop ecx
    pop ebx
    pop edi
    pop esi
    mov esp, ebp
    pop ebp
    retn
_V_blit ENDP

include code.ftr
