DATA SEGMENT
VAL1	DW	4546h
VAL2	DB	'TEST'
VAL3	DB	'УКРАЇНА'
VAL4	DD	0D7856FDAh
EXP	EQU 5A4h
EXP1	EQU	1h
EXP2	EQU	0h
DATA ENDS
CODE SEGMENT
	IF EXP2
	Sti 
ELSE
	jne LABEL2
	Inc ax
	inc eax	
	inc dx
	inc bx
	inc al
	inc ah
	inc cl
	inc ch
	inc dl
	inc dh
	inc bl
	inc bh
	Dec BYTE PTR es:[bp+di+EXP1]
	ENDIF
	IF EXP1
	Sbb al, es:[bx+di +EXP1]
	Sbb bl, es:[ebp + esi +EXP1]
	Dec BYTE PTR [bx + si + EXP]
	Test es:[bp+si],ax
	Test ax, es:[bp+si]
	LABEL2:
ELSE
	Or ax, EXP1
	Sbb al, es:[bx+di +EXP1]
	Sbb bl, es:[ebp + esi +EXP1]
	Movsx ax, al
	movsx cx, al
	Movsx bx, al
	movsx dx, al
	movsx ax, cx
	movsx cx, cx
	movsx bx, cx
	movsx dx, cx
	Add byte ptr [EXP1+bp+si],EXP1
	Add byte ptr gs:[bp+di],EXP1
	Jne LABEL1
	ENDIF
CODE ENDS
end
