0000:	mov	#0200,-(R6)	Initialize Interrupt Vector
0004:	mov	#0052,*#0176	Initialize output buffer
0012:	mov	#0100,*#0174	Start up output
0020:	jsr	R5,0100		Reverse the string
0024:	mov	#050015,0074	Finish with CR/LF
0032:	mov	#0100,*#0174	Make sure that outputs
0040:	tstb	*0176		Wait for output to finish
0044:	bne	0040
0046:	halt	
0050:	====			Data to reverse
0100:	mov	R5,R1		String reversal program
0102:	add	#0024,R1
0106:	mov	(R1)+,R0
0110:	mov	R0,R2
0112:	add	R1,R2
0114:	movb	(R1)+,R4
0116:	movb	*R2,0011(R1)
0122:	movb	R4,0012(R2)
0126:	dec	R2
0130:	dec	R0
0132:	dec	R0
0134:	bge	0114
0136:	rts	R5
0140:	----			Empty space
0172:	halt			Output data register
0174:	halt			Output status register
0176:	halt			Output buffer pointer
0200:	movb	*0176,*#0172	Move data from buffer to output
0206:	beq	0224		If it was real data
0210:	mov	#0100,*#0174	   enable output
0216:	inc	0176		   and increment pointer
0222:	rti			else
0224:	clr	*#0174		   disable output
0230:	rti	
