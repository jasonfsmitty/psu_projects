/*
	Jason F. Smith
	4588
	CSE 331
	
	Program (HW) #6
	
	Once again, this isn't commented extensively.
	It does work, finally, and I'm just glad to 
	finally get it done.
	
	Completion date and time: 1:06 am November 18, 1999
					(time to par-tay) 

*/

 
	!  This program will emulate a PDP-ll program running
	!  a PDP-11 program.  The input is given in a  file as
	!   a list of octal halfwords.  Some instructions
	!  may use two halfwords, if the PC is used in an operand.
	!
	!  Before anything else is done, the program to emulate
	!  will be read from the file and stored into the following
	!  space.  The emulator will then read information out of memory.
	!
	!  Besides the program to simulate, this will also hold
	!  a simulated stack, and an array recording the values of
	!  the PDP-11 registers.   The stack pointer (R6) is to be
	!  initialized to point at the end of the stack.
	!
	.bss
	.align 4		! make it aligned
PDPReg: .skip	2*8		! eight 16-bit registers
	.skip	2		! these two bytes will be used later
stack:	.skip	62		! 62 bytes should be enough for stack
prog:	.skip	180		! store the program here
	.text

	!  The following defines have been provided to give some
	!  suggested register usage.  It also provides a degree of
	!  uniformity in program structure to simplify debugging
	!  during office hours.

			! global pointer to input
			! global instruction pointer
					!    (as an offset off of g2)
			! global instruction
			! pointer to Register Array
!=================================================================
	
		
	
	
	
	
	
	
	
	
	
	
	

		! constants used to call get operand
			
	

	.global	main
main:	save	%sp, -96, %sp
start:
	set	prog, %o1		! read file into program space
	call	read_file
	ld	[%i1+4], %o0		!    file name = argv[1]

	set	prog, %g2		! point at top of program code
	set	PDPReg, %g5		! set up pointer to Registers
	st	%g0, [%g5 + 6*2]	! clear stack pointer and program
					!    counter (at the same time!)
b_point:
	nop
fetch:





	ldsh	[%g5 + 7*2], %g3	! read program counter
	ldsh	[%g2+%g3],%g4	! read the instruction
	inc	2, %g3
	sth	%g3, [%g5 + 7*2]	! increment program counter

	!	The gist of the emulator is here
	!	Determine which catagory of instruction this is.
	!	These statements are a big if/else section.
	!	Which type of catagory is the instruction?
	!	This is the same as in the first project.


!=====================================
/*
	The following lines commented out were used to debug, 
	I left then in here for the next program.
	
	clr 	%i5 

	call 	putchar
	mov 	"\n", %o0

	call 	out_num
	sub	%g3, 2, %o0
	call 	putchar
	mov 	" ", %o0		
out_start_reg:
	call 	putchar
	mov 	"R", %o0
	call 	putchar
	add 	%i5 , 48, %o0
	call 	putchar
	mov 	" ", %o0
	sll 	%i5 , 1, %o0
	call 	out_num
	ldsh 	[%g5+%o0], %o0
	call 	putchar
	mov 	" ", %o0
	
	inc 	%i5 
	subcc 	%i5 , 7, %g0
	ble 	out_start_reg
	nop			
				
	call 	putchar
	mov 	" ", %o0	
	
	call 	fflush
	clr	%o0	
*/
	
	! the if else section here is the same as the last project's setup.
	!	The changes take place in the loops.
				
	!setup for call to get the number four digit
	mov 	%g4, %o0
	call 	get_digit
	mov 	4, %o1
	
	mov %o0, %l6 	!stick it @#$#%
	
!check if sevens
	subcc %l6 , 7, %g0			!check for sevens
	be sevens_loop
	nop
	
	tst %l6 				!twos if digit!=0
	bne twos_loop				!check if twos
	nop
	
	mov %g4, %o0			!number 3 digit
	call get_digit
	mov 3, %o1
	mov %o0, %l6 
	
	subcc %l6 , 5, %g0
	be fives_loop
	nop

	subcc %l6 , 4, %g0
	be jsr_loop
	nop

	srl %g4, 8, %l7 
	tst %l7 
	bg branch_loop
	nop
	
	tst %g4
	be halt_loop
	nop
	
	subcc %g4, 2, %g0
	be rti_loop
	nop
	
	mov %g4, %o0
	call get_digit
	mov 2, %o1
	
	mov %o0, %l6 
	
	subcc %l6 ,	2, %g0
	be rts_loop
	nop
	
	subcc %l6 , 1, %g0
	be jmp_loop
	nop

!============================ default loop to end
! should never be used, assuming all instructions being
! read in are known.
! Safety back up.

	ba end_prog
	nop
	
!==============================================================
!/*
!	The following 'so called loops' are not loops.
!	Each one takes care of a case for each catagory
!	of instruction.  It's way too late (or early)
!	to attempt to comment everything.
!	Basically - each loop outputs the instruction,
!	and then the operand(s) if needed.
!*/
!==============================================================
!loop to output the instruction if a seven is in #4 digit
sevens_loop:


! get the first digit and save it into the register
	mov	%g4, %o0
	call	get_digit
	mov	2, %o1
	sll	%o0, 1, %l1 
	ldsh	[%l1 +%g5], %l4 
	
! get the second and the destination address
	call 	get_operand
	mov	0 , %o0
	mov	%o0, %g6 		!gets the return address
	ldsh	[%g6 ], %l5  	!gets the second operand
	
	add	%l1 , %g5, %g6 
	
	mov 	%g4, %o0
	call	get_digit
	mov	3, %o1		! gets the -1 value
	
	sll	%o0, 2, %o0
	set sevs_table, %o1	!jump table
	ld [%o0+%o1], %o0	!pointer
	jmpl %o0, %g0		!transfer control
	nop
	
sevs_table:
	.word s0, s1, s2, s3, s4
	
s0:!mul
	mov %l4 , %o0
	call .mul
	mov %l5 , %o1
	mov %o0, %g7 
	
	srl	%g4, 6, %l6 
	and	%l6 , 1, %l6 
	cmp	%l6 , %g0
	be	store_word
	nop
	
	stuh	%g7 , [%g6 ]
	mov	%g7 , %l2 
	ba 	fetch
	nop
store_word:	
	st	%g7 , [%g6 ]
	ba	fetch
	mov	%g7 , %l2 
	
	
	and 1, %g4, %l6 	!test if odd destination
	tst %l6 
	bne end_sevens
	nop
	stuh  %g7 , [%g6 +2]
	sra %g7 , 16, %g7 	!we want to save the high half
						! but keep the sign bit
	ba end_sevens
	nop
s1:!div
/*
	Quotient of the division goes to Rn, the remainder
	goes to Rn+1.
*/
	mov %l4 , %o0
	call .rem
	mov %l5 , %o1

	stsh  %o0, [%g6 +2]
	
	mov %l4 , %o0
	call .div
	mov %l5 , %o1

	mov %o0, %g7 
	
	stsh  %g7 , [%g6 ]
	ba fetch
	nop
	
	ba end_sevens
	nop
s2:!ash
s3:!ashc
s4:!xor

end_sevens:
	
!==============================================================	
!==============================================================
!code to ouput the instruction if it is a five in #3
fives_loop:

!  These have the possibility to be byte operations, but it was
!  left out until later.

!  get the operand and the destination address.
	call 	get_operand
	mov 	0 , %o0
	
	mov 	%o0, %g6 
	ldsh	[%g6 ], %l5 
	
	mov	%g4, %o0
	call 	get_digit
	mov 	2, %o1		! get the third digit, determines the instr.
	
	sll 	%o0, 2, %o0		! adjust for word offset of -1
	
	set 	fives_table, %o1
	ld 	[%o0+%o1], %o0
	jmpl 	%o0, %g0
	nop
	
	
fives_table:
	.word F0, F1, F2, F3, F4, F5, F6, F7
F0: ! clr
	ba 	end_fives
	mov 	%g0, %g7 

F1: ! complement (com)
	ba 	end_fives
	neg 	%l5 , %g7 
F2: ! inc
	ba 	end_fives
	add 	%l5 , 1, %g7 
F3: ! dec
	ba 	end_fives
	sub 	%l5 , 1, %g7 
F4: ! negate (neg)
	ba 	end_fives
	neg 	%l5 , %g7 
F5: ! adc (add carry ?? not needed )

F6: ! sbc (sub carry, also not needed )

F7: ! tst
    ! Nothing needs saved, just set the codes, then loop again
	ba fetch
	mov %l5 , %l2 
end_fives:
	mov %g7 , %l2 
	stsh %g7 , [%g6 ]
	
	ba fetch
	nop
!==============================================================
!code to output operand if a twos instruction
twos_loop:

!  get the second operand and the destination
	call 	get_operand
	mov	0 , %o0
	mov	%o0, %g6 
!  get the first operand.
	call 	get_operand
	mov 	1 , %o0
!  branch if byte operation
	cmp 	%g4, %g0
	bge 	ld_half
	nop
	ldsb	[%o0], %l4 
	ba 	twos_cont
	ldsb	[%g6 ], %l5 
ld_half:	
	ldsh	[%o0], %l4 
	ldsh	[%g6 ], %l5 
twos_cont:
	mov	 %g4, %o0
	call 	get_digit
	mov 	4, %o1		! get the third digit, determines the instr.
	
	sll 	%o0, 2, %o0		! adjust for word offset of -1
	
	set 	twos_table, %o1
	ld 	[%o0+%o1], %o0
	jmpl 	%o0, %g0
	nop
	
twos_table:
	.word T0, T1, T2, T3, T4, T5, T6
T0: ! not used

T1: ! mov (b)
	cmp	%g4, %g0
	bge	mov_half
	nop
	ba 	fetch
	stb	%l4 , [%g6 ]
mov_half:
	ba	fetch
	stsh    %l4 , [%g6 ]
T2: ! cmp (b)
! byte cmp is not implemented here, yet
	ba 	fetch
	sub 	%l4 , %l5 , %l2 
T3: ! bit (b)
	ba	 fetch
	and 	%l4 , %l5 , %l2 
T4: ! bic (b)
	andn	%l5 , %l4 , %g7 
	stsh	%g7 , [%g6 ]
	ba 	fetch
	mov  	%g7 , %l2 
T5: ! bis (b)
	or	%l4 , %l5 , %g7 
	stsh	%g7 , [%g6 ]
	ba	fetch
	mov 	%g7 , %l2 

T6: ! add or sub (depends on sign of instr)
	cmp 	%g4, %g0
	bl	subtract
	nop
	! else add
	add 	%l4 , %l5 , %g7 
	stsh	%g7 , [%g6 ]
	ba 	fetch
	mov 	%g7 , %l2 	
subtract:
	sub 	%l5 , %l4 , %g7 
	stsh	%g7 , [%g6 ]
	ba 	fetch
	mov 	%g7 , %l2 

!==============================================================
!branch instructions
branch_loop:
!  	read in the branch to value awhile and adjust
	sll	%g4, 24, %g6 
	sra	%g6 , 23, %g6 
	add	%g6 , %g3, %g6 	! could be the new pc address!!
!	get the branch table -1 and jump (...jump up, jump down, jump around)
	srl	%g4, 8, %l3 
	sll	%l3 , 2, %l3 
	set	branch_table, %o1
	ld	[%o1+%l3 ], %o0
	jmpl	%o0, %g0
	nop
	
branch_table:
	.word	B0, B1, B2, B3, B4, B5, B6, B7

B0: ! not used

B1: ! br (always)
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]

B2: ! bne
	cmp 	%l2 , %g0
	be	fetch
	nop
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B3: ! beq
	cmp	%l2 , %g0
	bne	fetch
	nop
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B4: ! bge
	cmp	%l2 , %g0
	bl	fetch
	nop
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B5: ! blt
	cmp	%l2 , %g0
	bge	fetch
	nop
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B6: ! bgt
	subcc	%l2 , %g0, %g0
	ble	fetch
	nop
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B7: ! ble
	cmp	%l2 , %g0
	bg	fetch
	nop
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
	
!==============================================================
jsr_loop:
	ldsh	[%g5+6*2], %l3 
	sub	%l3 , 2, %l3 	!auto decrement
	stsh	%l3 , [%g5+6*2]
	
	add	%l3 , %g2, %g6 ! store the address of stack
	
	mov 	%g4, %o0
	call 	get_digit
	mov	2, %o1			! get reg number
	sll	%o0, 1, %l1 		!  adjustment
		
	ldsh	[%g5+%l1 ], %g7 
	stsh	%g7 , [%g6 ]
! Rn is now stored onto the stack, next get the destination value of the jump

	call	get_operand
	mov	0, %o0
	mov	%o0, %g6 
! gets the address of the jump operand, now mov R7 into Rn
	ldsh	[%g5+7*2], %g3
	stsh	%g3, [%g5+%l1 ]
! give the pc it's new value
	sub	%g6 , %g2, %g6 
! store the pc
	stsh	%g6 , [%g5+7*2]

	ba 	fetch
	nop
!==============================================================
halt_loop:
	ba end_prog
	nop
!==============================================================
rti_loop:
	! not needed for this assignment

	ba fetch
	nop
!==============================================================
jmp_loop:
! 	jump up, jump down, jump around,... jump, jump
! get the destination
	call 	get_operand
	mov	0 , %o0
	mov	%o0, %g6 
	
	ldsh	[%g6 ], %l3 
	add	%l3 , 2, %l3 
	ldsh	[%g5+7*2], %g3
	add	%g3, %l3 , %g6 
	
	stsh	%g6 , [%g5+7*2]
	ba 	fetch
	mov	%g6 , %g3
!==============================================================
rts_loop:
! we trick the get operand function to auto dec on R6
	mov 	%g4, %l3 
	mov 	22, %g4	! 22==026, mode 2 (auto inc) on R6
	call 	get_operand
	mov 	0 , %o0
	mov 	%o0, %g6 
	mov	%l3 , %g4	
! we now have the address of the value, load the value
	ldsh	[%g6 ], %l3 
! get the value from the register
	mov 	%g4, %o0
	call	get_digit
	mov	0, %o1
	sll	%o0, 1, %l1 
! load the pc from Rn
	ldsh 	[%g5+%l1 ], %g3
	stsh	%g3, [%g5+7*2]
! save the value into the register
	stsh	%l3 , [%g5+%l1 ]
		
	ba fetch
	nop
!==============================================================
!==============================================================
end_prog:				! convenient ending breakpoint
	mov	1, %g1
	ta	 0
!=====================================================================
!======================================================================
	!  Read_file
	!  (not included here... the same as in the last assignment)
	!  You'll still need that, the convert from Octal, and the
	!  digit extractor.  Output routines will not be needed,
!=================================================================
!  Read_file
!  Given the name of the file (from the command line)
!  and a pointer to a program space, reads the data from
!  the file, converts it into binary, and stores it in
!  that space.  This uses the function from the preceding
!  assignment to convert a number from Octal ASCII to binary.
!
!  Input:	%i0 = name of the file
!		%i1 = space in memory to store into
!=================================================================
	

	!local variables

	line = -8

	.global	read_file
read_file:	save	%sp, -104, %sp


	mov	%i0, %o0		! set up call to
	clr	%o1			!    open file
	clr	%o2			!    (just as in the text)
	mov	5, %g1
	ta	0			! and make the system call
	mov	%o0, %l1		! save the file pointer
readloop:
	add	%fp, line, %o1		! read into local memory
	mov	7, %o2			!    (7 bytes)
	mov	3, %g1
	ta	0
	tst	%o0			! at end of input
	ble	done_read		!    give up
	nop
	add	%fp, line, %o0		! pass pointer to routine
	call	conv_oct		!    for output
	stb	%g0, [%o0+6]		! null character at end to be safe
	sth	%o0, [%i1]		!    
	inc	2, %i1			! and update pointer
	ba	readloop
	mov	%l1, %o0		! go back for more data
done_read:
	ret
	restore 
!==============================================
!	End of read in file function
!==============================================


!=========================================================
!Convert_Octal
!	Converts an octal number into it's binary
!	representation.
!Input: %o0 - address of beginning of string
!	      reads until NULL is reached.
!Output:%o0 - binary number representation.
!	%o1 - address of next char after the 
!	      string, useful when calls are
!	      made consecutively.
!=========================================================
	!local variables


	
	
	
	

	.global	conv_oct 
conv_oct :	save	%sp, -96, %sp


	mov 0, %l0 			!clears total
	mov %i0, %l1 		!sets the string ptr
convert_start:
	ldub [%l1 ], %l2 		!load in the char
	subcc %l2 , 48 , %l2 	!convert to integer value
	bl convert_end
	inc %l1 				!increment input ptr
	add %l2 , %l0 , %l0 		!add to the total
	ba convert_start
	sll %l0 , 3, %l0 		! to finish the
						!48 .
convert_end:
	srl %l0 , 3, %l0 		!adjust for extra 
	mov %l0 , %i0			!return the answer
	mov %l1 , %i1			!return the pointer

	ret
	restore 
!===================================
!	End of 48  function
!===================================

!=====================================================================
!	digit function
!	This function extracts the nth digit from an octal number.
!	Input:	%o0 - The number in binary form.
!		%o1 - N (as in the Nth digit to be extracted).
!	Output:	%o0 - The Nth octal digit from the number.
!=====================================================================

	
	
	.global get_digit
get_digit:
get_digit_start:
	tst %o1
	ble end_get_digit
	nop
	srl %o0, 3, %o0
	ba get_digit_start
	dec %o1
	
end_get_digit:
	and %o0, 7 , %o0

	retl
	nop
!===================================================
!	End of digit extracting function.
!===================================================

!===================================================
!
!	get operand
!	This function gets the address for the operand 
!	specified by %o0.  
!	 Note that it is the address and not the operand.
!	 Also, it is the absolute address, no adjusting is
! 	 needed for an offset.
!	Input:	%o0 - which operand?
!		If %o0==1 Then
!			The function gets the first operand.
!			(determined by digits 2 and 3 of the
!			 instruction.)
!		Else (%o0==0)
!			The function gets the second opeand.
!			(determinded by digits 1 and 0 of 
!			 the instruction.)
!	Output: %o0 - Effective address of operand.
!========================================================
	!local variables


		! mode number
		! register
		! value of x (displacement)
		! current address
		! is this a byte operation
		! is this an odd mode
		! explained above
		! return address

.global	get_operand
get_operand:	save	%sp, -96, %sp

!==============================================
!	Get the mode and register numbers
	sll %i0 , 1, %i0 		! which_op x2
! get the register
	mov %i0 , %o1
	call get_digit
	mov %g4, %o0
	mov %o0, %l1 
! get the mode
	add %i0 , 1, %o1
	call get_digit
	mov %g4, %o0
	mov %o0, %l0 
! clear other variables
	clr %l4 
	clr %l5 
get_op_start:
/*
	The only deferred addressing mode needed is mode 1,
	so this was left commented out to avoid bugs.  You 
	never know, especially with the luck I've been having.

	test for is byte and is odd
	and %l0 , 1, %l5 		!is it deferred?
	sub %l0 , %l5 , %l0 	!get even mode 

*/
	tst %g4
	bg op_cont
	nop
	mov 1, %l4 
op_cont:
	! We need to branch to the addressing mode type
	subcc %l0 , 1, %g0
	ble direct
	nop
	subcc %l0 , 4, %g0
	! if <0, then mode==2, ==0 means mode=4, and otherwise, mode 6
	bl auto_inc
	nop
	be auto_dec
	nop
	ba displace
	nop
!=========================================
!	The operand is in Rn
direct:
	sll %l1 , 1, %l1 
	add %l1 , %g5, %l3 
	tst %l0 
	be have_address
	nop
	ba have_pointer
	nop
!===========================================
!	Rn is a pointer to operand, then Rn+=2
auto_inc:
	sll %l1 , 1, %l1 
	ldsh [%l1 +%g5], %l3 	!address points to operand
	add %l3 , 2, %l2 
! if a byte operation, only add 1, so we sub isbyte from x
	sub %l2 , %l4 , %l2 
	stsh %l2 , [%l1 +%g5]
	ba have_address			! no deferred mode, yet
	add %l3 , %g2, %l3 
!====================================================
!	Rn is decremented, and is then a pointer to operand
auto_dec:
	sll %l1 , 1, %l1 
	ldsh [%g5+%l1 ], %l2 
	sub %l2 , 2, %l3 
! adjust for byte mode
	add %l3 , %l4 , %l3 
	stsh %l3 , [%l1 +%g5]
	ba have_address			! no deferred mode, yet
	add %l3 , %g2, %l3 
!=====================================================
!	The next register holds X, then add X to contents
!	of Rn to get a pointer
displace:
	sll %l1 , 1, %l1 
	ldsh [%g5+7*2], %g3
	ldsh [%g3+%g2], %l2 
	inc 2, %g3
	stsh %g3, [%g5+7*2]
	!we have x, now get Rn
	
	ldsh [%g5+%l1 ], %l3 
	! EA==Rn+X
	add %l3 , %l2 , %l3 
	! offset adjustment to get EA
	ba have_address
	add %l3 , %g2, %l3 
!====================================================
!	The address is still a pointer to the effective address
have_pointer:
	ldsh [%l3 ], %l2 
	add %g2, %l2 , %l3 
!	we now have the address of the operand
!==============================================================
!	The final address of the operand, it's EA is in address
have_address:
	mov %l3 , %i0 

	ret
	restore 
!============================================
!	End of get operand
!============================================




!===========================================================
!	Output number function
!	This function prints the number out to the screen.
!	Outputs the number in octal form.
!	Input:	%o0 - The number to output.
!	Output:	(to screen) the number in octal form.
!	
!	NOTE: This function is only here for debuging purposes.
!	I left it here for the next program also.
!===========================================================

	!local variables

	
	
	
	
	
	
	.global	out_num
out_num:	save	%sp, -96, %sp

out_num_start:
	mov %i0, %l0 		!get the number
	
	mov 12, %l3 		!number of shifts
	
	
	tst %l0 
	bge positive
	nop
	
	call putchar
	mov "-", %o0
	
	neg %l0 , %l0 
	
positive:
	cmp %l0 , 500 
	bl  out_num_loop
	nop
	
	mov 18, %l3 
	
out_num_loop:
	sub %l3 , 3, %l3 	!adjust for extra 
	srl %l0 , %l3 , %l2  !
	call putchar			!output the data
	add %l2 , 48 , %o0	!get char code
	sll %l2 , %l3 , %l2 	! back to subtract
		
	tst %l3 			!test if done
	bg  out_num_loop		!return to loop again
	sub %l0 , %l2 , %l0 	!get the lower total
					!  for other digits
	
	ret
	restore 
!===========================================
!	End of output number function
!===========================================

