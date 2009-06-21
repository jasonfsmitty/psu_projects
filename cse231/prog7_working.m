/*
	Jason F. Smith
	4588
	CSE 331
	
	Program (HW) #7 (working version)
*/

include(macro_defs.m)
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

	define(`prog_r', g2)		! global pointer to input
	define(`pc_r', g3)		! global instruction pointer
					!    (as an offset off of prog_r)
	define(`inst_r', g4)		! global instruction
	define(`PDP_r',g5)		! pointer to Register Array
!=================================================================
	define(`destination_r', g6 )
	define(`answer_r', g7 )	
	
	define( `mode_r', l0 )
	define( `reg_r',  l1 )
	define( `condition_r', l2 )
	define( `value_r', l3 )
	define( `first_r', l4 )
	define( `second_r', l5 )
	define( `digit_r', l6 )
	define( `counter_r', l7 )
	define( `status_r', i2 )
	define( `i_r', i3 )

	define( `SECOND_OP', 0 )	! constants used to call get operand
	define( `FIRST_OP', 1 )		
	

	begin_main
start:	
	sub	%g0, 1, %counter_r	! initialize the interupt counter
	set	prog, %o1		! read file into program space
	call	read_file
	ld	[%i1+4], %o0		!    file name = argv[1]

	set	prog, %prog_r		! point at top of program code
	set	PDPReg, %PDP_r		! set up pointer to Registers
	st	%g0, [%PDP_r + 6*2]	! clear stack pointer and program
					!    counter (at the same time!)

fetch:
! Here is the interupt testing, if the counter has reached zero, then 
!  interupt the program.
	ldsh	[%PDP_r+7*2], %pc_r
	ldsh	[%prog_r-2], %value_r
	subcc 	%pc_r, %value_r, %g0
	bge	cont_fetch
	nop
	tst	%counter_r
	bl	test_status
	nop
	be	interupt
	dec	%counter_r
	ba	cont_fetch
	nop
test_status:
	ldsh	[%prog_r+124], %value_r
	tst	%value_r
	be	cont_fetch
	nop
	mov	10, %counter_r
cont_fetch:
	ldsh	[%PDP_r + 7*2], %pc_r	! read program counter
	ldsh	[%prog_r+%pc_r],%inst_r	! read the instruction
	inc	2, %pc_r
	sth	%pc_r, [%PDP_r + 7*2]	! increment program counter

	!	The gist of the emulator is here
	!	Determine which catagory of instruction this is.
	!	These statements are a big if/else section.
	!	Which type of catagory is the instruction?
	!	This is the same as in the first project.
!=====================================
/*
	The following lines commented out were used to debug, 
	I left then in here for the next program.
*/	
/*	clr 	%i_r

	call 	putchar
	mov 	"\n", %o0

	call 	out_num
	sub	%pc_r, 2, %o0
	call 	putchar
	mov 	" ", %o0		
out_start_reg:
	call 	putchar
	mov 	"R", %o0
	call 	putchar
	add 	%i_r, 48, %o0
	call 	putchar
	mov 	" ", %o0
	sll 	%i_r, 1, %o0
	call 	out_num
	ldsh 	[%PDP_r+%o0], %o0
	call 	putchar
	mov 	" ", %o0
	
	inc 	%i_r
	subcc 	%i_r, 7, %g0
	ble 	out_start_reg
	nop			
				
	call 	putchar
	mov 	" ", %o0	
	
	call 	fflush
	clr	%o0	
*/
/*=====================================================*/
	
	! the if else section here is the same as the last project's setup.
	!	The changes take place in the loops.
				
	!setup for call to get the number four digit
	mov 	%inst_r, %o0
	call 	get_digit
	mov 	4, %o1
	
	mov %o0, %digit_r	!stick it @#$#%
	
!check if sevens
	subcc 	%digit_r, 7, %g0			!check for sevens
	be 	sevens_loop
	nop
	
	tst 	%digit_r				!twos if digit!=0
	bne 	twos_loop				!check if twos
	nop
	
	mov 	%inst_r, %o0			!number 3 digit
	call 	get_digit
	mov 	3, %o1
	mov 	%o0, %digit_r
	
	subcc 	%digit_r, 5, %g0
	be 	fives_loop
	nop

	subcc 	%digit_r, 4, %g0
	be 	jsr_loop
	nop

	srl 	%inst_r, 8, %digit_r
	tst 	%digit_r
	bg 	branch_loop
	nop
	
	tst 	%inst_r
	be 	halt_loop
	nop
	
	subcc 	%inst_r, 2, %g0
	be 	rti_loop
	nop
	
	mov 	%inst_r, %o0
	call 	get_digit
	mov 	2, %o1
	
	mov 	%o0, %digit_r
	
	subcc 	%digit_r, 2, %g0
	be 	rts_loop
	nop
	
	subcc 	%digit_r, 1, %g0
	be 	jmp_loop
	nop

!============================ default loop to end
! should never be used, assuming all instructions being
! read in are known.
! Safety back up.
	ba end_prog
	nop
!=============================================================
!
!	The following 'so called loops' are not loops.
!	Each one takes care of a case for each catagory
!	of instruction.
!==============================================================
!loop to output the instruction if a seven is in #4 digit
sevens_loop:
! get the first digit and save it into the register
	mov	%inst_r, %o0
	call	get_digit
	mov	2, %o1
	sll	%o0, 1, %reg_r
	ldsh	[%reg_r+%PDP_r], %first_r
! get the second and the destination address
	call 	get_operand
	mov	SECOND_OP, %o0
	mov	%o0, %destination_r		!gets the return address
	ldsh	[%destination_r], %second_r 	!gets the second operand
	
	add	%reg_r, %PDP_r, %destination_r
	
	mov 	%inst_r, %o0
	call	get_digit
	mov	3, %o1		! gets the index value
	
	sll	%o0, 2, %o0
	set 	sevs_table, %o1	!jump table
	ld 	[%o0+%o1], %o0	!pointer
	jmpl 	%o0, %g0		!transfer control
	nop
	
sevs_table:
	.word s0, s1, s2, s3, s4
	
s0:!mul =====================================================
	mov 	%first_r, %o0
	call 	.mul
	mov 	%second_r, %o1
	mov 	%o0, %answer_r
	
	srl	%inst_r, 6, %digit_r
	and	%digit_r, 1, %digit_r
	cmp	%digit_r, %g0
	be	store_word
	nop
	stuh	%answer_r, [%destination_r]
	ba	fetch
	mov	%answer_r, %condition_r
store_word:	
	st	%answer_r, [%destination_r]
	ba	fetch
	mov	%answer_r, %condition_r
s1:!div ======================================================
/* 	Quotient of the division goes to Rn, the remainder
  	goes to Rn+1.
*/
	mov %first_r, %o0
	call .rem
	mov %second_r, %o1

	stsh  %o0, [%destination_r+2]
	
	mov %first_r, %o0
	call .div
	mov %second_r, %o1

	stsh  %o0, [%destination_r]
	ba fetch
	nop
s2:!ash =======================================================
s3:!ashc ======================================================
s4:!xor =======================================================
	xor	%first_r, %second_r, %answer_r
	ba	fetch
	stsh	%answer_r, [%destination_r]
!==============================================================	
!==============================================================
!code to ouput the instruction if it is a five in #3
fives_loop:
!  These have the possibility to be byte operations, but it was
!  left out until later.
!  get the operand and the destination address.
	call 	get_operand
	mov 	SECOND_OP, %o0
	
	mov 	%o0, %destination_r
	tst	%inst_r
	bl	load_fives_byte
	nop
	ba	fives_cont
	ldsh	[%destination_r], %second_r
load_fives_byte:
	ldsb	[%destination_r], %second_r
	
fives_cont:
	mov	%inst_r, %o0
	call 	get_digit
	mov 	2, %o1		! get the third digit, determines the instr.
	sll 	%o0, 2, %o0		! adjust for word offset of index
	set 	fives_table, %o1
	ld 	[%o0+%o1], %o0
	jmpl 	%o0, %g0
	nop
fives_table: !====================================================
	.word F0, F1, F2, F3, F4, F5, F6, F7
F0: ! clr !=======================================================
	ba 	end_fives
	mov 	%g0, %answer_r
F1: ! complement (com) !==========================================
	ba 	end_fives
	neg 	%second_r, %answer_r
F2: ! inc !=======================================================
	ba 	end_fives
	add 	%second_r, 1, %answer_r
F3: ! dec !=======================================================
	ba 	end_fives
	sub 	%second_r, 1, %answer_r
F4: ! negate (neg) !==============================================
	ba 	end_fives
	neg 	%second_r, %answer_r
F5: ! adc (add carry ?? not needed ) !============================
F6: ! sbc (sub carry, also not needed ) !=========================
F7: ! tst !=======================================================
    ! Nothing needs saved, just set the codes, then loop again
	ba 	fetch
	mov 	%second_r, %condition_r
end_fives:
	mov 	%answer_r, %condition_r
	ba	fetch
	stsh 	%answer_r, [%destination_r]
!==============================================================
!code to output operand if a twos instruction
twos_loop:
!  get the first operand.
	call 	get_operand
	mov 	FIRST_OP, %o0
	mov	%o0, %first_r
!  get the second operand and the destination
	call 	get_operand
	mov	SECOND_OP, %o0
	mov	%o0, %destination_r

!  branch if byte operation
	cmp 	%inst_r, %g0
	bge 	ld_half
	nop
	ldsb	[%first_r], %first_r
	ba 	twos_cont
	ldsb	[%destination_r], %second_r
ld_half:	! =======================================================
	ldsh	[%first_r], %first_r
	ldsh	[%destination_r], %second_r
twos_cont:
	mov	 %inst_r, %o0
	call 	get_digit
	mov 	4, %o1		! get the third digit, determines the instr.
	sll 	%o0, 2, %o0		! adjust for word offset of index
	set 	twos_table, %o1
	ld 	[%o0+%o1], %o0
	jmpl 	%o0, %g0
	nop
	
twos_table:
	.word T0, T1, T2, T3, T4, T5, T6
T0: ! not used  =======================================================

T1: ! mov (b) =======================================================
	cmp	%inst_r, %g0
	bge	mov_half
	nop
	ba 	fetch
	stb	%first_r, [%destination_r]
mov_half: !=======================================================
	ba	fetch
	stsh    %first_r, [%destination_r]
T2: ! cmp (b) =======================================================
! byte cmp is not implemented here, yet
	ba 	fetch
	sub 	%first_r, %second_r, %condition_r
T3: ! bit (b) =======================================================
	ba	 fetch
	and 	%first_r, %second_r, %condition_r
T4: ! bic (b) =======================================================
	andn	%second_r, %first_r, %answer_r
	stsh	%answer_r, [%destination_r]
	ba 	fetch
	mov  	%answer_r, %condition_r
T5: ! bis (b) =======================================================
	or	%first_r, %second_r, %answer_r
	stsh	%answer_r, [%destination_r]
	ba	fetch
	mov 	%answer_r, %condition_r

T6: ! add or sub (depends on sign of instr) =========================
	cmp 	%inst_r, %g0
	bl	subtract
	nop
	! else add
	add 	%first_r, %second_r, %answer_r
	stsh	%answer_r, [%destination_r]
	ba 	fetch
	mov 	%answer_r, %condition_r	
subtract: !=======================================================
	sub 	%second_r, %first_r, %answer_r
	stsh	%answer_r, [%destination_r]
	ba 	fetch
	mov 	%answer_r, %condition_r
!==============================================================
!branch instructions
branch_loop:
! read in the branch to value awhile and adjust
	sll	%inst_r, 24, %destination_r
	sra	%destination_r, 23, %destination_r
	add	%destination_r, %pc_r, %destination_r	! could be the new pc address!!
! get the branch table index and jump (...jump up, jump down, jump around)
	srl	%inst_r, 8, %value_r
	sll	%value_r, 2, %value_r
	set	branch_table, %o1
	ld	[%o1+%value_r], %o0
	jmpl	%o0, %g0
	nop
branch_table:
	.word	B0, B1, B2, B3, B4, B5, B6, B7

B0: ! not used

B1: ! br (always) =======================================================
	mov 	%destination_r, %pc_r
	ba	fetch
	stsh	%destination_r, [%PDP_r+7*2]
B2: ! bne =======================================================
	cmp 	%condition_r, %g0
	be	fetch
	nop
	mov 	%destination_r, %pc_r
	ba	fetch
	stsh	%destination_r, [%PDP_r+7*2]
B3: ! beq =======================================================
	cmp	%condition_r, %g0
	bne	fetch
	nop
	mov 	%destination_r, %pc_r
	ba	fetch
	stsh	%destination_r, [%PDP_r+7*2]
B4: ! bge =======================================================
	cmp	%condition_r, %g0
	bl	fetch
	nop
	mov 	%destination_r, %pc_r
	ba	fetch
	stsh	%destination_r, [%PDP_r+7*2]
B5: ! blt =======================================================
	cmp	%condition_r, %g0
	bge	fetch
	nop
	mov 	%destination_r, %pc_r
	ba	fetch
	stsh	%destination_r, [%PDP_r+7*2]
B6: ! bgt =======================================================
	subcc	%condition_r, %g0, %g0
	ble	fetch
	nop
	ba	fetch
	stsh	%destination_r, [%PDP_r+7*2]
B7: ! ble =======================================================
	cmp	%condition_r, %g0
	bg	fetch
	nop
	mov 	%destination_r, %pc_r
	ba	fetch
	stsh	%destination_r, [%PDP_r+7*2]
!==============================================================
jsr_loop:
	ldsh	[%PDP_r+6*2], %value_r
	sub	%value_r, 2, %value_r	!auto decrement
	stsh	%value_r, [%PDP_r+6*2]
	add	%value_r, %prog_r, %destination_r! store the address of stack
	mov 	%inst_r, %o0
	call 	get_digit
	mov	2, %o1			! get reg number
	sll	%o0, 1, %reg_r		! shift adjustment
	ldsh	[%PDP_r+%reg_r], %answer_r
	stsh	%answer_r, [%destination_r]
! Rn is now stored onto the stack, next get the destination value of the jump
	call	get_operand
	mov	0, %o0
	mov	%o0, %destination_r
! gets the address of the jump operand, now mov R7 into Rn
	ldsh	[%PDP_r+7*2], %pc_r
	stsh	%pc_r, [%PDP_r+%reg_r]
! give the pc it's new value
	sub	%destination_r, %prog_r, %destination_r
! store the pc
	ba	fetch
	stsh	%destination_r, [%PDP_r+7*2]
!==============================================================
halt_loop:
	ba end_prog
	nop
!==============================================================
rti_loop:
! Program counter is popped off stack, as are the condition codes (n/z)
! Fist get the pc
	! auto increment on R6 to get the value from the stack
	ldsh	[%PDP_r+6*2], %value_r
	add	%value_r, 4, %o0
	stsh	%o0, [%PDP_r+6*2]
	add	%value_r, %prog_r, %destination_r
	ldsh	[%destination_r], %condition_r
	inc	2, %destination_r
	ldsh	[%destination_r], %pc_r
	ba	fetch
	stsh	%pc_r, [%PDP_r+7*2]
	
!==============================================================
jmp_loop:
! 	jump up, jump down, jump around,... jump, jump
! get the destination
	call 	get_operand
	mov	SECOND_OP, %o0
	mov	%o0, %destination_r
	
	ldsh	[%destination_r], %value_r
	add	%value_r, 2, %value_r
	ldsh	[%PDP_r+7*2], %pc_r
	add	%pc_r, %value_r, %destination_r
	
	stsh	%destination_r, [%PDP_r+7*2]
	ba 	fetch
	mov	%destination_r, %pc_r
!==============================================================
rts_loop:

! auto increment on R6 to get the value from the stack
	ldsh	[%PDP_r+6*2], %value_r
	add	%value_r, 2, %o0
	stsh	%o0, [%PDP_r+6*2]
	add	%value_r, %prog_r, %destination_r
	ldsh	[%destination_r], %value_r		!get the Rn value
! pc gets the value in Rn
	and 	%inst_r, 7, %reg_r	! get the register number
	sll	%reg_r, 1, %reg_r	! shift for offset adjustment
	ldsh	[%PDP_r+%reg_r], %pc_r	! get new pc value
	stsh	%pc_r, [%PDP_r+7*2]
	ba	fetch
	stsh	%value_r, [%PDP_r+%reg_r]
!===================================================================
!===================================================================
interupt:
! output the data...???
	sub	%g0, 1, %counter_r
	
	mov	%g0, %o0
	add	%prog_r, 0172, %o1
	mov	1, %o2
	mov	4, %g1
	ta	0

	ldsh	[%prog_r-2], %value_r	! the interupt handler value
! place the condition codes onto the stack.
	ldsh	[%PDP_r+6*2], %destination_r
	sub	%destination_r, 4, %destination_r
	stsh	%destination_r, [%PDP_r+6*2]
	add	%prog_r, %destination_r, %destination_r
	stsh	%condition_r, [%destination_r]
	inc	2, %destination_r
	ldsh	[%PDP_r+7*2], %pc_r
	stsh	%pc_r, [%destination_r]
	stsh	%value_r, [%PDP_r+7*2]	

	ba	fetch
	nop

!===================================================================
!===================================================================
end_prog:				! convenient ending breakpoint
	end_main
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
	define(`fd_r', l1)
	local_var
	var(line, 1, 8)
	begin_fn(read_file)
	mov	%i0, %o0		! set up call to
	clr	%o1			!    open file
	clr	%o2			!    (just as in the text)
	mov	5, %g1
	ta	0			! and make the system call
	mov	%o0, %fd_r		! save the file pointer
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
	mov	%fd_r, %o0		! go back for more data
done_read:
	end_fn(read_file)
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
	local_var
	define( `conversion', 48 )
	define( `in_ptr',     l1 )
	define( `total_r',    l0 )
	define( `temp_r',     l2 )
	begin_fn( conv_oct )
	mov 	0, %total_r			!clears total
	mov 	%i0, %in_ptr		!sets the string ptr
convert_start:
	ldub 	[%in_ptr], %temp_r		!load in the char
	subcc 	%temp_r, conversion, %temp_r	!convert to integer value
	bl 	convert_end
	inc 	%in_ptr				!increment input ptr
	add 	%temp_r, %total_r, %total_r		!add to the total
	ba 	convert_start
	sll 	%total_r, 3, %total_r		!shift to finish the conversion.
convert_end:
	srl 	%total_r, 3, %total_r		!adjust for extra shift
	mov 	%total_r, %i0			!return the answer
	mov 	%in_ptr, %i1			!return the pointer

	end_fn( conv_oct )
!===================================
!	End of conversion function
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
	local_var
	define( `moden_r',  l0 )	! mode number
	define( `place_r',  l0 )	! not used with mode
	define( `regn_r',   l1 )	! register
	define( `x_r',      l2 )	! value of x (displacement)
	define( `address_r',l3 )	! current address
	define( `isbyte_r', l4 )	! is this a byte operation
	define( `isodd_r',  l5 )	! is this an odd mode
	define( `whichop_r',i0 )	! explained above
	define( `return_r', i0 )	! return address

begin_fn(get_operand)
! Get the mode and register numbers
	sll 	%whichop_r, 1, %whichop_r		! which_op x2
	mov 	%whichop_r, %o1
	call 	get_digit		! get the register
	mov 	%inst_r, %o0
	sll	%o0, 1, %regn_r		! mov and adjust
	add 	%whichop_r, 1, %o1
	call 	get_digit		! get the mode
	mov 	%inst_r, %o0
	mov 	%o0, %moden_r
	clr 	%isbyte_r		! clear other variables
	clr 	%isodd_r
get_op_start:
	and 	%moden_r, 1, %isodd_r		!is it deferred?
	sub 	%moden_r, %isodd_r, %moden_r	!get even mode 
	tst 	%inst_r
	bg 	op_cont
	nop
	mov 	1, %isbyte_r
op_cont:	! determine which type of mode we are dealing with
	subcc 	%moden_r, 1, %g0
	ble 	direct
	nop
	subcc 	%moden_r, 4, %g0
! if <0, then mode==2, ==0 means mode=4, and otherwise, mode 6
	bl 	auto_inc
	nop
	be 	auto_dec
	nop
	ba 	displace
	nop
direct:

	ba 	have_pointer
	add 	%regn_r, %PDP_r, %address_r
auto_inc:

	ldsh 	[%regn_r+%PDP_r], %address_r	!address points to operand
	add 	%address_r, 2, %x_r
! if a byte operation, only add 1, so we sub isbyte from x
	sub 	%x_r, %isbyte_r, %x_r
	stsh 	%x_r, [%regn_r+%PDP_r]
	ba 	have_pointer		
	add 	%address_r, %prog_r, %address_r
auto_dec:

	ldsh 	[%PDP_r+%regn_r], %x_r
	sub 	%x_r, 2, %address_r
! adjust for byte mode
	add 	%address_r, %isbyte_r, %address_r
	stsh 	%address_r, [%regn_r+%PDP_r]
	ba 	have_pointer		
	add 	%address_r, %prog_r, %address_r
displace:

	ldsh 	[%PDP_r+7*2], %pc_r
	ldsh 	[%pc_r+%prog_r], %x_r
	inc 	2, %pc_r
	stsh 	%pc_r, [%PDP_r+7*2]
	ldsh 	[%PDP_r+%regn_r], %address_r
	add 	%address_r, %x_r, %address_r
	add 	%address_r, %prog_r, %address_r
	cmp	%isodd_r, 1
	be	have_pointer
	nop
	ba	have_address
	nop
have_pointer:
	tst	%isodd_r	! test if it's a deferred mode
	be	have_address	
	nop
	ldsh 	[%address_r], %x_r
	add 	%prog_r, %x_r, %address_r
have_address:
	mov 	%address_r, %return_r
	end_fn(get_operand)
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

	local_var
	
	define( `conversion', 48 )
	define( `total_r',    l0 )
	define( `temp_r',     l2 )
	define( `shift_r',    l3 )
	
	begin_fn(out_num)
out_num_start:
	mov 	%i0, %total_r		!get the number
	mov 	12, %shift_r		!number of shifts
	tst 	%total_r
	bge 	positive
	nop
	call 	putchar
	mov 	"-", %o0
	neg 	%total_r, %total_r
positive:
	cmp 	%total_r, 500 
	bl  	out_num_loop
	nop
	mov 	18, %shift_r
out_num_loop:
	sub 	%shift_r, 3, %shift_r	!adjust for extra shift
	srl 	%total_r, %shift_r, %temp_r !shift
	call 	putchar			!output the data
	add 	%temp_r, conversion, %o0	!get char code
	sll 	%temp_r, %shift_r, %temp_r	!shift back to subtract
		
	tst 	%shift_r			!test if done
	bg  	out_num_loop		!return to loop again
	sub 	%total_r, %temp_r, %total_r	!get the lower total
						!  for other digits	
	end_fn(out_num)
!===========================================
!	End of output number function
!===========================================

