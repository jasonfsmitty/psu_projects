/*
	Jason F. Smith
	4588

	Mike Schreiber
	5259

	CSE 231
	
	Program #7
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
	
		
	
			! the mode of the operand
			! the register number of operand
		! the condition codes
			! temp variable
			! first operand (digits 2, 3 )
		! second operand (digits 1, 0)
			! temp digit variable
		! counter for the interrupt
		! temporary variable for testing interupt
			! register for the carry bit

		! constants used to call get operand
			! explained at get operand function
	

	.global	main
main:	save	%sp, -96, %sp
start:	
	sub	%g0, 1, %l7 	! initialize the interupt counter
	set	prog, %o1		! read file into program space
	call	read_file
	ld	[%i1+4], %o0		!    file name = argv[1]

	set	prog, %g2		! point at top of program code
	set	PDPReg, %g5		! set up pointer to Registers
	st	%g0, [%g5 + 6*2]	! clear stack pointer and program
					!    counter (at the same time!)
	clr	%g6 
	clr	%g7 
fetch:
! code to implement the carry bit of the PDP-11
	bcc 	carry_clear
	nop				
	ba	carry_done		! the carry bit is set...
	mov	1, %i3 		! so set it.
carry_clear:	!if the carry bit is clear....
	tst	%g7 
	bge	carry_more
	nop
	neg	%g7 
carry_more:
	srl	%g7 , 16, %g7 	! test to see if the magnitude of the
	tst	%g7 			! answer is larger than 2^16-1
	ble	carry_done
	nop
	mov	1, %i3 
carry_done:
! Here is the interupt testing, if the counter has reached zero, then 
!  interupt the program.
	add	%g2, 0174, %l3 
	cmp	%g6 , %l3 
	be	test_status
	ldsh	[%g2+124], %l3 
	
	tst	%l7 		! test to see if we need to interrupt
	be	interupt
	dec	%l7 
	ba	cont_fetch		! if not, continue like normal
	nop
test_status:				! test to see if we need to 
	tst	%l3 		! start the countdown
	be	cont_fetch
	nop
	mov	10, %l7 		! if yes, set the counter to 10
cont_fetch: ! the normal fetch routine from before under an alias
	ldsh	[%g5 + 7*2], %g3	! read program counter
	ldsh	[%g2+%g3],%g4	! read the instruction
	inc	2, %g3
	sth	%g3, [%g5 + 7*2]	! increment program counter

	!	The gist of the emulator is here
	!	Determine which catagory of instruction this is.
	!	These statements are a big if/else section.
	!	Which type of catagory is the instruction?
	
	! the if else section here is the same as projects 5, 6 setup.
	!	The changes take place in the loops.
				
	!setup for call to get the number four digit
	mov 	%g4, %o0
	call 	get_digit
	mov 	4, %o1		! get the fourth digit
	
	mov %o0, %l6 	! store it in a register

	subcc 	%l6 , 7, %g0			!check for sevens
	be 	sevens_loop
	nop
	
	tst 	%l6 				!twos if digit!=0
	bne 	twos_loop				!check if twos
	nop
	
	mov 	%g4, %o0				!number 3 digit
	call 	get_digit
	mov 	3, %o1
	mov 	%o0, %l6 
	
	subcc 	%l6 , 5, %g0			! check if a fives
	be 	fives_loop				! operation
	nop

	subcc 	%l6 , 4, %g0			! is this a jsr instr
	be 	jsr_loop
	nop

	srl 	%g4, 8, %l6 			! check for branch
	tst 	%l6 
	bg 	branch_loop
	nop
	
	tst 	%g4					! HALT! in the name of love...
	be 	end_prog
	nop
	
	subcc 	%g4, 2, %g0				! is this boring...
	be 	rti_loop				! ... i mean a rti instr
	nop
	
	mov 	%g4, %o0
	call 	get_digit			! get the second digit
	mov 	2, %o1
	
	mov 	%o0, %l6 
	
	subcc 	%l6 , 2, %g0
	be 	rts_loop
	nop
	
	subcc 	%l6 , 1, %g0
	be 	jmp_loop
	nop
!=============================================================
!	The following 'so called loops' are not loops.
!	Each one takes care of a case for each catagory
!	of instruction.
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
! jump table to move the the right branch
	sll	%o0, 2, %o0
	set 	sevs_table, %o1	!jump table
	ld 	[%o0+%o1], %o0	!pointer
	jmpl 	%o0, %g0		!transfer control
	nop
	
sevs_table:
	.word s0, s1, s2, s3, s4
s0:!mul ====================================================
	mov 	%l4 , %o0		! get the answer
	call 	.mul
	mov 	%l5 , %o1
	mov 	%o0, %g7 
	
	srl	%g4, 6, %l6 
	and	%l6 , 1, %l6 	! if even register, 
	cmp	%l6 , %g0		! store 32 bits into two PDP-11
	be	store_word		! registers.
	nop
	stuh	%g7 , [%g6 ]	! if odd, store low half
	ba	fetch				! into the specified register
	mov	%g7 , %l2 
store_word:	
	st	%g7 , [%g6 ]
	ba	fetch
	mov	%g7 , %l2 
s1:!div ======================================================
/* 	Quotient of the division goes to Rn, the remainder
  	goes to Rn+1.
*/
	mov 	%l4 , %o0
	call 	.rem
	mov 	%l5 , %o1

	stsh  	%o0, [%g6 +2]
	
	mov	 %l4 , %o0
	call 	.div
	mov 	%l5 , %o1

	stsh 	%o0, [%g6 ]
	ba 	fetch
	mov	%o0, %l2 
s2:!ash =======================================================
	! arithmetic 
	sll	%l5 , 26, %l5 
	sra	%l5 , 26, %l5 
	tst	%l5 
	bl	shift_right
	nop  ! left...
	add	%l5 , -16, %l3 
	sra	%l4 , %l3 , %l6 
	and	%l6 , 1, %i3 
	ba	shift_done
	sll	%l4 , %l5 , %l4 
shift_right:
	neg 	%l5 
	dec	%l5 
	sra	%l4 , %l5 , %l4 
	and	%l4 , 1, %i3 
	sra	%l4 , 1, %l4 
shift_done:
	stsh	%l4 , [%g6 ]
	mov	%i3 , %g7 
	ba	fetch
	sll	%g7 , 16, %g7 
s3:!ashc ======================================================
	! arithmetic 
	ld	[%g6 ], %l4 
	sll	%l5 , 26, %l5 
	sra	%l5 , 26, %l5 
	tst	%l5 
	bl	shift_right_comb
	nop  ! left...
	sra	%l4 , 16, %l6 
	add	%l5 , -16, %l3 
	sra	%l6 , %l3 , %l6 
	and	%l6 , 1, %i3 
	ba	shift_comb_done
	sll	%l4 , %l5 , %l4 
shift_right_comb:
	neg 	%l5 
	dec	%l5 
	sra	%l4 , %l5 , %l4 
	and	%l4 , 1, %i3 
	sra	%l4 , 1, %l4 
shift_comb_done:
	st	%l4 , [%g6 ]
	mov	%i3 , %g7 
	ba	fetch
	sll	%g7 , 16, %g7 	
s4:!xor =======================================================
	xor	%l4 , %l5 , %g7 
	ba	fetch
	stsh	%g7 , [%g6 ]
!==============================================================	
!==============================================================
!code to ouput the instruction if it is a five in #3
fives_loop:
!  get the operand and the destination address.
	call 	get_operand
	mov 	0 , %o0
	
	mov 	%o0, %g6 
	tst	%g4
	bl	load_fives_byte
	nop
	ba	fives_cont
	ldsh	[%g6 ], %l5 
load_fives_byte:
	ldsb	[%g6 ], %l5 
	
fives_cont:
	mov	%g4, %o0
	call 	get_digit
	mov 	2, %o1		! get the third digit, determines the instr.
	sll 	%o0, 2, %o0		! adjust for word offset of -1
	set 	fives_table, %o1
	ld 	[%o0+%o1], %o0
	jmpl 	%o0, %g0
	nop
fives_table: !====================================================
	.word F0, F1, F2, F3, F4, F5, F6, F7
F0: ! clr !=======================================================
	ba 	end_fives
	mov 	%g0, %g7 
F1: ! complement (com) !==========================================
	ba 	end_fives
	neg 	%l5 , %g7 
F2: ! inc !=======================================================
	ba 	end_fives
	add 	%l5 , 1, %g7 
F3: ! dec !=======================================================
	ba 	end_fives
	sub 	%l5 , 1, %g7 
F4: ! negate (neg) !==============================================
	ba 	end_fives
	neg 	%l5 , %g7 
F5: ! adc (add carry ?? not needed ) !============================
	ba	end_fives
	add	%i3 , %l5 , %g7 
F6: ! sbc (sub carry, also not needed ) !=========================
	ba	end_fives
	sub	%l5 , %i3 , %g7 
F7: ! tst !=======================================================
    ! Nothing needs saved, just set the codes, then loop again
	ba 	fetch
	mov 	%l5 , %l2 
end_fives:
	mov 	%g7 , %l2 
	ba	fetch
	stsh 	%g7 , [%g6 ]
!==============================================================
!code to output operand if a twos instruction
twos_loop:
!  get the first operand.
	call 	get_operand
	mov 	1 , %o0
	mov	%o0, %l4 
!  get the second operand and the destination
	call 	get_operand
	mov	0 , %o0
	mov	%o0, %g6 

!  branch if byte operation
	cmp 	%g4, %g0
	bge 	ld_half			! is this a byte operation
	nop
	ldsb	[%l4 ], %l4 	! then load a byte
	ba 	twos_cont
	ldsb	[%g6 ], %l5 
ld_half:	! =======================================================
	ldsh	[%l4 ], %l4 	! else load a halfword
	ldsh	[%g6 ], %l5 
twos_cont:
	mov	 %g4, %o0
	call 	get_digit
	mov 	4, %o1		! get the third digit, determines the instr.
	sll 	%o0, 2, %o0		! adjust for word offset of -1
	set 	twos_table, %o1
	ld 	[%o0+%o1], %o0
	jmpl 	%o0, %g0		! jump to the right branch
	nop
	
twos_table:
	.word T0, T1, T2, T3, T4, T5, T6
T0: ! not used  =======================================================

T1: ! mov (b) =======================================================
	cmp	%g4, %g0
	bge	mov_half
	mov 	%l4 , %l2 
	ba 	fetch
	stb	%l4 , [%g6 ]
mov_half: !=======================================================
	ba	fetch
	stsh    %l4 , [%g6 ]
T2: ! cmp (b) =======================================================
	ba 	fetch
	sub 	%l4 , %l5 , %l2 
T3: ! bit (b) =======================================================
	ba	 fetch
	and 	%l4 , %l5 , %l2 
T4: ! bic (b) =======================================================
	andn	%l5 , %l4 , %g7 
	stsh	%g7 , [%g6 ]
	ba 	fetch
	mov  	%g7 , %l2 
T5: ! bis (b) =======================================================
	or	%l4 , %l5 , %g7 
	stsh	%g7 , [%g6 ]
	ba	fetch
	mov 	%g7 , %l2 

T6: ! add or sub (depends on sign of instr) =========================
	tst	%g4
	bl	subtract
	nop
	! else add
	add 	%l4 , %l5 , %g7 
	stsh	%g7 , [%g6 ]
	ba 	fetch
	mov 	%g7 , %l2 	
subtract: !=======================================================
	sub 	%l5 , %l4 , %g7 
	stsh	%g7 , [%g6 ]
	ba 	fetch
	mov 	%g7 , %l2 
!==============================================================
!branch instructions
branch_loop:
! read in the branch to value awhile and adjust
	sll	%g4, 24, %g6 
	sra	%g6 , 23, %g6 
	add	%g6 , %g3, %g6 	! could be the new pc address!!
! get the branch table -1 and jump (...jump up, jump down, jump around)
	srl	%g4, 8, %l3 
	sll	%l3 , 2, %l3 
	set	branch_table, %o1
	ld	[%o1+%l3 ], %o0
	jmpl	%o0, %g0
	nop
branch_table:
	.word	B0, B1, B2, B3, B4, B5, B6, B7

B0: ! not used

B1: ! br (always) =======================================================
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B2: ! bne =======================================================
	cmp 	%l2 , %g0
	be	fetch
	nop
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B3: ! beq =======================================================
	cmp	%l2 , %g0
	bne	fetch
	nop
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B4: ! bge =======================================================
	cmp	%l2 , %g0
	bl	fetch
	nop
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B5: ! blt =======================================================
	cmp	%l2 , %g0
	bge	fetch
	nop
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B6: ! bgt =======================================================
	subcc	%l2 , %g0, %g0
	ble	fetch
	nop
	ba	fetch
	stsh	%g6 , [%g5+7*2]
B7: ! ble =======================================================
	cmp	%l2 , %g0
	bg	fetch
	nop
	mov 	%g6 , %g3
	ba	fetch
	stsh	%g6 , [%g5+7*2]
!==============================================================
jsr_loop:
	ldsh	[%g5+6*2], %l3 
	sub	%l3 , 2, %l3 			!auto decrement
	stsh	%l3 , [%g5+6*2]
	add	%l3 , %g2, %g6 	! store the address of stack
	mov 	%g4, %o0
	call 	get_digit
	mov	2, %o1					! get reg number
	sll	%o0, 1, %l1 				!  adjustment
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
	ba	fetch
	stsh	%g6 , [%g5+7*2]
!==============================================================
rti_loop:
! Program counter is popped off stack, as are the condition codes (n/z)
! auto increment on R6 to get the value from the stack
	ldsh	[%g5+6*2], %l3 
	add	%l3 , 4, %o0
	stsh	%o0, [%g5+6*2]		! store the new stack value
	add	%l3 , %g2, %g6 
	ldsh	[%g6 ], %l2 	! load the condition code
	inc	2, %g6 
	ldsh	[%g6 ], %g3		! load the pc and store...
	ba	fetch
	stsh	%g3, [%g5+7*2]		!...into the PDP-11 register
	
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
	
	stsh	%g6 , [%g5+7*2]	! store the new pc into R7
	ba 	fetch
	mov	%g6 , %g3		! just incase
!==============================================================
rts_loop:
! auto increment on R6 to get the value from the stack
	ldsh	[%g5+6*2], %l3 
	add	%l3 , 2, %o0
	stsh	%o0, [%g5+6*2]
	add	%l3 , %g2, %g6 
	ldsh	[%g6 ], %l3 		!get the Rn value
! pc gets the value in Rn
	and 	%g4, 7, %l1 	! get the register number
	sll	%l1 , 1, %l1 	!  for offset adjustment
	ldsh	[%g5+%l1 ], %g3	! get new pc value
	stsh	%g3, [%g5+7*2]
	ba	fetch
	stsh	%l3 , [%g5+%l1 ]
!===================================================================
!===================================================================
interupt:
	stsh	%g0, [%g2+0174]	! clear the status register
	clr	%g6 		! clear the destination register
	sub	%g0, 1, %l7 	! move -1 into the counter, just in case
	clr	%o0			! clear for the write call
	add	%g2, 0172, %o1
	mov	1, %o2
	mov	4, %g1			! trap to system for the write system call
	ta	0

	ldsh	[%g2-2], %l3 	! the interupt handler value
! place the condition codes onto the stack.
	ldsh	[%g5+6*2], %g6 
	sub	%g6 , 4, %g6 	! make room for the 
	stsh	%g6 , [%g5+6*2]		! condition codes and 
	add	%g2, %g6 , %g6 	! the PC
	stsh	%l2 , [%g6 ]
	inc	2, %g6 			! move over for PC
	ldsh	[%g5+7*2], %g3			! store the PC
	stsh	%g3, [%g6 ]
	ba	fetch
	stsh	%l3 , [%g5+7*2]			! store the interrupt address

!===================================================================
!===================================================================
end_prog:				! convenient ending breakpoint
	mov	1, %g1
	ta	 0
!================================================================
!	End of main function
!================================================================
!================================================================
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

	mov 	0, %l0 			!clears total
	mov 	%i0, %l1 		!sets the string ptr
convert_start:
	ldub 	[%l1 ], %l2 		!load in the char
	subcc 	%l2 , 48 , %l2 	!convert to integer value
	bl 	convert_end
	inc 	%l1 				!increment input ptr
	add 	%l2 , %l0 , %l0 		!add to the total
	ba 	convert_start
	sll 	%l0 , 3, %l0 		! to finish the 48 .
convert_end:
	srl 	%l0 , 3, %l0 		!adjust for extra 
	mov 	%l0 , %i0			!return the answer
	mov 	%l1 , %i1			!return the pointer

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
	tst 	%o1
	ble 	end_get_digit
	nop
	srl 	%o0, 3, %o0
	ba 	get_digit_start
	dec 	%o1
end_get_digit:
	and 	%o0, 7, %o0
	retl
	nop
!===================================================
!	End of digit extracting function.
!===================================================
!===================================================
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
		! not used with mode
		! register
		! value of x (displacement)
		! current address
		! is this a byte operation
		! is this an odd mode
		! is byte and deferred
		! explained above
		! return address

.global	get_operand
get_operand:	save	%sp, -96, %sp

! Get the mode and register numbers
	sll 	%i0 , 1, %i0 		! which_op x2
	mov 	%i0 , %o1
	call 	get_digit		! get the register
	mov 	%g4, %o0
	sll	%o0, 1, %l1 		! mov and adjust
	add 	%i0 , 1, %o1
	call 	get_digit		! get the mode
	mov 	%g4, %o0
	mov 	%o0, %l0 
	clr 	%l4 		! clear other variables
	clr 	%l5 
get_op_start:
	and 	%l0 , 1, %l5 		!is it deferred?
	sub 	%l0 , %l5 , %l0 	!get even mode 
	tst 	%g4		! test for byte operation
	bg 	op_cont
	nop
	mov 	1, %l4 	! it is a byte operation
op_cont:! determine which type of mode we are dealing with
	and	%l5 , %l4 , %l6 
	subcc 	%l0 , 1, %g0	! time to determine which addressing
	ble 	direct			! mode we are using
	nop
	subcc 	%l0 , 4, %g0
! if <0, then mode==2, ==0 means mode=4, and otherwise, mode 6
	bl 	auto_inc
	nop
	be 	auto_dec
	nop
	ba 	displace
	nop
direct: ! the register contains the operand
	ba 	have_pointer
	add 	%l1 , %g5, %l3 
auto_inc: ! the register is a pointer to the operand, it is then inc by 2
	ldsh 	[%l1 +%g5], %l3 	!address points to operand
	add 	%l3 , 2, %l2 
! if a byte operation, only add 1, so we sub isbyte from x
	sub 	%l2 , %l4 , %l2 
	add	%l2 , %l6 , %l2 	! if oddbyte, then we are after a pointer
	stsh 	%l2 , [%l1 +%g5]	! so we must adjust so it is still aligned
	ba 	have_pointer		
	add 	%l3 , %g2, %l3 
auto_dec: ! same as auto inc, only it is pre-decremented rather than post-inc.
	ldsh 	[%g5+%l1 ], %l2 
	sub 	%l2 , 2, %l3 
! adjust for byte mode
	add 	%l3 , %l4 , %l3 
	sub	%l3 , %l6 , %l3 
	stsh 	%l3 , [%l1 +%g5]
	ba 	have_pointer		
	add 	%l3 , %g2, %l3 
displace:	! next instruction is a value that is added to the
		! specified register to get a pointer to the operand
	ldsh 	[%g5+7*2], %g3
	ldsh 	[%g3+%g2], %l2 
	inc 	2, %g3
	stsh 	%g3, [%g5+7*2]
	ldsh 	[%g5+%l1 ], %l3 
	add 	%l3 , %l2 , %l3 
	add 	%l3 , %g2, %l3 
have_pointer:	! do we have the address to a pointer, or the operand
	tst	%l5 	! test if it's a deferred mode
	be	have_address	
	nop
	ldsh 	[%l3 ], %l2 	! load up the pointer
	add 	%g2, %l2 , %l3 	! adjust
have_address:! we have the address of the operand, finally
	addcc	%l0 , %l5 , %g0
	bne	return_now
	nop
	tst	%l4 
	bne,a	return_now
	inc	%l3 	! we need to inc the address if
return_now:			! mode zero, and a byte operation
	mov 	%l3 , %i0 
	ret
	restore 
!============================================
!	End of get operand
!============================================
