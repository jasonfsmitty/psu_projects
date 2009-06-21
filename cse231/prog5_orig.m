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
	.asciz	"br ","bne","beq","bge","blt","bgt","ble"	! ___/off
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

	define(`eprog_r', l1)		! marks end of program



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
!	call	putchar			! putchar and fflush would be
!	mov	":", %o0		!    use if the line is output
!	call	putchar			!    piecemail.  Alternatively
!	mov	"\t", %o0		!    one could store the chars
!	call	fflush			!    in memory and 'write' later
!	clr	%o0
	ldsh	[%prog_r+%iptr_r],%inst_r ! fetch instruction
	inc	2, %iptr_r		!    and increment pointer

	!	The gist of the disassembler is here

end_line:
	call	putchar			! output newline
	mov	"\n", %o0		!    (again, one could write this
	call	fflush			!     to memory, and just use 'write')
	clr	%o0
test:	cmp	%iptr_r, %eprog_r	! while (iptr < end)
	bl	mloop
	nop
	end_main

	!  Read_file
	!  Given the name of the file (from the command line)
	!  and a pointer to a program space, reads the data from
	!  the file, converts it into binary, and stores it in
	!  that space.  This uses the function from the preceding
	!  assignment to convert a number from Octal ASCII to binary.
	!
	!  Input:	%i0 = name of the file
	!		%i1 = space in memory to store into
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
	!
	.data
mode:	.ascii '\0\0Rn\0\0\0\0\0(Rn)+\0\0-(Rn)\0\0\0X(Rn)\0\0\0'
!mode:	.ascii '  Rn     (Rn)+  -(Rn)   X(Rn)   '
	.text


	begin_fn(out_op)


	end_fn(out_op)
