
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                              stdlib.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                       Forrest Yu, 2005
;														Edward		2013
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


%include "sconst.inc"


[SECTION .text]

; 导出函数
global	disp_str
global	disp_color_str
global	out_byte
global	in_byte
global	enable_irq
global	disable_irq
global	enable_int
global	disable_int
global	time
global 	read_sector


; ========================================================================
;                  void out_byte(t_port port, t_8 value);
; ========================================================================
out_byte:
	mov	edx, [esp + 4]		; port
	mov	al, [esp + 4 + 4]	; value
	out	dx, al
	nop	; 一点延迟
	nop
	ret

; ========================================================================
;                  t_8 in_byte(t_port port);
; ========================================================================
in_byte:
	mov	edx, [esp + 4]		; port
	xor	eax, eax
	in	al, dx
	nop	; 一点延迟
	nop
	ret

; ========================================================================
;                  void disable_irq(int irq);
; ========================================================================
; Disable an interrupt request line by setting an 8259 bit.
; Equivalent code for irq < 8:
;       out_byte(INT_CTLMASK, in_byte(INT_CTLMASK) | (1 << irq));
; Returns true iff the interrupt was not already disabled.
;
disable_irq:
	mov	ecx, [esp + 4]		; irq
	pushf
	cli
	mov	ah, 1
	rol	ah, cl			; ah = (1 << (irq % 8))
	cmp	cl, 8
	jae	disable_8		; disable irq >= 8 at the slave 8259
disable_0:
	in	al, INT_M_CTLMASK
	test	al, ah
	jnz	dis_already		; already disabled?
	or	al, ah
	out	INT_M_CTLMASK, al	; set bit at master 8259
	popf
	mov	eax, 1			; disabled by this function
	ret
disable_8:
	in	al, INT_S_CTLMASK
	test	al, ah
	jnz	dis_already		; already disabled?
	or	al, ah
	out	INT_S_CTLMASK, al	; set bit at slave 8259
	popf
	mov	eax, 1			; disabled by this function
	ret
dis_already:
	popf
	xor	eax, eax		; already disabled
	ret

; ========================================================================
;                  void enable_irq(int irq);
; ========================================================================
; Enable an interrupt request line by clearing an 8259 bit.
; Equivalent code:
;	if(irq < 8){
;		out_byte(INT_M_CTLMASK, in_byte(INT_M_CTLMASK) & ~(1 << irq));
;	}
;	else{
;		out_byte(INT_S_CTLMASK, in_byte(INT_S_CTLMASK) & ~(1 << irq));
;	}
;
enable_irq:
        mov	ecx, [esp + 4]		; irq
        pushf
        cli
        mov	ah, ~1
        rol	ah, cl			; ah = ~(1 << (irq % 8))
        cmp	cl, 8
        jae	enable_8		; enable irq >= 8 at the slave 8259
enable_0:
        in	al, INT_M_CTLMASK
        and	al, ah
        out	INT_M_CTLMASK, al	; clear bit at master 8259
        popf
        ret
enable_8:
        in	al, INT_S_CTLMASK
        and	al, ah
        out	INT_S_CTLMASK, al	; clear bit at slave 8259
        popf
        ret



; ========================================================================
;                  void disable_int();
; ========================================================================
disable_int:
	cli
	ret

; ========================================================================
;                  void enable_int();
; ========================================================================
enable_int:
	sti
	ret
; ========================================================================
;                  void time(char *bcd);
; ========================================================================
time:
	push ebx
	push edx
	push eax
	push ecx
	push edi

	mov	edi, [esp + 8]		; ebx 指向首地址
	xor edx, edx
	mov dx,	 0x70
	mov al,  0x0
	out dx,  al
	mov dx,  0x71
	in  al,  dx
	mov byte [edi], al
	inc edi
	mov dx,	 0x70
	mov al,  0x2
	out dx,  al
	mov dx,  0x71
	in  al,  dx
	mov byte [edi], al
	inc edi
	mov dx,	 0x70
	mov al,  0x4
	out dx,  al
	mov dx,  0x71
	in  al,  dx
	mov byte [edi], al
	inc edi
	mov dx,	 0x70
	mov al,  0x7
	out dx,  al
	mov dx,  0x71
	in  al,  dx
	mov byte [edi], al
	inc edi
	mov dx,	 0x70
	mov al,  0x8
	out dx,  al
	mov dx,  0x71
	in  al,  dx
	mov byte [edi], al
	inc edi
	mov dx,	 0x70
	mov al,  0x9
	out dx,  al
	mov dx,  0x71
	in  al,  dx
	mov byte [edi], al
	
	pop edi
	pop ecx
	pop eax
	pop edx
	pop ebx

	ret

	




