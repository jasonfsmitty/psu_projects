0000:	mov	#0200,-(R6)	Initialize Interrupt Vector
0004:	mov	#0176,R1	Initialize output buffer pointer
0010:	mov	#0062,*R1
0014:	mov	#0174,R1	Begin output
0020:	mov	#0100,*R1
0024:	jsr	R5,0110		Call subroutine to sort data
0030:	mov	#050015,0104	Finish with CR/LF
0036:	mov	#0174,R1
0042:	mov	#0100,*R1	And make sure it outputs
0046:	mov	0176,R1
0052:	tstb	*R1		Then wait for output to complete
0054:	bne	0046
0056:	halt	
0060:	----			Data for string reversal
0110:	mov	R5,R1		The subroutine is almost the same
0112:	add	#0030,R1
0116:	mov	(R1)+,R0
0120:	mov	R0,R2
0122:	add	R1,R2
0124:	movb	(R1)+,R4
0126:	movb	*R2,0011(R1)	Just put new data someplace else
0132:	movb	R4,0012(R2)
0136:	dec	R2
0140:	dec	R0
0142:	dec	R0
0144:	bge	0124
0146:	rts	R5
0150:	----			Empty space
0172:	halt			This is already described in sort.o
0174:	halt	
0176:	halt	
0200:	mov	R0,-(R6)
0202:	mov	R1,-(R6)
0204:	mov	#0172,R1
0210:	mov	0176,R0
0214:	movb	*R0,(R1)+
0216:	beq	0234
0220:	inc	R1
0222:	mov	#0100,*R1
0226:	inc	0176
0232:	br 	0240
0234:	inc	R1
0236:	clr	*R1
0240:	mov	(R6)+,R1
0242:	mov	(R6)+,R0
0244:	rti	