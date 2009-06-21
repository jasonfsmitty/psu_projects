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
                movem.l	(sp)+,d0-d7/a0-a6
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
atr_on:		dc.b	$1b,'[1;7m',0    	; turn bold, reverse video on.
atr_off:	dc.b	$1b,'[0m',0	   	; turn off char attributes
save:		dc.b	$1b,'7',0		; save current cursor position
load:		dc.b	$1b,'8',0		; reload the cursor settings
movec:		dc.b	$1b,'[9;32H',0		; move to the clock position
movalm:		dc.b	$1b,'[10;32H',0		; move to the alarm postion
movtmr:		dc.b	$1b,'[11;32H',0		; move to postion for the timer
movMsg:         dc.b	$1b,'[7;28H',0		; move to the position of the message output
gfx:		dc.b	$1b,'[13;0H',0		; graphics character set

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

ihdr1:		dc.b	' Clock program using level 4 interrupts,'
		dc.b	' by Jason F Smith.  Choose your option',$0a,$0d
		dc.b	'  a - set alarm    c - set timer    s - set time'
		dc.b	$0a,$0d,'  p - run calculator    q - quit',$0a,$0d
		dc.b	$0a,$0d,' >> ',0
ihdr2:		dc.b	' Enter the time (hh:mm:ss): ',0
ihdr3:		dc.b	' Invalid input, make sure you enter as hh:mm:ss.',$0a,$0d,0
ihdr4:		dc.b	' The clock could not be set.',$0a,$0d,0
ihdr5:		dc.b	' The clock has been set.',$0a,$0d,0
ihdr6:		dc.b	' The timer has been set.',$0a,$0d,0
ihdr7:		dc.b	' The timer could not be set.',$0a,$0d,0
ihdr8:		dc.b	' The alarm has been set.',$0a,$0d,0

empty:		dc.b	$1b,'[K ',0		; used to erase the output
******************************************************************************
*
		org	$1000		; Main program start here
start:		trap	#0		; initialize serial port driver
*
*   ENTER YOUR HW9 PROGRAM HERE!
*
CLOCK:		nop			; This is interrupt driven CLOCK!
     * initialize the clock and screen
      		lea   	scroll_msg,a0
      		jsr	prt_str		; set the scroll mode on

* The following three lines are necessary to start 1 sec. timing.
* Execute when ready to tick.
		move.w	#$2000,sr	; Enable interrupt, all levels.
		move.b	#10,D0		; Command the 1sec timer in port B
		trap	#7		;   to start.  TRAP #7 writes to B.
		clr.l	d0
		lea	gfx,a0
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

		bra	mn_poll		; if not a valid input, keep waiting


inAlarm:
		bra	main		; return to the prompt again


inTimer:	
		bra	main		; return to the prompt again


inCalc:		
		bra	main		; return to the prompt again


inClock:
		bra	main		; return to the prompt again



inQuit:		lea	clk_on,a0
		move.b	#0,(a0)		; turn the clock off manually
		move.w	#2700,sr	; turn off the interrupt for the clock
		illegal			; Main program ends here
*
*   PUT YOUR SUBROUTINES HERE!
*
********************************************************************************
*	rd_time	- prompts and reads in the time from the user
*	Input - None
*	Output - the (hh:mm:ss) that the user entered
*		d0 - 0 if invalid input, 1 - valid input ( which means valid/invalid output too )
*		d1 - seconds
*		d2 - minutes
*		d3 - hours
********************************************************************************
rd_time:	movem.l	d4/d5/d6/d7,-(sp)		; save some registers


		movm.l	(sp)+,d4/d5/d6/d7		; restore registers
		rts				; return from subroutine

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




*---------------------------------------------------------------------------------
		end			; end of file
