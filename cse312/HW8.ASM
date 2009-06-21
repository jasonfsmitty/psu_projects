************************************************************************
*	Jason F Smith		CSE 312   Section 1
*	164-70-4588		Date: 3-21-2000
*	Homework #8  		32 bit calculator w/ infix to postfix translator.
*	Copywrite 2000 Jason F Smith
*
*	Program - Infix to postfix notation translator, and then calculate the answer.
*
*	Note: 	I realized that error checking was not required for this
*		project, but I added it in anyway.
************************************************************************
* System initialization parameters:
		org	0		; initial system stack pointer and
		dc.l	$10000,$1000	; initial program counter
		org	$10
		dc.l	$e00		; initialize "illegal" inst. vector
		org	$e00		; illegal instruction start here
goon:		nop			; illegal instruction definition:
		bra	goon		; //////// CPU idling ////////
		org	$70
		dc.l	$f00		; init. level 4 auto interrupt vector
		org	$f00
hoon:		nop			; level 4 interrupt occured
		bra	hoon		; <<< interrupt 4 occured >>>
		org	$80
		dc.l	init_duart	; TRAP#0: initialize serial port driver
		dc.l	key_ck_a	; TRAP#1: check keyboard
		dc.l	gchar_a		; TRAP#2: get character
		dc.l	pchar_a		; TRAP#3: put character
*************************************************************************
* M68681 Dual Serial Port constants
duart:		equ	$00020000	; M68681 Dual Serial Port base addr.
mr1a:		equ	1		; mode register 1
mr2a		equ	1		; mode register 2
sra:		equ	3		; status register
csra:		equ	3		; clock select register
cra:		equ	5		; command register
rba:		equ	7		; rx buffer register, serial in
tba:		equ	7		; tx buffer register, data out
ipcr:		equ	9		; input port change register
acr:		equ	9		; auxiliary control register
isr:		equ	11		; interrupt status register
imr:		equ	11		; interrupt mask register
mr1b:		equ	17		;  register for port B
mr2b:		equ	17		;  register for port B
srb:		equ	19		;  register for port B
csrb:		equ	19		;  register for port B
crb:		equ	21		;  register for port B
rbb:		equ	23		;  register for port B
tbb:		equ	23		;  register for port B
ivr:		equ	25		; ivterrupt vector register
txrdy:		equ	2
rxrdy:		equ	0
*-------------------------------------------------------------------
* Serial I/O routines
*-------------------------------------------------------------------	
* name: 	init_duart
* function:	M68681 Dual Serial Port initialize subroutine
* input: 	<none>
* output:	<none>
* registers:	A0
* 
		org	$400		; DUART init. program
init_duart:	lea	duart,a0
		move.b	#$80,acr(a0)	; select baud-rate set 1
		move.b	#$cc,csra(a0)	; set both rx and tx speed to 19.2k
		move.b	#$cc,csrb(a0)	; set both rx and tx speed to 19.2k
		move.b	#$02,mr1a(a0)	; set port A to 7 bit, even parity,
*                                        ;    1 stop bit, rxrts enabled.
		move.b	#$02,mr1b(a0)	; set port B to 7 bit, even parity,
*                                        ;    1 stop bit, rxrts enabled.
		move.b	#$07,mr2a(a0)	; select normal operation mode, enable
*					; txrtx, txcts, 1 stop bit
		move.b	#$07,mr2b(a0)	; select normal operation mode, enable
*					; txrtx, txcts, 1 stop bit
		move.b	#$05,cra(a0)	; enable port A tx and rx
		move.b	#$05,crb(a0)	; enable port B tx and rx
		move.b	#$00,imr(a0)	; disable interrupt
		move.b	#$01,ivr(a0)	; disable interrupt
		rte
********************************************************************
* name: 	pchar_a
* function:	transmit a byte of data to serial port A
* input: 	D0, data to send
* output:	<none>
* registers:	A0,D1
* 
pchar_a:	movem.l	d1/a0,-(sp)	; transmit a byte data in d0
		lea	duart,a0	;    to the serial port A.
opoll_a:	move.b	sra(a0),d1
		btst	#txrdy,d1
		beq	opoll_a
		move.b	d0,tba(a0)
		movem.l	(sp)+,d1/a0
		rte
********************************************************************
* name: 	gchar_a
* function:	get character from serial port A subroutine
* input: 	<none>
* output:	D0, received character
* registers:	A0,D0,D1
* 
gchar_a:	movem.l	d1/a0,-(sp)	; receive a byte date from the
		lea	duart,a0	;    serial port A to d0
ipoll_a:	move.b	sra(a0),d1
		btst	#rxrdy,d1
		beq	ipoll_a
		move.b	rba(a0),d0
		trap	#3		; echo print using trap#3
		cmp.b	#$0d,d0		; end of line check
		bne	e_gchar_a
		move.b	#$0a,d0		; print line-feed
		trap	#3		; pchar_a is trap#3
		move.b	#$0d,d0
e_gchar_a:	movem.l	(sp)+,d1/a0
		rte
********************************************************************
* name: 	key_ck_a
* function:	check serial port A for character subroutine
* input: 	<none>
* output:	D0, received character
* registers:	A0,D0,D1
* 
key_ck_a:	movem.l	d1/a0,-(sp)	; Check if any key pressed
		lea	duart,a0	;    serial port A to d0
		clr.l	d0		; d0=0 if no key pressed.
		move.b	sra(a0),d1
		btst	#rxrdy,d1
		beq	e_key_ck_a
		moveq	#1,d0		; d0=1 if a key pressed.
e_key_ck_a:	movem.l	(sp)+,d1/a0
		rte
******************************************************************************
*
		org	$8000
*
*   IF YOU NEED MEMORY SPACE, RESERVE THEM HERE.
*
line1:		dc.b	'     Runnning a 32-bit integer calculator program.'
		dc.b	$0a,$0d,'     To end, type "exit"',$0a,$0d
		dc.b	'     Note: <BACKSPACE> can be used to delete.',$0a,$0d,0

prompt:		dc.b	'Input > ',0
answer:		dc.b	'Answer: ',0

		ds.b	20	; stack for operators
input:		ds.b	51	; used to store the input (extra space for NULL)
output:		ds.b	60	; used to store the output of the preprocesor
expres:		ds.b	80	; the postfix notated expression
maxin:		dc.b	50	; maximum number of characters for input
error:		dc.l	0	; used for error checking in the in2pos subroutine
warning:	dc.b	'WARNING: Maximum number of input characters reached.  Input terminated.',$0a,$0d,0
yesError:	dc.b	'The input string contained the following error(s):',$0a,$0d,0
noError:	dc.b	'No errors found in the input string.',$0a,$0d,0
errorA:		dc.b	'     Input operand is too large (more than 10 characters).',$0a,$0d,0
errorB:		dc.b	'     Illegal operation: Multiple operators.',$0a,$0d,0
errorC:		dc.b	'     Expression is missing right parenthesis.',$0a,$0d,0
errorD:		dc.b	'     Expression is missing left parenthesis.',$0a,$0d,0
errorE:		dc.b	'     Operator without operand(s).',$0a,$0d,0
errorF:		dc.b	'     An unrecognized character was found.',$0a,$0d,0
errorG:		dc.b	'     Empty set of parenthesis was found.',$0a,$0d,0
		ds.b	20	; temporary stack for the in2pos translator
stack:		dc.b	0	; 'top' of stack
head2:		dc.b	'The output from the preprocessor was:',$0a,$0d,'     ',0
head3:		dc.b	'This program can also print the output from the'
		dc.b	' preprocessor.',$0a,$0d
		dc.b	'Would you like to see it? (Y/N) >> ',0
see_pre:	dc.b	0	; a byte to determine if pre proc is shown.


* data added for hw8, above is the exact same as hw7 data.

qstion:		dc.b	'Choose a format that you will be entering: Infix/Postfix (I/P) >> ',0
uansr:		dc.b	0		; used to store the answer of above question. 0-postfix, 1-infix
answrI:		dc.b	'Expression error checking is on.',$0a,$0d,0
answrP:		dc.b	'Error checking is off.',$0a,$0d,0
		ds.l	15		; stack used when calculating the answer.
cstack:		dc.l	0
		dc.b	0
exit:		dc.b	'exit',0
bye:		dc.b	$0a,$0d,'  Exiting...',$0a,$0d,0
result:		dc.l	0

cerrorA:	dc.b	' Error: Overflow during addition.',$0a,$0d,0
cerrorB:	dc.b	' Error: Overflow during subtraction.',$0a,$0d,0
cerrorC:	dc.b	' Error: Overflow during multiplication.',$0a,$0d,0
cerrorD:	dc.b	' Error: Operand overflow (too large/small).',$0a,$0d,0
cerrorE:	dc.b	' Error: Divide by zero.',$0a,$0d,0

cerrflg:	dc.b	0
******************************************************************************
*		
		org	$1000		; Main program start here
start:		trap	#0		; initialize serial port driver
* 
*   ENTER YOUR HW7 PROGRAM HERE!
* 
		lea	line1,a0	
		jsr	prt_str		; print the heading
		jsr	newline

		jsr	asktype		; find out if infix or postfix notation

loop:		jsr	newline
		lea	prompt,a0	; address of the prompt
		jsr	prt_str		;  go print it

		lea	input,a0	; set address to store the input
		jsr	read_in		; read in the input from the prompt
		jsr	ckexit		; check if the user typed exit, if so, the program is over

		lea	uansr,a1	; check for infix/postfix representation
		move.b	(a1),d1		
		cmp.b	#0,d1		; if zero, then in postfix
		beq	skippre		;   and the translator can be skipped

		jsr	in2pos		; jump to the infix to postfix translator.
		lea	error,a0	; lets check if there were any errors.
		move.w	(a0),d0		; check the flags
		cmp.b	#0,d0		; if d0=0, then there were no errors
		bne	loop		; if there were errors, the messages were already printed,
					* the expression means nothing, so get new input
	* no errors, so print out the answer.
skippre:	lea	answer,a0
		jsr	prt_str
		
		cmp.b	#0,d1		; check if infix/postfix notation
		beq	prtPostfx
		lea	expres,a0
		bra	answerPrt
prtPostfx:	lea	input,a0
answerPrt:	jsr	prt_str

		move.b	#' ',d0		; add in some spaces and an equal sign
		trap	#3
		move.b	#'=',d0
		trap	#3
		move.b	#' ',d0
		trap	#3

		jsr	calc		; go and calculate the answer
					* also prints it
		jsr	newline
		bra	loop		; do over again infinitely

		illegal			; Main program ends here...never reached

*
*   PUT YOUR SUBROUTINES HERE!
*
*---------------------------------------------------------------------------------
*	asktype - Asks the user if he/she will be using infix or postfix notation
*		  in their expressions.
*	Input:	None
*	Output:	d0 - 0 if user pick postfix notation, 1 if infix
*	Variables:
*		a0 - address of the header to print
*		a1 - address of the answer byte in memeory
*---------------------------------------------------------------------------------
asktype:	movem.l	a0/a1,-(sp)	; save the registers
		lea	qstion,a0
		jsr	prt_str
		lea	uansr,a1	; address of the answer byte
		
askloop:	trap	#1		; check if key pressed
		cmp.b	#0,d0		
		beq 	askloop		; poll the keyboard until a keypress
		trap	#2
		cmp.b	#'I',d0
		beq	askInfx
		cmp.b	#'i',d0
		beq	askInfx
		cmp.b	#'P',d0
		beq	askPstfx
		cmp.b	#'p',d0
		beq	askPstfx

		bra	askloop

askInfx:	move.b	#1,d0
		lea	answrI,a0
		bra	askend
askPstfx:	clr.b	d0
		lea	answrP,a0
askend:		jsr	newline
		jsr	prt_str
		move.b	d0,(a1)			; store the answer
		movem.l	(sp)+,a0/a1		; restore the registers
		rts
*---------------------------------------------------------------------------------
*	ckexit - test if the user typed in 'exit'
*	Input:	a0 - the address of the user's input
*	Output: None.  If the user entered 'exit' then goto Illegal, otherise, returns
*	Variables: a0 - address of input string
*		   a1 - address of 'exit' string
*---------------------------------------------------------------------------------
ckexit:		movem.l	a0/a1,-(sp)	; save address registers
		lea	exit,a1		; load exit string, 
		lea	input,a0
		
		cmpm.b	(a1)+,(a0)+	; test 'e'
		bne	noexit
		cmpm.b	(a1)+,(a0)+	; test 'x'
		bne	noexit
		cmpm.b	(a1)+,(a0)+	; test 'i'
		bne	noexit
		cmpm.b	(a1)+,(a0)+	; test 't'
		bne	noexit

		lea	bye,a0
		jsr	prt_str		; print 'exiting...'
		illegal			; only reached if exit was typed

noexit:		movem.l	(sp)+,a0/a1	; restore the registers
		rts
*---------------------------------------------------------------------------------
*	calc - calculates the answer to the expression
*	Input:	The postfix notated expression to be transalated.
*	Output:	Prints to the screen the decimal answer, also stored at 'result'
*	Variables:
*		a0 - address of the postfix notation
*		a1 - temporary stack pointer for the operands
*		a2 - address of the error flags
*		d0 - used to print the answer to terminal
*		d1 - first number off the stack
*		d2 - second number off the stack, also the answer in two operand ops
*		d3 - the error flags of this calculation
*		d4 - used temporarily in mult and div to keep track of sign
*	Note:  	The multiply and divide sections were made to calculate the answer
*		with unsigned numbers, and do error checking before adjusting for
*		the true sign of the answer.  This may make is seem (more) confusing
*		than it should be.
*
*---------------------------------------------------------------------------------
calc:		movem.l	d0/d1/d2/d3/d4/a0/a1/a2,-(sp)		; save registers
		lea	cstack,a1	; load the stack address
		lea	cerrflg,a2	; load the error flag address
		clr.l	d3		; clear error flags
	* determine the address of the input string
		lea	uansr,a0	; get answer to infix/postfix question
		move.b	(a0),d0
		cmp.b	#0,d0		; if d0==0, then user entered postfix notation
		bne	c_infix
		lea	input,a0
		bra	c_loop
c_infix:	lea	expres,a0
c_loop:	* check if there was an error on the previous process.
		cmp.b	#0,d3		; test if there were any errors
		beq	c_no_errs
		
		btst	#4,d3		; test for divide by zero
		bne	c_errors	; if set, then exit, can not continue.
					* otherwise, it's an overflow error, so keep on going...

c_no_errs:	move.b	(a0)+,d0	; get the first character from the stack
		cmp.b	#' ',d0		; check if a space
		beq	c_loop
		cmp.b	#'^',d0		; unary negation
		beq	c_nega
		cmp.b	#'*',d0		; check if multiply
		beq	c_mult
		cmp.b	#'/',d0		; division
		beq	c_div
		cmp.b	#'+',d0		; add
		beq	c_add
		cmp.b	#'-',d0		; subtract
		beq	c_sub

		cmp.b	#'0',d0
		blt	c_strng
		cmp.b	#'9',d0
		bgt	c_strng
*---------------------------------------------------------------------------------
	* it's a digit, so read it in and place on temp stack
		clr.l	d1
c_digit:	sub.b	#$30,d0		; sub ASCII offset
		move.l	d1,d2
		asl.l	#3,d1		;   	  d1 * 8
		asl.l	#1,d2		; 	+ d1 * 2
		add.l	d2,d1		; total=  d1 * 10
		bcs	toobig
c_slip:		add.l	d0,d1
		bvs	toobig
		move.b	(a0)+,d0
		cmp.b	#' ',d0		; test if a space
		bne	c_digit		; continue until a space in entered
		cmp.l	#0,d1		; test if the answer is still +
		blt	toobig
		move.l	d1,-(a1)
		bra	c_loop

toobig:		or.b	#8,d3		; set error flag
		bra	c_errors

*---------------------------------------------------------------------------------
* it was a strange character... for now just skip over it
c_strng:	cmp.b	#0,d0		; test if NULL was found
		beq	c_errors	; if so, check for overflows and leave
		bra	c_loop		; otherwise, skip it

*---------------------------------------------------------------------------------
*  unary negation operation
c_nega:		move.l	(a1)+,d1	; read off the top of the stack
		neg.l	d1		; negate the operand
		move.l	d1,-(a1)	; push the answer back on to the stack
		bra	c_loop		; continue

*---------------------------------------------------------------------------------
*  multiply
c_mult:		move.l	(a1)+,d2	; read off the second operand
		move.l	(a1)+,d1	; read off the first operand

		clr.l	d4		; clear the temp register
		cmp.l	#0,d1		; test if d1 is negative
		bge	cm_pos
		neg.l	d1
		move.l	#1,d4
cm_pos:		cmp.l	#0,d2		; test if d2 is negative
		bge	cm_2pos
		neg.l	d2
		add.l	#1,d4
	* both numbers are positive going into the multiply routine
cm_2pos:	jsr	mult32		; jump to subroutine to multiply
		cmp.l	#0,d2		; test out the MSBs of the answer
		beq	c_mtst
		or.b	#4,d3		; set error code
		bra	c_errors	; print the error
c_mtst:		cmp.l	#0,d1		; test the LSB of d1
		bge	c_mneg
		or.b	#4,d3
		bra	c_errors

c_mneg:		btst	#0,d4
		beq	cm_not
		neg.l	d1		; negate to the correct answer

cm_not:		move.l	d1,-(a1)	; push the answer back on stack
		bra	c_loop
*---------------------------------------------------------------------------------
* divide
c_div:		move.l	(a1)+,d2
		move.l	(a1)+,d1

		move.l	#0,d4		; clear temp register
		cmp.l	#0,d2		; test if d2 is zero
		bge	d_tst_nxt
		neg.l	d2		; negate if neccessary
		move.l	#1,d4		; set the temp to 1

d_tst_nxt:	cmp.l	#0,d1		; test if d1 is <0
		bge	dTstDone
		neg.l	d1		; negate d1
		add.l	#1,d4		; will either set or clear if a change of sign is needed

dTstDone:	jsr	div32		; divide two unsigned 32 bit numbers
		or.b	d2,d3		; set the error flag if needed.
		cmp.b	#0,d2		; test error code
		bne	c_div_done	; if not zero, then there was a zero divide

		btst	#0,d4		; test if both operands were the same sign
		beq	c_div_done	; if d4[0]==0, then both operands the same sign
		neg.l	d1		; negate the answer

c_div_done:	move.l	d1,-(a1)	; push the answer onto the stack
		bra	c_loop
*---------------------------------------------------------------------------------
* add
c_add:		move.l	(a1)+,d2	
		move.l	(a1)+,d1
		add.l	d1,d2
		bvs	e_add
c_aslip:	move.l	d2,-(a1)
		bra	c_loop
e_add:		or.b	#1,d3
		bra	c_aslip
*---------------------------------------------------------------------------------
* subtract
c_sub:		move.l	(a1)+,d2
		move.l	(a1)+,d1
		sub.l	d2,d1
		bvs	e_sub
c_sslip:	move.l	d1,-(a1)
		bra	c_loop
e_sub:		or.b	#2,d3
		bra	c_sslip
*---------------------------------------------------------------------------------
c_errors:	cmp.b	#0,d3		; test for errors
		beq	c_exit		; there were no errors, exit
	* at least one error
		jsr	newline
		btst	#0,d3		; test if it was error A
		beq	ctsterrB
		lea	cerrorA,a0
		jsr	prt_str
ctsterrB:	btst	#1,d3
		beq	ctsterrC
		lea	cerrorB,a0
		jsr	prt_str
ctsterrC:	btst	#2,d3
		beq	ctsterrD
		lea	cerrorC,a0
		jsr	prt_str
ctsterrD:	btst	#3,d3
		beq	ctsterrE
		lea	cerrorD,a0
		jsr	prt_str

ctsterrE:	btst	#4,d3
		beq	c_skdone
		lea	cerrorE,a0
		jsr	prt_str
		bra	c_skdone

c_exit:		move.b	d3,(a2)		; save error flags
	* print out the answer, which is on the stack.
	
		move.l	(a1)+,d2	; get the answer

		cmp.l	#0,d2
		bne	cc_skip
		move.b	#'0',d0
		trap	#3
		bra	c_skdone

cc_skip:	cmp.l	#0,d2		; test if 
		bge	c_pos		;  it needs a '-' sign
		move.b	#'-',d0
		trap	#3		; print the '-' sign
		neg.l	d2		; negate it so it's positive now
c_pos:		move.l	d2,d0		; get ready...
c_dlp:		cmp.l	#0,d0
		beq	c_done
		jsr	div10		; to divide by ten
		add.b	#$30,d1		; get the		
		move.b	d1,-(a1)
		bra	c_dlp

c_done:		movea.l	a1,a0
		jsr	prt_str
c_skdone:	movem.l	(sp)+,d0/d1/d2/d3/d4/a0/a1/a2		; restore the registers
		rts

*---------------------------------------------------------------------------------
*	checkpre - ask the user if he/she wants to see the output of
*	the preprocessor.
*	Input:	user enters Y or N on the keyboard.
*	Output: Stores a boolean value (0/1) in 'see_pre' so that we can test it
*		each loop to determine if we show the preprocessor output.
*	Variables:
*		d0 - used to read in from the keyboard (trap #3)
*		a0 - used to print the heading, and also to store the address
*		     of the boolean value after a Y or N has been read in.
*---------------------------------------------------------------------------------
checkpre:	movem.l	d0/a0,-(sp)	; save the registers.
		lea	head3,a0
		jsr	prt_str		; print the heading
		lea	see_pre,a0	; set variable address

chcklp:		trap	#1		; check the keyboard
		cmp.b	#0,d0		; test if no input
		beq	chcklp		;  if none, then wait
		trap	#2		; actually get the character
		cmp.b	#'n',d0		; test if a no was pressed
		beq	sayNo
		cmp.b	#'N',d0		; another test for no
		beq	sayNo
		cmp.b	#'Y',d0		; test if yes
		beq	sayYes
		cmp.b	#'y',d0
		beq	sayYes
		bra 	chcklp		; otherwise, keep looping
sayNo:		move.b	#0,(a0)		; store zero, don't see it.
		bra	chckend
sayYes:		move.b	#1,(a0)		; store a one, show it.
chckend:	movem.l	(sp)+,d0/a0	; restore registers.
		rts			; return
*---------------------------------------------------------------------------------
*	charread:	Reads in a character from the input string and sets it's type
*			for returning to the preprocessor
*	Input:	a0 - the address of the string, it is automatically incremented.
*	Output: a0 - the incremented address.
*		d0 - the character read in.
*		d1 - the type of the character read in.
*			1 - Digit for an operand (0-9)
*			2 - Operator (+,-,*,/)
*			3 - '('
*			4 - ')'
*			5 - Other, could be a '=' or <RETURN> or something else
*	Variables: Used just as indicated in Output.
*---------------------------------------------------------------------------------
charread:	move.b	(a0)+,d0	; get the character
		cmp.b	#')',d0 	; test if ')'
		beq	rightpar
		cmp.b	#'(',d0		; test if '('
		beq	leftpar	
		cmp.b	#'+',d0		; test if an operand...
		beq	op_sign
		cmp.b	#'-',d0		
		beq	op_sign
		cmp.b	#'*',d0
		beq	op_sign
		cmp.b	#'/',d0
		beq	op_sign
		cmp.b	#'0',d0		; check if it could be a digit
		blt	strange
		cmp.b	#'9',d0		; check upper bound
		bgt	strange
	* else it's a digit for an operand.
		move.b	#1,d1		; set the value
		bra	crd_done
leftpar:	move.b	#3,d1		; left paren. value
		bra	crd_done
rightpar:	move.b	#4,d1		; right paren
		bra	crd_done
op_sign:	move.b	#2,d1		; op sign
		bra	crd_done
strange:	move.b	5,d1		; strange char.
crd_done:	rts			; return 

*---------------------------------------------------------------------------------
*	prepros	- the preprocessor for the in2pos subroutine.
*	Input:	The string to be translated, at location input. (ending with a NULL)
*	Output: The translated string at location output. (ending with a NULL)
*	Varaiables:
*		a0 - address of the input string, at 'input'
*		a1 - address of the output string, at 'output'
*		a2 - address of 'error' word
*		d0 - the current character read in.
*		d1 - a value corresponding to a character read in.
*			0 - No character read in. 
*			1 - Digit for an operand.
*			2 - Operator
*			3 - '('
*			4 - ')'
*			5 - Other ( maybe a '=' or a <RETURN> )
*		d2 - error flags, moved into the byte location 'error'
*			0 - no errors.
*			1 - Operand too large (counted by digits, not foolproof)
*			2 - Multiple operators.  (2+*4)
*			4 - Missing right paren.
*			8 - Missing left paren.
*			16 - Operator without operand ( 2+ )
*			32 - Unrecognized character
*			64 - Empty parenthesis '()' 
*		d3 - a value corresponding to the previous character read in (see d1)
*		d4 - count of the number of '(', used in error checking
*		d5 - counter for number of digits of an operand.
*---------------------------------------------------------------------------------
preproc:	movem.l	d0/d1/d2/d3/d4/d5/a0/a1/a2,-(sp)	; save the registers.
		lea	input,a0	; set input address
		lea	output,a1	; set output address
		lea	error,a2
		move.l	#0,d3		; no character previously read in
		move.l	#0,d2		; clear the error flags.
		move.l	#0,d4		; clear number of '('

		jsr	charread	; get the first character
		cmp.b	#'-',d0		; test if a unary negation
		bne	precheck	; if not, continue normally
		move.b	#2,d1		; set previous character
		move.b	#'^',(a1)+	; 	store character
	* now continue on to the next character like normal
preloop:	move.b	d1,d3		; set previous character
preread:	jsr	charread	; get the next character

precheck:	cmp.b	#$20,d0		; check if it was a space
		beq	preread		; if so, ignore
		cmp.b	#0,d0		; check if NULL
		beq	predone
		cmp.b	#1,d1		; check if a digit
		beq	predigit	
		cmp.b	#2,d1		; check if an operator
		beq	pre_op
		cmp.b	#3,d1		; check if a '('
		beq	preleft
		cmp.b	#4,d1		; check if a ')'
		beq	prerite
	* otherwise it's a strange character, check if a <RETURN> or '=' or...
		cmp.b	#0,d0		; test if a NULL was found...
		beq	predone		;  if so, then we are done
		cmp.b	#13,d0		; check if a <RETURN>
		beq	predone		;  if so, we are done
		cmp.b	#'=',d0		; check if a '='
		beq	predone		;  if so, then we are done
	* strange character, one that should set an error flag...	
		or.w	#32,d2		; set error flags
		move.b	d0,(a1)+	; enter in character anyway
		bra	preloop		; go get next character and continue

predigit:	move.b	#0,d5		; set counter or MAX number of digits
		move.b	#1,d3		; set the previous type
		cmp.b	#4,d3		; check previous
		bne	dignxt		;   if not ')', then skip
		move.b	#'*',(a1)+	; add in a '*'

dignxt:		move.b	d0,(a1)+	; enter in character
	* There is no error checking because anything can come before a digit
		jsr	charread	; get the next character
		
		cmp.b	#$30,d0		; test if not a digit
		blt	digend
		cmp.b	#$39,d0		
		bgt	digend

		add.b	#1,d5		; increment digit counter
		cmp.b	#10,d5		; check number of digits...
		blt	dignxt		; if not over max, continue
		or.w	#1,d2		; set error condition
		bra	dignxt		; loop again until non-char

digend:		move.b	#1,d1		; set current digit
		move.w	#0,d5		; reset the digit counter for next time
		suba.l	#1,a0		; decrement the string pointere
		bra	preloop

pre_op:		move.b	d0,(a1)+	; always move the sign to output.
		cmp.b	#5,d3		; check previous=strange
		beq	preloop		; if so, skip error checking
		cmp.b	#4,d3		; check if previous was ')'
		beq	preloop		; if so, skip errors
		cmp.b	#1,d3		; check if previous was digit
		beq	preloop		; if so, go to next character

		cmp.b	#2,d3		; check if previous was operator
		bne	p_opnxt		
		or.w	#2,d2		; set multiple operators error
		bra	preloop		; next character please

p_opnxt:	cmp.b	#3,d3		; test if previous was '('
		bne	p_oplst
		cmp.b	#'-',d0		; test if current is a unary negation
		beq	prenega		;  if so, nothing wrong
		or.w	#16,d2		; set error of operator without operand
		bra	preloop		; continue on to next character
	* if it reaches here, then the previous was <NONE> as in it is the first character		
p_oplst:	or.w	#16,d2		; set error
		bra	preloop

prenega:	move.b	#'^',-(a1)	; change the minus sign to the unary negation
		adda.l	#1,a1		; increment the pointer back to were it should be
		bra	preloop

preleft:	add.b	#1,d4		; increment number of '('
		cmp.b	#0,d3		; test if previous was <NONE>
		beq	lftmove
		cmp.b	#2,d3		; test if previous was operator
		beq	lftmove
		cmp.b	#3,d3		; test if previous was another '('
		beq	lftmove
		cmp.b	#5,d3		; check if previous was some other character
	* this means it was either a digit or a ')', so a '*' is need to be added to string
		move.b	#'*',(a1)+	; add in a '*'
lftmove:	move.b	d0,(a1)+	; move the character to the output
	* no error checking here
		bra 	preloop		; go to the next character

prerite:	sub.b	#1,d4		; decrement number of '('
		move.b	d0,(a1)+	; always move to output
		cmp.b	#0,d4		; test if there are more ')' than '('
		bge	p_rtnxt		;   if there are still more '(', then skip error
		or.w	#8,d2		; set error flag
		move.b	#0,d4		; set to zero so that we can still get missing ')' errors later if needed.

p_rtnxt:	cmp.b	#1,d3		; check if previous was digit
		beq	preloop		;   if so, go to next
		cmp.b	#4,d3		; check if previous was another ')'
		beq	preloop		;   if so go to next
		cmp.b	#5,d3		; check if previous was strange
		beq	preloop		;   if so, go to next
		cmp.b	#0,d3		; check if this is the first character...
		beq	preloop		; ...error was set because d4 was less than 0.
	* otherwise, there was some type of error due to the previous character, set flags...
		cmp.b	#2,d3		; check if previous was an operator..
		bne	p_rtlst
		or.w	#16,d2		; set the flags
		bra	preloop		; go on to the next character
p_rtlst:	or.w	#64,d2		; empty '()'
		bra	preloop	

predone:  * check for errors....first is the number of '('....		
		cmp.b	#0,d4		; test number of '('
		ble	preleave	; zero is the correct amount
		or.w	#4,d2		; set to many '(' error code
					*  ... too many ')' is set in 'prerite:'
preleave:	cmp.b	#2,d3		; test if last was operator
		bne	preexit
		or.b	#16,d2		; set error for operator without operand

preexit:	move.w	d2,(a2)		; save the errors
		move.b	#0,(a1)		; enter a NULL into the string
		movem.l (sp)+,d0/d1/d2/d3/d4/d5/a0/a1/a2	 ; restore the registers
		rts
*---------------------------------------------------------------------------------
*	errorprt - prints any errors according to the error flags in 'error'
*	Input:	The input bit flags located in 'error'.
*	Output: d0 - zero if no errors, otherwise, means there was error(s).
*
*	Error Flag Bits
*	Bit#	Error Message
*	0 	Operand contains more than 10 characters.
*	1	Multiple operators (343*-223)
*	2	Missing ')'.
*	3	Missing '('.
*	4	Operator without operand ( 1234+).
*	5	Unrecognized character in the input string ( 1233j35).
*	6	Empty parenthesis.
*
*---------------------------------------------------------------------------------
errorprt:	movem.l	a0,-(sp)	; save the registers
		move.l	#0,d0		; clear d0
		lea	error,a0	; get error address
		move.w	(a0),d0		; get the error flags
		cmp.w	#0,d0		; test if there are no errors..
		beq	errorext	;   if not, leave

		lea	yesError,a0	; load the error message
		jsr	prt_str		; print the error message header
		asr.w	#1,d0		; test the #0 bit, for errorA
		bcc	tsterrB		; if C==0, then not error A
		lea	errorA,a0	; set error A address
		jsr	prt_str		; print the string
tsterrB:	asr.w	#1,d0		; test bit #1
		bcc	tsterrC		; if not set, then skip
		lea	errorB,a0
		jsr	prt_str
tsterrC:	asr.w	#1,d0
		bcc	tsterrD
		lea	errorC,a0
		jsr	prt_str
tsterrD:	asr.w	#1,d0
		bcc	tsterrE
		lea	errorD,a0
		jsr	prt_str
tsterrE:	asr.w	#1,d0
		bcc	tsterrF
		lea	errorE,a0
		jsr	prt_str
tsterrF:	asr.w	#1,d0
		bcc	tsterrG
		lea	errorF,a0
		jsr	prt_str
tsterrG:	asr.w	#1,d0
		bcc	errorlve
		lea	errorG,a0
		jsr	prt_str
		bra	errorlve

errorext:	lea	noError,a0	; load no error message
		jsr	prt_str		; print the no error message
errorlve:	lea	error,a0	; get ready to restore the return value
		move.w	(a0),d0		; set the return value
		movem.l	(sp)+,a0	; restore registers
		rts
*---------------------------------------------------------------------------------
*	in2pos - infix to postfix notation translator
*	Input:	The expression is stored in location 'input'.
*	Output: If there are any errors, they are printed to the terminal. 
*		Otherwise, the expression in postfix form is stored in location 'expres'.
*
*	Variables:	d0 - the current character.
*			d1 - the current type of character, see the preprocessor header
*			d2 - the character on top of the stack.
*			d3 - temporary variable for popping the stack
*			d4 - the type of the last character read in
*
*			a0 - the address of 'output', as in preprocessor output.
*			a1 - the address of 'expres'.
*			a2 - the temporary stack used to translate
*	
*---------------------------------------------------------------------------------
in2pos:		movem.l	d0/d1/d2/d3/a0/a1/a2,-(sp)	; save the registers.

		jsr	preproc		; jump to the preprocessor
		jsr	newline		
		jsr 	errorprt	; print any errors (if any)
		jsr	newline

	* The following lines were used for debugging----------------------
	* We check if the user wants to see them though
		lea	see_pre,a0	; get the variable address
		move.b	(a0),d2		; get the variable
		cmp.b	#0,d2		; test if not to show preproc
		beq	preskip

		lea	head2,a0
		jsr	prt_str		; print the header
		lea	output,a0	; load the preprocessor output
		jsr	prt_str		; print it
		jsr	newline		; print a newline
		jsr	newline
	* End of debuging lines---------------------------------------------

preskip:	cmp.w	#0,d0		; test the return value from errorprt, 
		bne	te_done		; if error(s) than we can not continue		
		lea	output,a0	; get the address of the the string
		lea	expres,a1	; get the address of where to store the translation.
		lea	stack,a2	; get the address of the stack.

translp:	move.b	(a2),d2		; set the character from top of the stack

		move.b	(a0)+,d0	; the the next character
		cmp.b	#$30,d0		; test if less than '0'
		blt	trans2
		cmp.b	#$39,d0		; test if greater than '9'
		bgt	trans2
		bra	transdgt	; otherwise, it's a digit

trans2:		cmp.b	#'(',d0		; check for highest priority
		beq	tr_lft		; if so, jump
		cmp.b	#')',d0		; test if a left paren
		beq	tr_rite		;   if so branch
		cmp.b	#'^',d0		; test if a negation sign
		beq	tr_neg		
		cmp.b	#'*',d0		; test if it's a '*'
		beq	tr_pri2		
		cmp.b	#'/',d0		; test if it's a '/'
		beq	tr_pri2		
		cmp.b	#'+',d0		; test if a plus sign
		beq	tr_pri1
		cmp.b	#'-',d0		; test if a minus sign
		beq	tr_pri1		
	
		cmp.b	#0,d0		; test if it's a NULL
		bra	t_end	;   if so, finished
	* testing for a minus sign was already done,... anything else should be an error
	* skip over it for now though
		bra	translp		; repeat, skip over an unrecognized characters
tr_lwr:	* this is for when the symbol on the stack is lower than that of the current one
		move.b	d0,-(a2)	; push the current onto the stack
		bra	translp

tr_lft:	* a left parenthesis, highest priority, so always pop into the stack
		move.b	d0,-(a2)	; move onto the stack
		bra	translp

tr_rite: * a right parenthesis, pop stack until a left one is reached
		move.b	(a2)+,d2	; get the first char from the stack
		cmp.b	#'(',d2		; check the top of the stack
		beq	translp		; if a '(', then we are done here
		cmp.b	#0,d2		; test if a NULL, it's an error check
		beq	t_end		;    if so, we are done

		move.b	d2,(a1)+	; otherwise, it's an operator that
		move.b	#$20,(a1)+	;   needs moved to the output, with a space
		bra	tr_rite		; loop again and test if we are done

tr_neg:	* unary negation
		cmp.b	#'^',d2		; check if the stack is a ^
		bne	tr_lwr		; if not, than it is lower pri, so store on stack
		move.b	(a2)+,(a1)+	; pop the stack to the output
		move.b	#$20,(a1)+	;  add in a space
		bra	tr_lwr		; now we can push the ^ onto the stack

tr_pri2: * multiply and divide
		cmp.b	#'-',(a2)	; test if a - is on top
		beq	tr_lwr		; it's of lower priority
		cmp.b	#'+',(a2)	; test if + is on top
		beq	tr_lwr		; it's of lower priority
		cmp.b	#0,(a2)		; test if nothing on stack
		beq	tr_lwr		;   if nothing, the definitily lower
	* otherwise, it means it's of higher or equal priority, so pop
		move.b	(a2)+,(a1)+	; pop to the output
		move.b	#$20,(a1)+	; add in a space
		bra	tr_pri2		; keep going until lower priority

tr_pri1: * add and subract
	* pop until either a '(' or a NULL if found on the stack
	*  reason - pop as long as the top is of equal or higher priority
	*     everything except the '(' has a equal or higher priority.
		cmp.b	#'(',(a2)	; test the top
		beq	tr_lwr		;   if a '(', then it is of lower pri
		cmp.b	#0,(a2)		; test if nothing on the stack
		beq	tr_lwr		; if not, lower priority
		move.b	(a2)+,(a1)+	; otherwise, we need to pop the operators
		move.b	#$20,(a1)+	; add in a space
		bra	tr_pri1		; keep on branching

transdgt:	move.b	d0,(a1)+	; move to the output
		jsr	charread	; get the next character
		cmp.b	#1,d1		; test if still a digit
		bne	trdgtend	; if not, finish operand.
		bra	transdgt	; it it is another digit, keep looping
trdgtend:	move.b	#$20,(a1)+	; send a space to the expression string
		move.b	#1,d4		; set previous type
		bra	trans2		; continue on..		

t_end:		cmp.b	#0,(a2)		; test the top of the stack
		beq	t_leave
		move.b	(a2)+,(a1)+	; send extra operators to output
		move.b	#$20,(a1)+	; add in a space
		bra	t_end		; keep on looping till done

t_leave:	move.b	#0,(a1)		; enter in a NULL
te_done:	movem.l	(sp)+,d0/d1/d2/d3/a0/a1/a2	restore the registers
		rts			; return from in2pos
*---------------------------------------------------------------------------------
*	prt_str - prints a string in the location pointed to by a0
*	Input:	a0 - the address of the string
*	Output:	The string printed on the terminal window.
*---------------------------------------------------------------------------------
prt_str:	movem.l	d0/a0,-(sp)
		
prt_lp:		move.b	(a0)+,d0	; get the character
		cmp.b	#0,d0		; test if NULL
		beq	prtdone		; if null, then done
		trap	#3		; print the character.
		bra	prt_lp		; loop until a null if found

prtdone:	movem.l	(sp)+,d0/a0
		rts

*---------------------------------------------------------------------------------
*	newline: print a newline to the screen.
*---------------------------------------------------------------------------------
newline:	movem.l	d0,-(sp)	; save d0
		move.b	#$0A,d0		; line feed
		trap	#3		; print
		move.b	#$0D,d0		; return
		trap	#3		; print
		movem.l	(sp)+,d0	; restore d0
		rts			; return
*---------------------------------------------------------------------------------
*	read_in: 	sub routine to read in expressions from the keyboard.
*	Input:	a0 - The address to store the character string.
*		maxin:	A byte size integer containing the maximun number of
*			characters that can be stored in the string.
*	Output: a0 - The string in ASCII form.  It is read until either a <RETURN>
*		     is pressed or the maximum number of characters is reached.
*	Variables:
*		a0 - the address of the string stored in memory
*		d0 - used to show the characters onto the screen.
*		d1 - the maximum number of characters that can be read in
*		d2 - the number of characters read in so far.
*	Note:	I made it so that the backspace button deletes the previous character
*---------------------------------------------------------------------------------
read_in:	movem.l	d0/d1/d2/a0/a1,-(sp)	; store the registers to the stack.
		lea	maxin,a1		; get the address of maxin
		move.b	(a1),d1 		; set the max num of char
		move.w	#0,d2			; set the number of char read in to zero

rd_loop:	trap	#1			; check keyboard
		cmp.b	#0,d0			; check if key pressed
		beq	rd_loop			; if not (do==0), loop

		trap	#2			; get the character
		cmp.b	#$0d,d0			; check if enter was pressed
		beq	rd_done			; if so, then all done

		cmp.b	#$08,d0			; check if backspace
		bne	rd_other		; if not, go process the character
		
		cmp.w	#0,d2			; make sure we have read some characters in
		bgt	rd_backsp		; if there are some characters, the process the backspace
		move.b	#$20,d0			; if not, we need to account for the backspace
		trap	#3			; moves the cursor back into position
		bra	rd_loop			; now continue on to next char

rd_backsp:	sub.l	#1,a0			; go back one space
		move.b	#0,(a0)			; clear the string of the old character
		move.b	#$20,d0			; get a space ready...
		trap	#3			;    and print it
		move.b	#$08,d0			; now move back to where the cursor should be..
		trap	#3			;    and print it to move.
		sub.b	#1,d2			; we can now take one more character
		bra	rd_loop			; now go get some more characters.

rd_other:	move.b	d0,(a0)+		; move the character into the string

rd_check:	add.b	#1,d2			; increment number of characters read in
		cmp.b	d1,d2			; test if maximum
		bne	rd_loop			; if not, we can take more characters
		move.b	#0,(a0)			; else, too many characters, end the string
		jsr	newline
		lea	warning,a0		; warn the user that they have reached the maximum
		jsr	prt_str			;  number of characters in the input string
		bra	rddone2

rd_done:	move.b	#0,(a0)			; enter a NULL into the string
rddone2:	movem.l (sp)+,d0/d1/d2/a0/a1	; restore registers
		rts				; return 

*---------------------------------------------------------------------------------
* Subrouting to divide two 32-bit numbers.
*	Input:	d1 - the dividend, I think 	(numerator)
*		d2 - the divisor (?)		(denominator)
*	Output: d1 - The answer ( d1/d2 )
*		d2 - If the denominator is zero, this is set to non-zero.
*		     Zero on valid operation.
*	Variables:
*		d1/d2 - numerator/denominator
*		d3 - temp register used to shift into
*		d4 - the result so far
*		d5 - 31 zeros, and one bit set.  Used for setting d4
*---------------------------------------------------------------------------------
div32:		movem.l	d3/d4/d5,-(sp)	; save registers
		clr.l	d5		; clear d5
		bset	#31,d5		;  and then set the bit
		clr.l	d4		; clear the answer
		clr.l	d3		; clear the temp register

		cmp.l	#0,d2		; test if divide by zero
		bne	div32Loop	; if not, continue
		move.l	#0,d1
		move.b	#16,d2		; set the error code
		bra	div_leave	; and leave the subroutine

div32Loop:	lsl.l	#1,d3		; shift left part of dividend
		lsl.l	#1,d1		; shift right part
		bcc	divSkip
		or.l	#1,d3		; set the first bit of d3
divSkip:	cmp.l	d2,d3		; check if we can divide this number
		bge	div_Yes		; if d3 greater than d2, we can subtract
set_loop:	lsr.l	#1,d5
		bcs	div_done	; if carry bit is set, then we are all done
		bra	div32Loop

div_Yes:	sub.l	d2,d3		; d3 = d3-d2
		or.l	d5,d4		; set the bit in the answer
		bra	set_loop	; go do the regular stuff

div_done	move.l	d4,d1
		move.l	#0,d2		

div_leave:	movem.l	(sp)+,d3/d4/d5	; restore registers
		rts

*---------------------------------------------------------------------------------
* Subroutine to divide 32 bit number by 10.
* Input:  32bit dividend in D0
* Output:  32bit quotion in D0 and remainder in D1
*          (Note: remainder is only 4 bit, why?)
* Algorithm:  Standard shift and subtract, repeated
*---------------------------------------------------------------------------------
div10:		movem.l	d2/d3,-(sp)
		move.l	d0,d2
		moveq	#0,d0		; Init. new quotion: D0.L
		moveq	#0,d1		; Init. remainder: D1.L
		rol.l	#3,d2		; Preshift 'dend by 'sor size -1.
		move.b	d2,d1		; Since 'sor is only 4 bits long, use
		and.b	#7,d1		;    D1.B as temporary 'dend holder.
		moveq	#28,d3		; Total 29 shift and subtract ops.
dvloop:		lsl.l	#1,d2		; Dividend shift one bit.
		roxl.b	#1,d1
		sub.b	#10,d1		; Subtract divisor from dividend.
		bcs	q0		; Check 'dend<'sor.
		ori.b	#$10,ccr	; If 'dend>='sor then set quotion bit.
		bra	q_update
q0:		add.b	#10,d1		; If 'dend<'sor then restore 'dend and
		andi.b	#0,ccr		;    clear quotion bit.
q_update:	roxl.l	#1,d0		; Include new quotion bit.
		dbf	d3,dvloop
		movem.l	(sp)+,d2/d3
		rts
*---------------------------------------------------------------------------------

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
		end			; end of file
