0000:	mov	#0200,-(R6)	Initialize interrupt vector
0004:	mov	#0122,0176	Initalize output buffer at data
0012:	mov	#0100,0174	Start doing output now
0020:	mov	#0142,R5	Pointer for program results
0024:	mov	#0020,R4	Loop counter (for all data)
0030:	mov	#0017,R1	Where to start looking for data
0034:	movb	0122(R1),R2	Guess this is the smallest value
0040:	cmpb	R2,0122(R1)	
0044:	blt	0054		If we found a smaller one
0046:	mov	R1,R3		   remember where we saw it
0050:	movb	0122(R1),R2	   and remember new smallest value
0054:	dec	R1		Continue to look
0056:	bge	0040		   till done with the array
0060:	movb	R2,(R5)+	Copy smallest value to results
0062:	mov	#0175,R2	   and replace with something else
0066:	movb	R2,0122(R3)
0072:	dec	R4		Until everything copied/sorted
0074:	bgt	0030
0076:	mov	#050015,(R5)+	Output a carriage return / line feed
0102:	mov	#0100,0174	And make sure output is alive
0110:	mov	0176,R1
0114:	tstb	*R1		Loop until pointing at null end
0116:	bne	0110
0120:	halt	
0122:	----			Data Area and Empty Space
0172:	halt			Output data register
0174:	halt			Output status register
0176:	halt			Output buffer pointer
0200:	mov	R0,-(R6)	Save registers being used
0202:	mov	R1,-(R6)
0204:	mov	#0172,R1	Point at output data
0210:	mov	0176,R0		Fetch a character
0214:	movb	*R0,(R1)+	   and output it
0216:	beq	0234
0220:	inc	R1		if it really was a character
0222:	mov	#0100,*R1	   initiate output
0226:	inc	0176		   and advance the pointer
0232:	br 	0240
0234:	inc	R1		else
0236:	clr	*R1		   disable output
0240:	mov	(R6)+,R1
0242:	mov	(R6)+,R0	restore registers
0244:	rti			return from trap
