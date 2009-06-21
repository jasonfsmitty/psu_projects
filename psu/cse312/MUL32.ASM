******************************************************************
*  A 32bit unsigned multiply subroutine
*
*  Following section of code multiply d1 (32bit) by d2 (32bit) and
*  produce product (64bit) in d2 (msb) and d1 (lsb), unsigned.
*  
*  Divide and conqure algorithm, using 16bit unsigned multiply
*  instruction of MC68000.
*
******************************************************************
mult32:		movem.l	d3/d4/d5,-(sp)	;A 32bit multiply subroutine.
		move.l	d1,d5		;Copy multiplier, multiplicand
		move.l	d1,d4		;  distribute them over
		swap	d4		;  multiple registers
		move.l	d2,d3		;  to generate partial
		swap	d2		;  products.

		mulu	d3,d1		;Generate 4 partial products.
		mulu	d2,d5
		mulu	d4,d3
		mulu	d4,d2

		move.l	d5,d4		;Split d5 into d5 and d4,
		swap	d5		;  add them to d1 and d2
		clr.w	d5
		clr.w	d4
		swap	d4
		add.l	d5,d1
		addx.l	d4,d2

		move.l	d3,d4		;Split d3 into d3 and d4,
		swap	d3		;  add them to d1 and d2
		clr.w	d3
		clr.w	d4
		swap	d4
		add.l	d3,d1
		addx.l	d4,d2
		movem.l	(sp)+,d3/d4/d5	;All done
		rts
******************************************************************
