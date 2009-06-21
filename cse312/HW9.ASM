* Jason F. Smith    CSE 312     SS# 4588

*	ENTER YOUR TITLE BLOCK HERE


************************************************************************
* Prefix file to CSE 312 homework 9 program.
* V3.1  9Nov.98
*****************************************************************************
* System (VECTOR TABLE) initialization parameters:
		org	0		; initial system stack pointer and
		dc.l	$10000,$1000	; initial program counter
		org	$10
		dc.l	$e00		; initialize "illegal" inst. vector
		org	$70
		dc.l	$800		; init. level 4 auto interrupt vector
		org	$80
		dc.l	init_duart	; TRAP#0: initialize serial port driver
		dc.l	key_ck_a	; TRAP#1: check keyboard from Port A
		dc.l	gchar_a		; TRAP#2: get character from Port A
		dc.l	pchar_a		; TRAP#3: put character to Port A
		ds.l	1		; TRAP#4: not defined yet
		dc.l	gcha_bs		; TRAP#5: empty a character from Port B
		dc.l	gchar_b		; TRAP#6: get character from Port B
		dc.l	pchar_b		; TRAP#7: put character to Port B
*---------------------------------------------------------------------
		org	$e00		; illegal instruction start here
goon:		nop			; illegal instruction definition:
		bra	goon		; //////// CPU idling ////////
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
*                                       ;    1 stop bit, rxrdy enabled.
		move.b	#$02,mr1b(a0)	; set port B to 7 bit, even parity,
*                                       ;    1 stop bit, rxrdy enabled.
		move.b	#$07,mr2a(a0)	; select normal operation mode, disable
*					; txrtx, txcts, 1 stop bit
		move.b	#$07,mr2b(a0)	; select normal operation mode, disable
*					; txrtx, txcts, 1 stop bit
		move.b	#$05,cra(a0)	; enable port A tx and rx
		move.b	#$05,crb(a0)	; enable port B tx and rx
		move.b	#$20,imr(a0)	; enable rxrdy B interrupt and
*					;      disable other interrupt
		move.b	#28,ivr(a0)	; set interrupt vector same as auto vec.
		rte
********************************************************************
* name: 	pchar_b
* function:	transmit a byte of data to serial port B
* input: 	D0, data to send
* output:	<none>
* registers:	A0,D1
*
pchar_b:	movem.l	d1/a0,-(sp)	; transmit a byte data in d0
		lea	duart,a0	;    to the serial port B.
opoll_b:	move.b	srb(a0),d1
		btst	#txrdy,d1
		beq	opoll_b
		move.b	d0,tbb(a0)
		movem.l	(sp)+,d1/a0
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

gcha_bs
********************************************************************
* name: 	gcha_bs
* function:	get character from serial port B subroutine upon an
*		interrupt occurance.  Status check not necessary.
*		This is different from gchar_b.
*
* input: 	<none>
* output:	D0, received character
* registers:	A0,D0
*
gcha_bs:	movem.l	a0,-(sp)	; receive a byte date from the
		lea	duart,a0	;    serial port B to d0
		move.b	rbb(a0),d0	;Remove a byte from the port B.
		movem.l	(sp)+,a0
		rte

********************************************************************
* name: 	gchar_b
* function:	get character from serial port B subroutine
* input: 	<none>
* output:	D0, received character
* registers:	A0,D0,D1
*
gchar_b:	movem.l	d1/a0,-(sp)	; receive a byte date from the
		lea	duart,a0	;    serial port B to d0
ipoll_b:	move.b	srb(a0),d1	;Get the copy of the port B status.
		btst	#rxrdy,d1	;Check to see if a byte data came in.
		beq	ipoll_b		;If no, then loop, wait.
		move.b	rbb(a0),d0	;If Yes, remove a byte from the port B.
		trap	#3		; echo print using trap#3
		cmp.b	#$0d,d0		; end of line check
		bne	b_e_gchar
		move.b	#$0a,d0		; print line-feed
		trap	#3		; pchar_a is trap#3
		move.b	#$0d,d0
b_e_gchar:	movem.l	(sp)+,d1/a0
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
*****************************************************************************
*
		org	$800		; <<< interrupt 4 occured >>>
                movem.l	d0-d7/a0-a6,-(sp)
		trap	#5		; Upon port B receiving a byte
*	data, an interrupt was generated.  So, remove the data just received
*	to clear the interrupt, otherwise inturrupt is pending - causing
*	consecutive interrupt without break.  TRAP  #5 removes a byte from
*	the port B.  The data just received in D0 is useless in the clock
*	program, it only serves as generating an interrupt, no other purpose.
*
*   ENTER YOUR INTERRUPT LEVEL 4 SERVICE ROUTINE HERE!
*
	* check if the clock is turned on currently
		lea	clk_on,a0
		move.b	(a0),d0
		cmp.b	#0,d0
		beq	intQdone		; if not, quickly leave

		lea	save,a0
		jsr	prt_str
      		lea	atr_on,a0
      		jsr	prt_str			; save the attributes for the current cursor string
      		lea	movec,a0			; string to get to the clock postion
      		jsr	prt_str

	* update the clock

	   	lea	cclock,a1			; get the time
	     	move.b	(a1)+,d1		; get the seconds
		move.b	(a1)+,d2		; get the minutes
      		move.b	(a1)+,d3		; get the hours

		add.b	#1,d1		; increment one second
      		cmp.b	#60,d1	; test if one minute has been up
		blt	e_clock	; if not 60, than don't increment minutes/hours
      		clr.l	d1			; clear seconds to zero
      		add.b	#1,d2		; increment minutes
      		cmp.b	#60,d2
      		blt	e_clock	; if not 60 minutes, don't update hours
      		clr.l	d2
      		add.b	#1,d3		; increment hours
      		cmp.b	#24,d3	; check if day is done
      		blt	e_clock
      		clr.l	d3			; clear the hours

e_clock:	* time to print the current time, first have to translate it
		* first save the new time
		move.b	d3,-(a1)
		move.b	d2,-(a1)
		move.b	d1,-(a1)
		lea	sclock,a1	; get address of the string
      		adda.l	#8,a1		; offset to the actual location.
      		move.b	#0,d4		; turn off the leading zeros
		jsr	t_time
      		lea	sclock,a0
      		jsr	prt_str

********************************************************************
* check for the timer

		lea	tmr_on,a0
		move.b	(a0),d0
		cmp.b	#0,d0		; check if the timer is turned on
		beq	ckalarm	; if zero, it's off, so check the timer
		lea	ctimer,a0
		move.b	(a0)+,d1
		move.b	(a0)+,d2
		move.b	(a0)+,d3
	
	* subtract one second  ===========================================
		cmp.b	#0,d1	; check if the seconds are empty
		beq	tmrsecs
		sub.b	#1,d1
		bra	tmrprt
tmrsecs:	move.b	#59,d1
		cmp.b	#0,d2	; check if the minutes are emtpy
		beq	tmrmin
		sub.b	#1,d2	; otherwise decrement the minutes
		bra	tmrprt
tmrmin:		move.b	#59,d2	; set the number of minutes
		cmp.b	#0,d3	; check to make sure there wasn't an error
		beq	tmrerr
		sub.l	#1,d3	; otherwise subtract the amount from the hours
		bra	tmrprt
tmrerr:		clr.l	d1
		clr.l	d2
		clr.l	d3	; if error, turn the alarm off
	* print the timer ================================================
tmrprt:	* translate the timer into a string

	* after the update, we need to check that the timer isn't done yet
		cmp.b	#0,d1
		bne	tmprt2
		cmp.b	#0,d2
		bne	tmprt2
		cmp.b	#0,d3
		bne	tmprt2

	* else it's zero, and the alarm is going to go off
		lea	tmr_on,a0
		move.b	#0,(a0)		; turn the alarm off for future interrupts
	* emit an audible sound ( \a in c )
		move.b	#7,d0
		trap	#3		; set off a bell
	* print the message
		lea	movMsg,a0
		jsr	prt_str
		lea	tmrMsg,a0
		jsr	prt_str

	* actual printing of the timer string
tmprt2:		lea	stimer,a1
		adda.l	#8,a1		; offset to the correct part in string
		move.b	#1,d4		; turn leading zeros on
		jsr	t_time		; go and translate

		lea	movtmr,a0	; string to move to the timer
		jsr	prt_str		; move there
		lea	stimer,a0	; get the string of the timer
		jsr	prt_str
		
		lea	ctimer,a0
		move.b	d1,(a0)+
		move.b	d2,(a0)+
		move.b	d3,(a0)+	; save the new time

	* all done with the timer ---------------------------------------------

***************
* alarm stuff
ckalarm:	lea	alrm_on,a0
		cmpi.b	#0,(a0)		; check if alarm is off
		beq	intdone
	* otherwise, the alarm is ready to run.
		lea	calarm,a1	; memory of the alarm setting
		lea	cclock,a2	; the memory of the current time
	* to see if the alarm has gone off, check to see if it is the same
	* time as the current time
		cmpm.b	(a1)+,(a2)+	; check the seconds
		bne	intdone
		cmpm.b	(a1)+,(a2)+	; check the minutes
		bne	intdone
		cmpm.b	(a1)+,(a2)+	; check hours
		bne	intdone
	* at this point, the alarm is to go off, beep for the user
		move.b	#7,d0
		trap	#3
		move.b	#7,d0
		trap	#3		; do it twice to be different than the timer
		
		move.b	#0,(a0)		; turn the alarm off
		lea	movMsg,a0
		jsr	prt_str		; go to the message section

		lea	almMsg,a0
		jsr	prt_str

		
***************************************************************************
*	The following lines return the cursor back to it's position before the interrupt
intdone:	lea	atr_off,a0
		jsr	prt_str			; turn the regular character attributes back on
		lea	load,a0
		jsr	prt_str
intQdone:	movem.l	(sp)+,d0-d7/a0-a6
		rte			; <<< interrupt 4 occured >>>
*
******************************************************************************
*
		org	$8000
*
*   IF YOU NEED MEMORY SPACE, RESERVE THEM HERE.
*

* strings to mess with the terminal window.

scroll_msg:	dc.b	$1b,'[13;25r',0      	; set the scroll portion of screen
		ds.b	5
atr_on:		dc.b	$1b,$1b,'[1;7m',0    	; turn bold, reverse video on.
		ds.b	5			; buffer
atr_off:	dc.b	$1b,$1b,'[0m',0	   	; turn off char attributes
save:		dc.b	$1b,'7',0		; save current cursor position
load:		dc.b	$1b,'8',0		; reload the cursor settings
movec:		dc.b	$1b,$1b,'[4;32H',0		; move to the clock
		ds.b	2			; needed for tetris
movalm:		dc.b	$1b,$1b,'[5;32H',0		; move to the alarm
		ds.b	2			; needed for tetris
movtmr:		dc.b	$1b,$1b,'[6;32H',0		; move to the timer
		ds.b	2			; needed for tetris
movMsg:         dc.b	$1b,$1b,'[8;28H',0		; position output msg
		ds.b	2
gfx:		dc.b	$1b,$1b,'[13;0H',0		; graphics character set
xy_init:	dc.b	$1b,'[0;0H',0

tmrMsg:		dc.b	' The timer has expired.  ',0
almMsg:		dc.b	' The alarm has gone off. ',0

sclock:		dc.b	' Clock:               ',0
stimer:		dc.b	' Timer:               ',0
salarm:		dc.b	' Alarm:               ',0
cclock:		dc.b	0,0,0		; space for seconds, minutes and hours
ctimer:		dc.b    0,0,0		; space for the timer (countdown)
calarm:		dc.b	0,0,0		; space for the alarm
tmr_on:		dc.b	0
alrm_on:	dc.b	0		; bytes to determine if the timer/alarm is on or off
clk_on:		dc.b	1		; is the clock on or off?

ihdr1:		dc.b	$1b,'[1m'  		; turn boldface on
		dc.b	' Clock program using level 4 interrupts,'
		dc.b	' by Jason F Smith.  Choose your option'
		dc.b	$0a,$0d
		dc.b	'  a - set alarm         c - set timer    s - set time'
		dc.b	$0a,$0d
		dc.b	'  p - run calculator    t - tetris       q - quit'
		dc.b	$0a,$0d,$0a,$0d
		dc.b	' >> '
		dc.b	$1b,'[0m',0		; turn attributes off
ihdr2:		dc.b	' Enter the time (hh:mm:ss): ',0
ihdr3:		dc.b	' Invalid input, make sure you enter as hh:mm:ss.',$0a,$0d,0
ihdr4:		dc.b	' The clock could not be set.',$0a,$0d,0
ihdr5:		dc.b	' The clock has been set.',$0a,$0d,0
ihdr6:		dc.b	' The timer has been set.',$0a,$0d,0
ihdr7:		dc.b	' The timer could not be set.',$0a,$0d,0
ihdr8:		dc.b	' The alarm has been set.',$0a,$0d,0
ihdr9:		dc.b	' The alarm could not be set.',$0a,$0d,0
empty:		dc.b	'                      ',0		; used to erase the output
************************************************************************
*  The following data space is for the calculator of HW #8
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
*  Data needed for Tetris game
screen:		dc.b	$1b,'[1m'			; bold face
		dc.b	$1b,'[1;1H'				; move to (1,1) on screen
		dc.b	' +----------+ ',$0a,$0d	; top of screen
field:		dc.b	' |          | ',$0a,$0d	; top line
		dc.b	' |          | ',$0a,$0d	; 19th line...
		dc.b	' |          | ',$0a,$0d	;**************************************
		dc.b	' |          | ',$0a,$0d	; The field is arranged so that
		dc.b	' |          | ',$0a,$0d	; the origin is at (0,0), which
		dc.b	' |          | ',$0a,$0d	; is at the top left.
		dc.b	' |          | ',$0a,$0d	; the max is (9,19) at bottom right
		dc.b	' |          | ',$0a,$0d	;**************************************
		dc.b	' |          | ',$0a,$0d
		dc.b	' |          | ',$0a,$0d
		dc.b	' |          | ',$0a,$0d	; 10th line
		dc.b	' |          | ',$0a,$0d
		dc.b	' |          | ',$0a,$0d
		dc.b	' |          | ',$0a,$0d
		dc.b	' |          |    < - move left',$0a,$0d
		dc.b	' |          |    > - move right',$0a,$0d	; 5th line
		dc.b	' |          |    / - move down',$0a,$0d
		dc.b	' |          |    z - rotate left',$0a,$0d
		dc.b	' |          |    x - rotate right',$0a,$0d
		dc.b	' |          |    ESC/1 - quit',$0a,$0d	; 1st line
		dc.b	' +----------+ ',$0a,$0d		; bottom of screen
		dc.b	'    TETRIS    ',0 
i_type:		ds.b	1		; data for the current item
*	Type:	$01 - Block
*		$02 - Long
*		$03 - S
*		$04 - rev S
*		$05 - L
*		$06 - rev L
*		$07 - T

i_pos:		ds.b	1
*	Postion:
*		$01 - up (original)
*		$02 - left
*		$04 - down
*		$08 - right

i_xy:		ds.w	1	; the x,y of the item, used to plot the item
i_pts:		ds.w	4	; the x,y of each 'block' of the item
t_pts:		ds.w	4	; temp points for moving/dropping
t_xy:		ds.w	1
prevsecs:	ds.b	1

setscroll:	dc.b	$1b,'[1;25r',0	; back to the complete screen
		ds.b	5
nmovC:		dc.b	$1b,$1b,'[10;35H',0		; new position for clock
		ds.b	2			; buffer
nmovA:		dc.b	$1b,$1b,'[11;35H',0		; new pos for alarm
		ds.b	2			; buffer
nmovT:		dc.b	$1b,$1b,'[12;35H',0		; new position for timer
		ds.b	2			; needed as a buffer
nmovMsg:	dc.b	$1b,$1b,'[14;30H',0		; new postion for the message output
		ds.b	2
movBack:	dc.b	$1b,$1b,'[22;15H',0

Msgaaa:	dc.b	' Initializing Tetris by Jason F Smith....',$0a,$0d,0
Msgaab:	dc.b	$1b,'[2J',0		; erase the entire screen
newattr:	dc.b	$1b,'[0m',0
		ds.b	7
new_off:	dc.b	$1b,$1b,'[1m',0
		ds.b	7
field2:		ds.b	320	; 20 rows * 10 columns,... 16 bytes per row to assist in jumping around
e_field2:	ds.b	1
* raw data, supposedly random, but I just typed them all in, close enough I guess
data:		dc.b	3,7,5,1,2,5,7,6,3,4,4,1,5,2,7,6,4,2,3,1,5,6,7,3,1,4,2,6,6,4,7,6,1,3,2,4,7
		dc.b	1,1,5,3,4,7,6,2,7,1,4,3,5,6,3,2,4,5,1,1,7,1,3,2,5,3,1,2,5,4,3,2,6,1,5,3,7
		dc.b	4,6,2,7,3,4,2,1,5,3,6,4,5,7,3,5,1,2,3,6,7,1,2,3,6,1,3,4,3,5,2,6,4,7,1,1,2
		dc.b	7,4,5,3,6,1,7,2,5,3,5,4,7,6,5,3,5,1,2,7,4,6,1,7,3,3,4,5,2,1,5,6,1,7,3,5,4
		dc.b	3,7,2,5,3,7,3,4,2,1,4,3,1,3,2,6,4,7,2,5,3,7,2,4,1,5,3,4,1,2,6,7,2,1,4,1,4
		dc.b	2,6,7,1,4,2,5,4,1,6,7,5,4,2,3,1,6,7,3,4,2,5,6,7,7,1,1,4,2,3,5,1,3,4,2,5,5
enddata:	dc.b	2
g_xy:		dc.b	$1b,'['
x_y:		dc.b	10
*****************************************************************************
*
		org	$1000		; Main program start here
start:		trap	#0		; initialize serial port driver
*
*   ENTER YOUR HW9 PROGRAM HERE!
*
CLOCK:		nop			; This is interrupt driven CLOCK!
     * initialize the clock and screen
		lea	atr_on,a0
		jsr	prt_str
		
		clr.l	d1
		move.b	#' ',d0
		move.w	#960,d1		; 80*12=960 white spaces
		bra	spTst
spLp:		trap	#3
spTst:		dbra	d1,spLp
		lea	atr_off,a0
		jsr	prt_str

      		lea   	scroll_msg,a0
      		jsr	prt_str		; set the scroll mode on

* The following three lines are necessary to start 1 sec. timing.
* Execute when ready to tick.
		move.w	#$2000,sr	; Enable interrupt, all levels.
		move.b	#10,D0		; Command the 1sec timer in port B
		trap	#7		;   to start.  TRAP #7 writes to

		clr.l	d0
		lea	gfx,a0		; goto the correct cursor position
		jsr	prt_str

main:	* print the header for the user to choose.
		lea	ihdr1,a0
		jsr	prt_str
	* get the users input and process it
mn_poll:	trap	#1		; check for input
		cmp.b	#0,d0
		beq	mn_poll		; wait until input

		trap	#2		; actually get the input, now process
		
		cmp.b	#'a',d0		; check for alarm
		beq	inAlarm
		cmp.b	#'c',d0		; check for countdown timer
		beq	inTimer	
		cmp.b	#'p',d0		; check for calculator
		beq	inCalc
		cmp.b	#'s',d0		; set the clock
		beq	inClock
		cmp.b	#'q',d0
		beq	inQuit
		cmp.b	#'1',d0
		beq	inQuit
		cmp.b	#'t',d0
		beq	plytetris

		bra	mn_poll		; if not a valid input, keep waiting

plytetris:	jsr	tetris
		bra	main

inAlarm:	jsr	rd_time		; read in the time
		cmp.b	#0,d0		; check if invalid input
		beq	AlmNoGd

		lea	salarm,a1	; get addresss of the string
		adda.l	#8,a1		; offset to the alarm time
		clr.l	d4		; turn leading zeros off
		jsr	t_time		; translate to a string
	* The following lines are a 'mini' critical section, for several reasons
	* If the interrupt hits while in here, several things may happen,
	* 1 - the save and restore of the cursor could get messed up
	* 2 - if an interrupt occurs while the alarm time is being saved, the
	*     alarm could be updated with incorrect amounts in memory (correct 
	*     seconds, but not hours and minutes for example )

		move.w	#$2700,sr		; turn interrupts off briefly

		lea	calarm,a1	; get address of the actual alarm in memory
		move.b	d1,(a1)+	; save seconds
		move.b	d2,(a1)+	; minutes
		move.b	d3,(a1)+	; hours
		lea	save,a0		
		jsr	prt_str
		lea	atr_on,a0	; get the attributes (reverse lettering, bold)
		jsr	prt_str
		lea	movalm,a0	; print the time the alarm will go off
		jsr	prt_str
		lea	salarm,a0
		jsr	prt_str
		lea	load,a0
		jsr	prt_str		; restore the cursor
		move.w	#$2000,sr		; turn interrupts back on, if there was
						* an interrupt during the section, it will occur now
	* end of mini critical section ***********************************************
		lea	alrm_on,a0
		move.b	#1,(a0)		; turn the alarm on
		lea	ihdr8,a0	; print alarm set heading
		jsr	prt_str
		bra	main		; loop again

AlmNoGd:	lea	ihdr9,a0	; load error message
		jsr	prt_str		; and print it
		bra	main		; return to the prompt again


inTimer:	jsr	rd_time		; read in the time from the user
		cmp.b	#0,d0		; test if valid
		beq	TmrNoGd		; if not a valid
	* unlike the alarm, the timer is update on every tick, so
	* it isn't necessary to print it out here, it does it in the
	* interrupt.  It does need turned on though
		move.w	#$2700,sr	; temp turn off clock
		lea	tmr_on,a0
		move.b	#1,(a0)		; turn the timer on
		lea	ctimer,a1
		move.b	d1,(a1)+
		move.b	d2,(a1)+
		move.b	d3,(a1)+	; store the timer values
		move.w	#$2000,sr	; turn interrupts back on
		lea	ihdr6,a0
		jsr	prt_str
		bra	main

TmrNoGd:	lea	ihdr7,a0
		jsr	prt_str
		bra	main		; return to the prompt again


inCalc:		jsr	hw9Calc		; go to the calculator	
		bra	main		; return to the prompt again


inClock:	jsr	rd_time
		cmp.b	#0,d0
		beq	ClkNoGd
		lea	cclock,a1
		move.w	#$2700,sr	; turn off clock temporarily
		move.b	d1,(a1)+
		move.b	d2,(a1)+
		move.b	d3,(a1)+	; save the new time
		move.w	#$2000,sr	; turn clock back on
		lea	ihdr5,a0
		jsr	prt_str
		bra	main

ClkNoGd:	lea	ihdr4,a0
		jsr	prt_str
		bra	main		; return to the prompt again



inQuit:		lea	clk_on,a0
		move.b	#0,(a0)		; turn the clock off manually
		move.w	#2700,sr	; turn off the interrupt for the clock
		illegal			; Main program ends here
*
*   PUT YOUR SUBROUTINES HERE!
*
*************************************************************************************
*
*	Tetris program
*	Added just for fun.  It repositions the clock, alarm and timer, which all
*	continue to run while the game runs.
*	Commenting might not be that great....
*
**************************************************************************************
* initialize, replace some of the data for the clock program
tetris:	movem.l	d0-d7/a0-a7,-(sp)		; save all registers
	* initialize the clock and stuff
		lea		clk_on,a0
		move.b		#0,(a0)			; turn off clock temporarily

		move.w		#$2700,sr		; turn off interrupts

		lea		Msgaaa,a1
		jsr		prt_str			; print 'Initializing...'
		lea		atr_off,a0			; make sure the attributes are off
		jsr		prt_str

	* switch the addresses of the messages so that the time is on a different part of the 
	* screen, they will need to be moved back at the end of the game
		lea		setscroll,a1
		lea		scroll_msg,a2
		jsr		switch			; change the scroll pattern
		lea		scroll_msg,a0
		jsr		prt_str

		lea		nmovC,a1			; clock positioning
		lea		movec,a2			; addresses of the move messages
		jsr		switch
		lea		nmovA,a1			; alarm positioning
		lea		movalm,a2
		jsr		switch
		lea		nmovT,a1			; timer positioning
		lea		movTmr,a2
		jsr		switch
		lea		nmovMsg,a1			; Msg positioning
		lea		movMsg,a2
		jsr		switch

		lea		atr_on,a1			; new attributes
		lea		newattr,a2
		jsr		switch

		lea		atr_off,a1
		lea		new_off,a2
		jsr		switch

	* print the clock and the timer/alarm if needed
		lea		Msgaab,a0			; erase the entire screen
		jsr		prt_str

		lea		movec,a1
		jsr		prt_str
		lea		sclock,a2
		jsr		prt_str

		lea		alrm_on,a1
		move.b		(a1),d0
		cmp.b		#0,d0
		beq		lbl_ad
		lea		movalm,a0
		jsr		prt_str
		lea		salarm,a0
		jsr		prt_str

lbl_ad:		lea		tmr_on,a0
		move.b		(a0),d0
		cmp.b		#0,d0
		beq		lbl_ae
		lea		movtmr,a0
		jsr		prt_str
		lea		stimer,a0
		jsr		prt_str

lbl_ae:   * print the playing field
		lea		screen,a0
		jsr		prt_str
	* clean out the field
		lea		field2,a0
		lea		e_field2,a1
lbl_zxc:	move.b		#' ',(a0)+
		cmpa.l		a1,a0
		blt		lbl_zxc

		lea		data,a6
		jsr		create

		move.w		#$2000,sr	; turn interrupts back on
		lea		clk_on,a0
		move.b		#1,(a0)		; turn clock  back on

		lea		cclock,a0
		lea		prevsecs,a1
		move.b		(a0),(a1)		; set previous seconds
		bra		t_lblab
	************************************************************************************
	* The actual game....
tetrismn:	
	* check the drop by comparing seconds of the clock
		move.w		#$2000,sr	; turn them on
		nop				; if an interrupt, make sure it has time
		move.w		#$2700,sr	; turn them off

		lea		atr_off,a0
		jsr		prt_str

		lea		prevsecs,a1
		lea		cclock,a0
		move.b		(a0),d0
		move.b		(a1),d1
		cmp.b		d0,d1
		beq		t_lblaa		* no drop
		jsr		drop_i		* drop the item
		move.b		d0,(a1)		; set the new previous seconds
		bra		t_lblab		* go to the drawing section

t_lblaa:	trap		#1			; check for input
		cmp.b		#0,d0
		beq		tetrismn
		trap		#2

		move.b		d0,d1
		move.b		#8,d0
		trap		#3
		move.b		#' ',d0
		trap		#3
		move.b		#8,d0
		trap		#3
		
		cmp.b		#'/',d1		; check for <down> keypress
		beq		t_lblac
		cmp.b		#',',d1		; check for move left
		beq		t_lblad		; move left
		cmp.b		#'.',d1		; check for move right
		beq		t_lblae		; move right
		cmp.b		#'z',d1
		beq		t_lblaf		; rotate left
		cmp.b		#'x',d1
		beq		t_lblag		; rotate right
		cmp.b		#27,d1
		beq		tetrsOvr		; leave
		cmp.b		#'1',d1
		beq		tetrsOvr

		cmp.b		#'p',d1
		beq		spd_up

		bra		tetrismn

spd_up:	* increase the speed of the game
		move.b		9,d0
		trap		#7
		bra		tetrismn


t_lblac:	* down keypress
		jsr		drop_i
		bra		tetrismn

t_lblad:	* move left
		move.b	#-1,d0
		jsr		move_i
		bra		tetrismn

t_lblae:	* move right
		move.b	#1,d0
		jsr		move_i
		bra		tetrismn

t_lblaf:	* rotate left
		move.b	#1,d0
		jsr		rotate_i
		bra		tetrismn

t_lblag:	* rotate right
		move.b	#-1,d0
		jsr		rotate_i
		bra		tetrismn

t_lblab:	* this label is left over from a bad looping decision, left here just incase I didn't update everything
		bra		tetrismn	* keep looping

	************************************************************************************
	* Shutdown, restore the values of the clock program, reverse most things from the setup
tetrsOvr:
		move.w		#$2700,sr		; make sure off

		lea		clk_on,a0
		move.b		#0,(a0)				; turn off the clock

		lea		Msgaab,a0			; erase the entire screen
		jsr		prt_str

* fill half the screen with black again		
		lea		xy_init,a0
		jsr		prt_str
		lea		newattr,a0
		jsr		prt_str
		clr.l		d1

		move.b		#' ',d0
		move.w		#960,d1		; 80*12=960 white spaces
		bra		spTsta

spLpa:		trap		#3
spTsta:		dbra		d1,spLpa

		lea		save,a0
		jsr		prt_str

		lea		atr_off,a0
		jsr		prt_str

		lea		setscroll,a1
		lea		scroll_msg,a2
		jsr		switch			; change the scroll pattern
		lea		scroll_msg,a0
		jsr		prt_str

		lea		nmovC,a1			; clock positioning
		lea		movec,a2			; addresses of the move messages
		jsr		switch
		lea		nmovA,a1			; alarm positioning
		lea		movalm,a2
		jsr		switch
		lea		nmovT,a1			; timer positioning
		lea		movTmr,a2
		jsr		switch
		lea		nmovMsg,a1			; Msg positioning
		lea		movMsg,a2
		jsr		switch

		lea		atr_on,a1			; new attributes
		lea		newattr,a2
		jsr		switch

		lea		atr_off,a1
		lea		new_off,a2
		jsr		switch

		lea		atr_on,a0
		jsr		prt_str

		lea		movec,a1
		jsr		prt_str
		lea		sclock,a2
		jsr		prt_str

		lea		alrm_on,a1
		move.b		(a1),d0
		cmp.b		#0,d0
		beq		lbl_av
		lea		movalm,a0
		jsr		prt_str
		lea		salarm,a0
		jsr		prt_str

lbl_av:		lea		tmr_on,a0
		move.b		(a0),d0
		cmp.b		#0,d0
		beq		lbl_ax
		lea		movtmr,a0
		jsr		prt_str
		lea		stimer,a0
		jsr		prt_str

	* everything is back to what it should be
lbl_ax:		lea		atr_off,a0
		jsr		prt_str
		lea		gfx,a0
		jsr		prt_str
		move.w		#$2000,sr		; timer back on
		lea		clk_on,a0			; turn back on the clock
		move.b		#1,(a0)
		movem.l		(sp)+,d0-d7/a0-a7		; restore the registers
		rts
***************************************************************************************
*	rotate_i	- rotate the item to the left or right
*	Input:	d0 - the direction.
*		(-1) -> right
*		1    -> left
*	Output: If the rotation can be done, it is, otherwise, it just returns
***************************************************************************************
rotate_i:	movem.l		d0/d1/d2/d3/a0/a1,-(sp)

		lea		i_pos,a0
		move.b		(a0),d3
		add.b		d0,d3

		cmp.b		#0,d3
		bgt		rLblaa
		move.w		#4,d3
rLblaa:		cmp.b		#5,d3
		blt		rLblbb
		move.w		#1,d3
rLblbb:		lea		t_xy,a0
		move.b		d3,(a0)		; temp save of the new position
		lea		i_xy,a0
		move.b		(a0)+,d1
		move.b		(a0)+,d2
		lea		i_type,a0
		move.b		(a0),d0
		jsr		plot_i		; plot the item into t_pts

		lea		t_pts,a0
		move.w		#3,d3

rLblcc:		move.b		(a0)+,d1
		move.b		(a0)+,d2
		jsr		readxy
		cmp.b		#0,d0
		blt		rLbldd
		cmp.b		#'#',d0
		beq		rLbldd
		dbra		d3,rLblcc
	* valid rotation
		lea		t_xy,a0
		lea		i_pos,a1
		move.b		(a0),(a1)		; save the position
*		lea		i_xy,a0
*		lea		t_xy,a1
*		move.w		(a0),(a1)		; set the same xy so draw_i doesn't mess things up
		jsr		draw_i		; draw and move points from t_pts to i_pts

rLbldd:		movem.l		(sp)+,d0/d1/d2/d3/a0/a1
		rts

********************************************************************************
*	draw_i - given the point in i_pts and t_pts, this subroutine deletes
*		 the item from i_pts on the field, and draws to the t_pts
*	Input: None
*	Output: None
********************************************************************************
draw_i:		movem.l		d0/d1/d2/d3/a0/a1,-(sp)
	* delete the old item
		move.w		#3,d3
		lea		i_pts,a0
		
lblDraw:	move.b		(a0)+,d1
		move.b		(a0)+,d2
		move.b		#' ',d0
		jsr		writexy
		dbra		d3,lblDraw

		move.w		#3,d3
		lea		t_pts,a1
	* draw the new item
lbl2Draw:	move.b		(a1)+,d1
		move.b		(a1)+,d2
		move.b		#'@',d0
		jsr		writexy
		dbra		d3,lbl2Draw

		move.w		#3,d3
		lea		i_pts,a0
		lea		t_pts,a1
lbl3Draw:	move.w		(a1)+,(a0)+	; move the points from temp to the set points
		dbra		d3,lbl3Draw

		movem.l		(sp)+,d0/d1/d2/d3/a0/a1
		rts

********************************************************************************
*	move_i - move the item ( if possible )
*	Input:
*		d0:	1 - move to the right, -1 - move left
*	Output: If the piece can be moved, it is updated in the field
*		  Otherwise, nothing happens
********************************************************************************
move_i:		movem.l		d0/d1/d2/d3/d4/a0/a1,-(sp)	
		move.b		d0,d4		; save the move for later

	* set all of the new points as they are updated
		move.w		#3,d3
		lea		i_pts,a0
		lea		t_pts,a1

lbl_jk:		move.b		(a0)+,d1
		move.b		(a0)+,d2
		add.b		d4,d1		; update due to the move
		move.b		d1,(a1)+
		move.b		d2,(a1)+	; save in t_pts
		jsr		readxy		; read the xy from the field
		cmp.b		#0,d0
		blt		movDone
		cmp.b		#'#',d0
		beq		movDone
		dbra		d3,lbl_jk
	* if it reached this point, than it's valid,.. update the xy coordinate in i_xy
		lea		i_xy,a0
		add.b		d4,(a0)	; update the x value
		
	* the old item needs erased, and the new one drawn
		jsr		draw_i

movDone:	movem.l		(sp)+,d0/d1/d2/d3/d4/a0/a1
		rts

********************************************************************************
*	drop_i	drop the item, (due to the time being up)
*	Input:	None, assumes a drop of 1 in the field
*	Output: If the drop is successful (doesn't land on another) then it
*		returns like normal.  If it does land, than it	
********************************************************************************
drop_i:		movem.l		d0/d1/d2/d3/d4/a0/a1,-(sp)
		move.w		#3,d3
		lea		i_pts,a0
		lea		t_pts,a1

lblttt:		move.b		(a0)+,d1	; get x
		move.b		(a0)+,d2	; get y
		add.b		#1,d2		; drop
		move.b		d1,(a1)+
		move.b		d2,(a1)+	; save points in t_xy
		jsr		readxy		; test xy
		cmp.b		#0,d0
		blt		lblhit
		cmp.b		#'#',d0
		beq		lblhit
		dbra		d3,lblttt

	* if it gets to this point, than it is valid drop, no hit.
		jsr		draw_i
		lea		i_xy,a0
		move.w		(a0),d1
		add.w		#$0001,d1
		move.w		d1,(a0)		; save new y coordinate

lblsss:		movem.l		(sp)+,d0/d1/d2/d3/d4/a0/a1
		rts

lblhit:	* the item hit something while falling, so make it land
		move.w		#3,d3
		lea		i_pts,a0
		move.b		#'#',d0		; set what to draw
lblqqq:		move.b		(a0)+,d1
		move.b		(a0)+,d2
		jsr		writexy
		dbra		d3,lblqqq
	* before a new item is created, check all of the lines that the dropped
	* item landed in.  Only these four lines (at max) need to be checked for
	* a complete line.
		lea		clk_on,a0
		move.b		#0,(a0)
		move.w		#$2000,sr
		
		move.w		#3,d3		; check all four lines
		lea		i_pts,a0

		move.w		#19,d2
lblrsr:		move.w		#9,d1		; set another counter
lbl_wrs:	jsr		readxy		; check that coordinate
		cmp.b		#'#',d0
		bne		notLine
		dbra		d1,lbl_wrs
	* if it reaches these lines, than it found a complete line, have to update it
		jsr		wipe
		add.b		#1,d2		; have to recheck the same line

notLine:	dbra		d2,lblrsr
		lea		clk_on,a0
		move.b		#1,(a0)
		move.w		#$2700,sr	; turn timer back on

	* if any comlete lines, they are cleared out, we can create a new piece and continue

		jsr		create		; create a new item to drop
		bra		lblsss

********************************************************************************
*	wipe	called when a complete line is found, and so the lines above need moved down
*	Input: d2 - the y value of the line that was found to be complete
********************************************************************************
wipe:		movem.l		d0/d1/d2,-(sp)
		and.w		#$00ff,d2	; make sure word value
w_lbl_w:	move.w		#9,d1
w_lbla:		sub.b		#1,d2	; go to the line above
		cmp.b		#0,d2
		blt		w_lblz
		jsr		readxy	; read from the field
		cmp.b		#'#',d0
		bne		w_lblz

w_lblx:		add.b		#1,d2
		jsr		writexy

		dbra		d1,w_lbla	; go thru the row

		dbra		d2,w_lbl_w	; for each column

		movem.l		(sp)+,d0/d1/d2
		rts

w_lblz:		move.b		#' ',d0
		bra		w_lblx

********************************************************************************
*	plot_i	plot the item into x,y coordinates for each 'block'
*	Input:		d0 - type of block
*			d1 - x of the item
*			d2 - y of the item
*			d3 - position (rotation)
*	Output:	Given the above information, the piece is plotted into four
*			sets of (x,y) which is stored in memory at 't_pts'
********************************************************************************
plot_i:		movem.l		d0/d1/d2/d3/d4/a0,-(sp)
		lea		t_pts,a0		; get address to store the points

	* switch by the type value
		cmp.b		#1,d0
		beq		pBlock
		cmp.b		#2,d0
		beq		pLong
		cmp.b		#3,d0
		beq		pS
		cmp.b		#4,d0
		beq		pRevS
		cmp.b		#5,d0
		beq		pL
		cmp.b		#6,d0
		beq		pRevL
* T shaped item  ==========================================================================
	* #7 is a T shaped, and also the default
		move.b		d1,d4
		lsl.w		#8,d4
		move.b		d2,d4
		add.w		#$0101,d4
		move.w		d4,(a0)+	; set middle point
		btst		#0,d3
		beq		pTlblaz		; if left or right
		move.b		d1,d4
		lsl.w		#8,d4
		move.b		d2,d4
		add.w		#$0001,d4
		move.w		d4,(a0)+
		add.w		#$0200,d4
		move.w		d4,(a0)+
		cmp.b		#1,d3
		beq		pTlblax
		sub.w		#$0101,d4
		move.w		d4,(a0)+
		bra		pDone
pTlblax:	add.w		#$ff01,d4
		move.w		d4,(a0)+
		bra		pDone

pTlblaz:   * left or right positioned
		sub.w		#$0001,d4
		move.w		d4,(a0)+
		add.w		#$0002,d4
		move.w		d4,(a0)+
		cmp.b		#02,d3
		beq		pTlblav
		sub.w		#$0101,d4
		move.w		d4,(a0)+
		bra		pDone
pTlblav:	add.w		#$00ff,d4
		move.w		d4,(a0)+
		bra		pDone

* block item, all the same! ===================================================================
pBlock:		move.b		d1,d4
		lsl.w		#8,d4
		move.b		d2,d4		; set x,y
		move.w		d4,(a0)+
		add.b		#1,d4
		move.w		d4,(a0)+
		add.w		#$0100,d4
		move.w		d4,(a0)+
		sub.b		#1,d4
		move.w		d4,(a0)+
		bra		pDone

* long item, only two positions to plot ======================================================
pLong:		btst		#0,d3		; test position
		beq		pLlbl
		* vertical
		move.b		d1,d4
		add.b		#1,d4
		lsl.w		#8,d4
		move.b		d2,d4
		move.w		d4,(a0)+
		add.b		#1,d4
		move.w		d4,(a0)+
		add.b		#1,d4
		move.w		d4,(a0)+
		add.b		#1,d4
		move.w		d4,(a0)+
		bra		pDone
pLlbl:	* positions 2 or 4, both the same, (horizontal)
		move.b		d1,d4
		lsl.w		#8,d4
		move.b		d2,d4
		add.b		#2,d4
		move.w		d4,(a0)+
		add.w		#$0100,d4
		move.w		d4,(a0)+
		add.w		#$0100,d4
		move.w		d4,(a0)+
		add.w		#$0100,d4
		move.w		d4,(a0)+
		bra		pDone

* S shaped item, again only two postions ======================================================
pS:
		move.b		d1,d4
		add.b		#1,d4
		lsl.w		#8,d4
		move.b		d2,d4
		add.b		#1,d4
		move.w		d4,(a0)+
		add.b		#1,d4
		move.w		d4,(a0)+
		btst		#0,d3		; test the position
		beq		pSlbla
		sub.w		#$0100,d4
		move.w		d4,(a0)+
		add.w		#$01ff,d4
		move.w		d4,(a0)+
		bra		pDone
pSlbla:		sub.w		#$0101,d4
		move.w		d4,(a0)+
		sub.b		#1,d4
		move.w		d4,(a0)+
		bra		pDone

*  Reversed S, two positions to draw ==========================================================
pRevS:

		move.b		d1,d4
		add.b		#1,d4
		lsl.w		#8,d4
		move.b		d2,d4
		add.b		#1,d4
		move.w		d4,(a0)+
		add.b		#1,d4
		move.w		d4,(a0)+
		btst		#0,d3		; test the position
		beq		pRevSz
		add.w		#$0100,d4
		move.w		d4,(a0)+
		sub.w		#$0201,d4
		move.w		d4,(a0)+
		bra		pDone

pRevSz:		add.w		#$00ff,d4
		move.w		d4,(a0)+
		sub.b		#1,d4
		move.w		d4,(a0)+
		bra		pDone

* ignore the lines below for now
		move.b		d1,d4
		lsl.w		#8,d4
		move.b		d2,d4
		add.w		#0101,d4
		move.w		d4,(a0)+
		add.b		#1,d4
		move.w		d4,(a0)+
		btst		#0,d3
		beq		pRevSb
		sub.w		#$0101,d4
		move.w		d4,(a0)+
		add.w		#$0201,d4
		move.w		d4,(a0)+
		bra		pDone

pRevSb:		add.w		#$00ff,d4
		move.w		d4,(a0)+
		sub.b		#1,d4
		move.w		d4,(a0)+
		bra		pDone

*  L shaped, have to draw all four ============================================================
pL:		move.b		d1,d4
		lsl.w		#8,d4
		move.b		d2,d4
		add.w		#$0101,d4
		move.w		d4,(a0)+
		btst		#0,d3
		beq		pLlbla
		sub.b		#1,d4
		move.w		d4,(a0)+
		add.b		#2,d4
		move.w		d4,(a0)+
		cmp.b		#3,d3
		beq		pLlblb
		add.w		#$0100,d4
		move.w		d4,(a0)+
		bra		pDone
pLlblb:		sub.w		#$0102,d4
		move.w		d4,(a0)+
		bra		pDone

pLlbla:		sub.w		#$0100,d4
		move.w		d4,(a0)+
		add.w		#$0200,d4
		move.w		d4,(a0)+
		cmp.b		#4,d3		; check the type
		beq		pLlblc
		sub.w		#$0200,d4
		add.b		#1,d4
		move.w		d4,(a0)+
		bra		pDone

pLlblc:		sub.w		#1,d4
		move.w		d4,(a0)+
		bra		pDone

* Reversed L, again all 4 need drawn ==========================================================
pRevL:		move.b		d1,d4
		lsl.w		#8,d4
		move.b		d2,d4
		btst		#0,d3
		beq		pRevLa
		add.w		#$0100,d4
		move.w		d4,(a0)+
		add.b		#1,d4
		move.w		d4,(a0)+
		add.b		#1,d4
		move.w		d4,(a0)+
		cmp.b		#1,d3
		beq		pRevLb
		add.w		#$00fe,d4
		move.w		d4,(a0)+
		bra		pDone

pRevLb:		sub.w		#$0100,d4
		move.w		d4,(a0)+
		bra		pDone

pRevLa:	* pos #2 and 4
		add.b		#1,d4
		move.w		d4,(a0)+
		add.w		#$0100,d4
		move.w		d4,(a0)+
		add.w		#$0100,d4
		move.w		d4,(a0)+
		cmp.b		#2,d3
		beq		pRevLc
		add.b		#1,d4
		move.w		d4,(a0)+
		bra		pDone

pRevLc:		sub.w		#$0201,d4
		move.w		d4,(a0)+

* All done ============================================================================
pDone:		movem.l	(sp)+,d0/d1/d2/d3/d4/a0
		rts
********************************************************************************
*	create - create a new piece and store in memory
*		  This uses A6 as a 'static' varaiable which keeps track of where in
*		  the data the 'random' number is to loaded from
********************************************************************************
create:		movem.l		d0/d1/d2/d3/a0/a1,-(sp)
		move.b		(a6)+,d0
		lea		enddata,a0
		cmpa.l		a0,a6
		ble		lbl_af
		lea		data,a6		; reset the 'random' pointer 

lbl_af:		lea		i_type,a0
		move.b		d0,(a0)		; set the type
		lea		i_pos,a0
		move.b		#1,(a0)		; set the position (rotation)
		lea		i_xy,a0
		move.w		#$0401,(a0)

		lea		i_pts,a0
		move.w		#$0400,(a0)
		move.b		#1,d3
		move.b		#0,d2
		move.b		#4,d1
		jsr		plot_i		; plot the item

		lea		i_pts,a0
		lea		t_pts,a1
		move.w		#3,d3
		move.b		#'@',d0

lbl_gg:		move.b		(a1)+,d1
		move.b		(a1)+,d2
		move.b		d1,(a0)+
		move.b		d2,(a0)+
		jsr		writexy
		dbra		d3,lbl_gg

		movem.l		(sp)+,d0/d1/d2/d3/a0/a1
		rts
********************************************************************************
*	switch - switch the move messages of a1 and a2
*	Input - a1 - one of the addresses to switch
*		  a2 - the other address
********************************************************************************
switch:		movem.l		d0/d1/d2/a1/a2,-(sp)
		move.w		#10,d0
		bra		lbl_ac

lbl_ab:		move.b		(a1),d1
		move.b		(a2),d2
		move.b		d2,(a1)+
		move.b		d1,(a2)+

lbl_ac:		dbra		d0,lbl_ab
		
swDone:		movem.l		(sp)+,d0/d1/d2/a1/a2
		rts
*******************************************************************************
*	gotoxy - gotoxy on the screen
*	Input: 	d1 - x
*		d2 - y
*******************************************************************************
gotoxy:		movem.l		d0/d1/d2/a0,-(sp)
		and.l		#$000000ff,d1
		and.l		#$000000ff,d2	; clear out the upper bytes
		add.b		#3,d1
		add.b		#2,d2
		lea		x_y,a0
		move.l		d2,d0
		move.l		d1,d2		; keep x around for later
		jsr		div10

		cmp.b		#0,d0		; test if zero in d0
		beq		g_lblaa
		add.b		#'0',d0
		move.b		d0,(a0)+

g_lblaa:	add.b		#'0',d1
		move.b		d1,(a0)+
		
		move.b		#';',(a0)+

		move.l		d2,d0
		jsr		div10

		cmp.b		#0,d0
		beq		g_lblbb
		add.b		#'0',d0
		move.b		d0,(a0)+

g_lblbb:	add.b		#'0',d1
		move.b		d1,(a0)+

		move.b		#'H',(a0)+
		move.b		#0,(a0)+		; enter in a NULL

		lea		g_xy,a0
		jsr		prt_str

		movem.l		(sp)+,d0/d1/d2/a0
		rts

********************************************************************************
*	readxy - reads the location (x,y) from the tetris field
*	Input:	d1 - x	( x and y are treated as unsigned word values)
*			d2 - y
*	Output: 	d0 - value from the field at (x,y), or -1 if not in field
********************************************************************************
readxy:		movem.l		d1/d2/a0,-(sp)		; save registers
		and.l		#$000000ff,d1
		and.l		#$000000ff,d2	; make sure it's only the byte value
		cmp.b		#0,d1
		blt		lblz
		cmp.b		#0,d2
		blt		lblz
		cmp.b		#9,d1
		bgt		lblz
		cmp.b		#19,d2
		bgt		lblz
		lsl.l		#4,d2				; offset for each line
		add.l		d1,d2				; total offset from beginning of field
		lea		field2,a0
		adda.l		d2,a0
		move.b		(a0),d0		; get the data, the 2 is needed for
								* the offset because of the space and '|'
lbly:		movem.l		(sp)+,d1/d2/a0		; restore registers
		rts
lblz:		move.l		#-1,d0
		bra		lbly
*******************************************************************************
*	writexy - almost as same as readxy, but writes d0 to (d1,d2)
*******************************************************************************
writexy:	movem.l		d0/d1/d2/a0,-(sp)	; save regs
 * write it to the screen
		and.l		#$000000ff,d1
		and.l		#$000000ff,d2

		jsr		gotoxy
		trap		#3
		lea		movBack,a0
		jsr		prt_str

	* enter it into the 'field' so that the data is recorded
		lea		field2,a0
		lsl.l		#4,d2		; times 16
		add.l		d2,d1		; add in x offset
		adda.l		d1,a0
		move.b		d0,(a0)		; enter the data into the field		 

		movem.l		(sp)+,d0/d1/d2/a0
		rts

********************************************************************************
*
*	END OF TETRIS SUBROUTINES
*
********************************************************************************




********************************************************************************
*	rd_time	- prompts and reads in the time from the user
*	Input - None
*	Output - the (hh:mm:ss) that the user entered
*		d0 - 0 if invalid input, 1 - valid input ( which means valid/invalid output too )
*		d1 - seconds
*		d2 - minutes
*		d3 - hours
********************************************************************************
rd_time:	movem.l	a0,-(sp)		; save a0
		jsr	newline
		lea	ihdr2,a0
		jsr	prt_str		; print the prompt for the input

		lea	input,a0
		jsr	read_in			; read in the input
	* process the input
		clr.l	d1
		clr.l	d2		; clear out the return registers just in case
		clr.l	d3
		move.b	(a0)+,d0		; read in the first character of the hours
		jsr	testTm
		cmp.b	#0,d0
		beq	rdTErr
		sub.b	#'0',d0		; subtract offset
		move.b	d0,d3		; move into the hours part
		lsl.b	#3,d3
		lsl.b	#1,d0
		add.b	d0,d3		; finish multiply by ten
		
		move.b	(a0)+,d0
		jsr	testTm		; check the second digit of hours
		cmp.b	#0,d0
		beq	rdTErr
		sub.b	#'0',d0
		add.b	d0,d3		; save the number of hours
		move.b	(a0)+,d0	; get next character
		cmp.b	#':',d0		; check if the digit is a colon
		bne	rdTerr		; if not, error
		move.b	(a0)+,d0	; get the first digit of the minutes
		jsr	testTm
		cmp.b	#0,d0
		beq	rdTerr
		sub.b	#'0',d0		; ascii offset
		move.b	d0,d2		; save into the minutes
		lsl.b	#3,d0		; times 8
		lsl.b	#1,d2		; times 2
		add.b	d0,d2		; equal times ten
		move.b	(a0)+,d0
		jsr	testTm
		cmp.b	#0,d0
		beq	rdTErr
		sub.b	#'0',d0		; ascii offset
		add.b	d0,d2		; get the total number of minutes entered
	* seconds
		move.b	(a0)+,d0
		cmp.b	#':',d0
		bne	rdTErr
		move.b	(a0)+,d0
		jsr	testTm
		cmp.b	#0,d0
		beq	rdTerr
		sub.b	#'0',d0
		move.b	d0,d1
		lsl.b	#3,d1		; times 8
		lsl.b	#1,d0		; times 2
		add.b	d0,d1		; times 10 total
		move.b	(a0)+,d0
		jsr	testTm
		cmp.b	#0,d0
		beq	rdTerr
		sub.b	#'0',d0
		add.b	d0,d1		; get the total number of seconds
		move.b	(a0)+,d0
		
		move.b	#$7f,d0		; turn valid input on

		cmp.b	#23,d3		; check the hours
		bgt	rdTErr
		cmp.b	#0,d3
		blt	rdTErr
		cmp.b	#59,d2		; check minutes
		bgt	rdTErr
		cmp.b	#0,d2	
		blt	rdTErr
		cmp.b	#59,d1		; check seconds
		bgt	rdTErr
		cmp.b	#0,d1
		blt	rdTErr	

rdTEnd:		movem.l	(sp)+,a0		; restore register
		rts				; return from subroutine

rdTErr:		clr.l	d0
		jsr	newline
		lea	ihdr3,a0
		jsr	prt_str
		bra	rdTEnd

******************
* test the character entered to see if it was a valid character
* if invalid, d0 is zero, otherwise it is the character still
testTm:	
		cmp.b	#'0',d0
		blt	TmNot
		cmp.b	#'9',d0
		bgt	TmNot
		rts
TmNot:		clr.l	d0
		rts

********************************************************************************
*  t_time - Translate the time into a string
*  Input: d1 - seconds
*         d2 - minutes
*         d3 - hours
*         d4 - leading zeros on or off ( 1-on, 0-off )
*	  d5 - temp variable
*	  a1 - address to store the string
********************************************************************************
t_time:		movem.l	d0/d1/d2/d3/d4/d5/a1,-(sp)
		clr.l	d0
         	move.b	d1,d5		; temp save of the seconds
         	move.b	d3,d0		; do the hours first
         	jsr	div10		; divide by ten, tens digit in d0, remainder in d1
         	cmp.b	#0,d0		; test if it is a leading zero
         	bne	t_notzero
	*else it is a zero
         	cmp.b	#0,d4	; test if show leading zero on/off
         	bne	t_notzero	; treat it like any other number if on
      * else we have to skip over it
      		move.b	#' ',(a1)+		; enter in a space to the string
      		bra	t_hours
t_notzero:	add.b	#'0',d0		; get the ascii char code
		move.b	d0,(a1)+		; enter into string
t_hours:	add.b	#'0',d1		; get the remainder's ascii value
		move.b	d1,(a1)+			; enter into the string
      		move.b	#':',(a1)+
      		clr.l	d0
      		move.b	d2,d0		; time for the minutes
      		jsr	div10		; div by ten
      		add.b	#'0',d0
      		move.b	d0,(a1)+		; enter into the string
      		add.b	#'0',d1
      		move.b	d1,(a1)+		; enter the second minute digit into the string
 		move.b	#':',(a1)+
      		clr.l	d0			; time for the seconds
      		move.b	d5,d0
      		jsr	div10
      		add.b	#'0',d0
      		move.b	d0,(a1)+
      		add.b	#'0',d1
      		move.b	d1,(a1)+
      		move.b	#' ',(a1)+
      		move.b	#0,(a1)		; enter in a NULL to finish the string

		movem.l	(sp)+,d0/d1/d2/d3/d4/d5/a1
            	rts

*---------------------------------------------------------------------------------
*	prt_str - prints a string in the location pointed to by a0
*	Input:	a0 - the address of the string
*	Output:	none.
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
******************************************************************************
*		Calculator program 
* 
hw9Calc:
		jsr	newline
		lea	line1,a0	
		jsr	prt_str		; print the heading
		jsr	newline

		jsr	asktype		; find out if infix or postfix notation

loop:		jsr	newline
		lea	prompt,a0	; address of the prompt
		jsr	prt_str		;  go print it

		lea	input,a0	; set address to store the input
		jsr	read_in		; read in the input from the prompt
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

		rts			; return to the main loop

noexit:		lea	uansr,a1	; check for infix/postfix representation
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

*---------------------------------------------------------------------------------
		end			; end of file
