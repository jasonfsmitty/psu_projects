/*
		JASON F SMITH
		CSE 231
		
		NOTE:	This program is not commented
			much, but it works.
			
		I think that it would be incredibly
		useful, and helpful if Roger could
		supply us with some sample files
		and the correct output.  We sat here
		and stabbed in the dark trying to
		figure out the correct instructions
		out of the files provided.  It took
		me probably an extra two hours of work
		just because I had no clue if my output
		was right or not, and it turns out
		(if I decoded the dis.in and fac.in
		files correctly) that I was right all
		along.			
		
		All output in base eight.
*/


include(macro_defs.m)
	!  This program will disassemble a PDP-ll program and
	!  display it on the screen.  The input is given in a
	!  file as a list of octal halfwords.  Some instructions
	!  may use two halfwords, if the PC is used in an operand.
	!
	!  Before anything else is done, the program to disassemble
	!  will be read from the file and stored into the following
	!  space.  The disassembler will then read information
	!  out of memory.
	.bss
	.align 2
	.skip	2				! this is used later
prog:	.skip	180
	!
	!  Here follows a list of the operands to disassemble
	!  this is not a complete list, but it includes all
	!  the operations that will be required for the emulator
	!  (plus a few that will not be)
	!
	!  Many of these are set up as arrays of four-byte
	!  strings to make for very easy indexing.  For example,
	!  the MUL instruction is decoded as 070RDD, with the
	!  second zero useable as a subscript to sevens[].
	!
	!  The comments at the end of each line below describe
	!  the instruction formats.  "R" specifies a simple
	!  register operand.  "SS" and "DD" indicate operands
	!  with an addressing mode and a register (in that order)
	!  Underscores indicate potential array subscripts.
	!  A "B" indicates byte operands (if 1, add a "b" to the
	!  opcode)
	!
	!  It may be assumed that all instructions in the input
	!  are among those which follow, if that simplifies the
	!  program
	!
	.data
twos:	.word	0						! B_SSDD
	.asciz	"mov","cmp","bit","bic","bis","add"
sevens:	.asciz	"mul","div","ash"				! 07_RDD
fives:	.asciz  "clr","com","inc","dec","neg","adc","sbc","tst"	! B05_DD
branch:	.word	0
	.asciz	"br ","bne","beq","bge","blt","bgt","ble"	!___/off
jmp:	.asciz	"jmp"						! 0001DD
jsr:	.asciz	"jsr"						! 004RDD
rts:	.asciz	"rts"						! 00020R
rti:	.asciz  "rti"						! 000002
halt:	.ascii  "halt"						! 000000
	.text

	!  The following defines have been provided to give some
	!  suggested register usage.  It also provides a degree of
	!  uniformity in program structure to simplify debugging
	!  during office hours.

	define(`prog_r', g2)		! global pointer to input
	define(`iptr_r', g3)		! global instruction pointer
					!    (as an offset off of prog_r)
	define(`inst_r', g4)		! global instruction

	define(`twos_ptr', l4)		! for easy access
	define(`sevs_ptr', l5)		!     to the static arrays
	define(`fives_ptr', l6)
	define(`branch_ptr', l7)

	define(`eprog_r', l1 )		! marks end of program
	define(`digit_r', l2 )		! the digit read back
	define(`WRITE',    4 )		!for write call
	define(`num_char', 4 )		!number of chars to write
	define(`conversion', 48 )	
	define(`temp_r',  l3 )
	define(`reg_r',   l0 )
	
	begin_main
start:
	set	prog, %o1		! read file into program space
	call	read_file
	ld	[%i1+4], %o0		!    file name = argv[1]
	mov	%o1, %eprog_r		! save end of program

	set	twos, %twos_ptr		! initialize static registers
	set	sevens, %sevs_ptr
	set	fives, %fives_ptr
	set	branch, %branch_ptr
	set	prog, %prog_r		! point at top of program code
	sub	%o1, %prog_r, %eprog_r	! calculate length of program code
	clr	%iptr_r			! for (ipc = 0; iptr<=eprog; iptr++) 
mloop:



	call	out_num			! let's see the address
	mov	%iptr_r, %o0		!    
	call	putchar			! putchar and fflush would be
	mov	":", %o0		!    use if the line is output
	call	putchar			!    piecemail.  Alternatively
	mov	"\t", %o0		!    one could store the chars
	call	fflush			!    in memory and 'write' later
	clr	%o0
	ldsh	[%prog_r+%iptr_r],%inst_r ! fetch instruction
	inc	2, %iptr_r		!    and increment pointer

	!	The gist of the disassembler is here
	!	Determine which catagory of instruction this is.
	!	These statements are a big if/else section.
	!	Which type of catagory is the instruction?
				
	!setup for call to get the number four digit
	mov %inst_r, %o0
	call get_digit
	mov 4, %o1
	
	mov %o0, %digit_r	!stick it @#$#%
	
!check if sevens
	subcc %digit_r, 7, %g0			!check for sevens
	be sevens_loop
	nop
	
	tst %digit_r				!twos if digit!=0
	bne twos_loop				!check if twos
	nop
	
	mov %inst_r, %o0			!number 3 digit
	call get_digit
	mov 3, %o1
	mov %o0, %digit_r
	
	subcc %digit_r, 5, %g0
	be fives_loop
	nop

	subcc %digit_r, 4, %g0
	be jsr_loop
	nop

	srl %inst_r, 8, %temp_r
	tst %temp_r
	bg branch_loop
	nop
	
	tst %inst_r
	be halt_loop
	nop
	
	subcc %inst_r, 2, %g0
	be rti_loop
	nop
	
	mov %inst_r, %o0
	call get_digit
	mov 2, %o1
	
	mov %o0, %digit_r
	
	subcc %digit_r,	2, %g0
	be rts_loop
	nop
	
	subcc %digit_r, 1, %g0
	be jmp_loop
	nop

!============================ default loop to end
! should never be used, assuming all instructions being
! read in are known.
! Safety back up.
	ba end_line
	nop
	
!==============================================================
/*
	The following 'so called loops' are not loops.
	Each one takes care of a case for each catagory
	of instruction.  It's way too late (or early)
	to attempt to comment everything.
	Basically - each loop outputs the instruction,
	and then the operand(s) if needed.
*/
!==============================================================
!loop to output the instruction if a seven is in #4 digit
sevens_loop:

	mov %inst_r, %o0
	call get_digit
	mov 3, %o1
	
	sll %o0, 2, %digit_r
	add %digit_r, %sevs_ptr, %o1
	mov num_char, %o2
	mov WRITE, %g1
	ta 0				!call write

	call putchar
	mov "\t", %o0
	call putchar
	mov "R", %o0

	!get the register output
	mov %inst_r, %o0
	call get_digit
	mov 2, %o1

	call putchar
	add %o0, conversion, %o0
	call putchar
	mov ",", %o0
	call putchar
	mov " ", %o0
	
	call fflush
	clr %o0	
	
	mov %inst_r, %o0
	call get_digit
	mov 0, %o1
	
	mov %o0, %reg_r
	
	mov %inst_r, %o0
	call get_digit
	mov 1, %o1
	

	call out_op
	mov %reg_r, %o1
	
	
	ba end_line
	nop
	
!==============================================================
!code to ouput the instruction if it is a five in #3
fives_loop:
	mov %inst_r, %o0
	call get_digit
	mov 2, %o1
	mov %o0, %digit_r
	
	clr %o0
	
	sll %digit_r, 2, %digit_r
	add %digit_r, %fives_ptr, %o1
	mov num_char, %o2
	mov WRITE, %g1
	ta 0

	cmp %inst_r, %g0
	bge end_fives
	nop
	
	call putchar
	mov "b", %o0
	
end_fives:
	call putchar
	mov "\t", %o0
	
	call fflush
	clr %o0
	
	mov %inst_r, %o0
	call get_digit
	mov 0, %o1
	
	mov %o0, %reg_r
	
	mov %inst_r, %o0
	call get_digit
	mov 1, %o1
	
	call out_op
	mov %reg_r, %o1

	ba end_line
	nop
!==============================================================
!code to output operand if a twos instruction
twos_loop:
	mov %inst_r, %o0
	call get_digit
	mov 4, %o1
	
	sll %o0, 2, %digit_r
	clr %o0
	add %digit_r, %twos_ptr, %o1
	mov num_char, %o2
	mov WRITE, %g1
	ta 0

	cmp %inst_r, %g0
	bge end_twos
	nop

	call putchar
	mov "b", %o0
end_twos:
	call putchar
	mov "\t", %o0
	
	call fflush
	clr %o0
	
	mov %inst_r, %o0
	call get_digit
	mov 2, %o1
	
	mov %o0, %reg_r
	
	mov %inst_r, %o0
	call get_digit
	mov 3, %o1
	
	call out_op
	mov %reg_r, %o1

	call putchar
	mov ",", %o0
	call putchar
	mov " ", %o0
	call fflush
	clr %o0

	mov %inst_r, %o0
	call get_digit
	mov 0, %o1
	
	mov %o0, %reg_r
	
	mov %inst_r, %o0
	call get_digit
	mov 1, %o1
	
	call out_op
	mov %reg_r, %o1


	ba end_line
	nop
!==============================================================
!branch instructions
branch_loop:
	clr %o0
	srl %inst_r, 8, %temp_r
	sll %temp_r, 2, %temp_r
	add %temp_r, %branch_ptr, %o1
	!dec %o1
	mov num_char, %o2
	mov WRITE, %g1
	ta 0	

	sll %inst_r, 24, %temp_r
	sra %temp_r, 23, %temp_r
	
	call putchar
	mov "\t", %o0
	call fflush
	clr %o0
	
	call out_num
	add %temp_r, %iptr_r, %o0


	ba end_line
	nop
!==============================================================
jsr_loop:
	clr %o0
	set jsr, %o1
	mov num_char, %o2
	mov WRITE, %g1
	ta 0
	
	call putchar
	mov "\t", %o0
	call fflush
	clr %o0
	
	mov %inst_r, %o0
	call get_digit
	mov 2, %o1
	
	mov %o0, %o1
	call out_op
	clr %o0
	
	call putchar
	mov ",", %o0
	call putchar
	mov " ", %o0
	call fflush
	clr %o0
	
	mov %inst_r, %o0
	call get_digit
	mov 0, %o1
	
	mov %o0, %reg_r
	
	mov %inst_r, %o0
	call get_digit
	mov 1, %o1
	
	call out_op
	mov %reg_r, %o1

	
	ba end_line
	nop
!==============================================================
halt_loop:
	clr %o0
	set halt, %o1
	mov num_char, %o2
	mov WRITE, %g1
	ta 0

	ba end_line
	nop
!==============================================================
rti_loop:
	clr %o0
	set rti, %o1
	mov num_char, %o2
	mov WRITE, %g1
	ta 0

	ba end_line
	nop
!==============================================================
jmp_loop:
	clr %o0
	set jmp, %o1
	mov num_char, %o2
	mov WRITE, %g1
	ta 0
	
	call putchar
	mov "\t", %o0
	
	call fflush
	clr %o0
	
	mov %inst_r, %o0
	call get_digit
	mov 0, %o1
	
	mov %o0, %reg_r
	
	mov %inst_r, %o0
	call get_digit
	mov 1, %o1
	
	call out_op
	mov %reg_r, %o1
	
	ba end_line
	nop
!==============================================================
rts_loop:
	clr %o0
	set rts, %o1
	mov num_char, %o2
	mov WRITE, %g1
	ta 0
	
	call putchar
	mov "\t", %o0
	
	call fflush
	clr %o0
	
	mov %inst_r, %o0
	call get_digit
	mov 0, %o1
	
	mov %o0, %o1
	call out_op
	clr %o0
	
	ba end_line
	nop
!==============================================================
!==============================================================
end_line:
	call	putchar			! output newline
	mov	"\n", %o0		!    (again, one could write this
	call	fflush			!     to memory, and just use 'write')
	clr	%o0			! your comments are not welcome
test:	cmp	%iptr_r, %eprog_r	! while (iptr < end)
	bl	mloop
	nop
	end_main
!================================================
!	End of main function
!================================================



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

!=======================================================
!  Output Operand
!  This procedure will be used to output a single
!  PDP-11 operand, as specified by a register and
!  an addressing mode.  The variable mode is defined
!  as    char mode[4][8]	for easy reference.
!
!  Input	%i0 = addressing mode	(0-7)
!		%i1 = register		(0-7)
!
!  Note:  The mode array is now stored as a static
!  variable, not a stack variable.  Since this function
!  will be called many times, it is much more efficient
!  to allocate and initialize it only once.  Furthermore,
!  it's much easier to put spaces into the string this way.
!
!  Whatever was used to get the 'mode' pointer before,
!  (adding %fp to the mode offset), should not use the
!  'set' instruction to point at this.
!==============================================================

	.data
mode:	.ascii '\0\0\0Rn\0\0\0\0\0(Rn)+\0\0-(Rn)\0\0\0X(Rn)\0\0\0'
!mode:	.ascii '   Rn     (Rn)+  -(Rn)   X(Rn)   '
	.text


	local_var
	
	define( `mode_r', l0 )
	define( `reg_r', l1 )
	
	define( `mode_ptr', l2 )
	define( `isodd_r', l3  )
	define( `back_r', l4 )
	define( `aster',  l5 )
	
	define( `op_char', 8 )
	define( `conversion', 48 )
	
	begin_fn(out_op)
op_start:
	set mode, %mode_ptr
	mov %i0, %mode_r
	mov %i1, %reg_r
	mov "*", %aster
	and 1, %mode_r, %isodd_r
	
!determine what type of output is needed
	cmp %reg_r, 7	!program counter, special fn
	be  pc_output
	nop
	
	cmp %mode_r, 6		!mode 6 or 7
	bge displacement	!replace x
	nop
	
!else, normal output
!just like last program

	sub %mode_r, %isodd_r, %mode_r
	sll %mode_r, 2, %mode_r
	
	add %mode_ptr, %mode_r, %mode_ptr
	add 4, %mode_ptr, %mode_ptr
	
	add conversion, %reg_r, %reg_r
	
	stb %reg_r, [%mode_ptr]
	
	sub %mode_ptr, 4, %mode_ptr
	
	tst %isodd_r
	ble op_finish
	nop
	
	stb %aster, [%mode_ptr]
	nop
	
op_finish:
	clr %o0
	mov %mode_ptr, %o1
	mov op_char, %o2
	mov WRITE, %g1
	ta 0
	
	mov "\0", %aster
	stb %aster, [%mode_ptr]
	
	ba out_done
	nop
!====================================	
displacement:
! replace X with something called a 'number'
	tst %isodd_r
	be next_dis
	nop
	
	call putchar 
	mov %aster, %o0
	call fflush
	clr %o0		
next_dis:
	
	ldsh	[%prog_r+%iptr_r],%inst_r ! fetch instruction
	inc	2, %iptr_r		!    and increment pointer
	
	call out_num
	mov %inst_r, %o0
	
	call putchar
	mov "(", %o0
	call putchar
	mov "R", %o0
	call putchar
	add %reg_r, conversion, %o0
	call putchar
	mov ")", %o0
	call fflush
	clr %o0	
	
	ba out_done
	nop
!====================================
! using program counter register (R7)
!  z zZZZzzzz zzzzZZZ zzZ (i wish)	
pc_output:
	ldsh	[%prog_r+%iptr_r],%inst_r ! fetch instruction
	inc	2, %iptr_r		!    and increment pointer
	
	tst %isodd_r
	be next_pc
	nop
	
	call putchar 
	mov %aster, %o0
	call fflush
	clr %o0		
next_pc:
	cmp %mode_r, 2
	ble pc_two
	nop
!===============================
!else the mode is either 6 or 7

	add %iptr_r, %inst_r, %o0

	call out_num
	nop
	
	ba out_done
	nop
	
pc_two:
	call putchar
	mov "#", %o0
	call fflush
	clr %o0
	
	call out_num
	mov %inst_r, %o0
	
	ba out_done
	nop
!====================================
out_done:
	end_fn(out_op)
!==========================================
!	End of output operand function
!==========================================

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

	mov 0, %total_r			!clears total
	mov %i0, %in_ptr		!sets the string ptr
convert_start:
	ldub [%in_ptr], %temp_r		!load in the char
	subcc %temp_r, conversion, %temp_r	!convert to integer value
	bl convert_end
	inc %in_ptr				!increment input ptr
	add %temp_r, %total_r, %total_r		!add to the total
	ba convert_start
	sll %total_r, 3, %total_r		!shift to finish the
						!conversion.
convert_end:
	srl %total_r, 3, %total_r		!adjust for extra shift
	mov %total_r, %i0			!return the answer
	mov %in_ptr, %i1			!return the pointer

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
	local_var
	define( `and_with',  7 )
	define( `number',   i0 )
	define( `n',        i1 )

	begin_fn( get_digit )
digit_start:
	mov %n, %o0
	call .mul	
	mov 3, %o1
	
	srl %number, %o0, %number
	and %number, and_with, %number

	end_fn( get_digit )

!===================================================
!	End of digit extracting function.
!===================================================

!===========================================================
!	Output number function
!	This function prints the number out to the screen.
!	Outputs the number in octal form.
!	Input:	%o0 - The number to output.
!	Output:	(to screen) the number in octal form.
!===========================================================

	local_var
	
	define( `conversion', 48 )
	define( `total_r',    l0 )
	define( `temp_r',     l2 )
	define( `shift_r',    l3 )
	
	begin_fn( out_num )
out_num_start:
	mov %i0, %total_r		!get the number
	
	mov 12, %shift_r		!number of shifts
	
	
	tst %total_r
	bge positive
	nop
	
	call putchar
	mov "-", %o0
	
	neg %total_r, %total_r
	
positive:
	cmp %total_r, 500 
	bl  out_num_loop
	nop
	
	mov 18, %shift_r
	
out_num_loop:
	sub %shift_r, 3, %shift_r	!adjust for extra shift
	srl %total_r, %shift_r, %temp_r !shift
	call putchar			!output the data
	add %temp_r, conversion, %o0	!get char code
	sll %temp_r, %shift_r, %temp_r	!shift back to subtract
		
	tst %shift_r			!test if done
	bg  out_num_loop		!return to loop again
	sub %total_r, %temp_r, %total_r	!get the lower total
					!  for other digits
	
	end_fn( out_num )
!===========================================
!	End of output number function
!===========================================

 
