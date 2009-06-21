 
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
        .align 4                ! make it aligned
PDPReg: .skip   2*8             ! eight 16-bit registers
        .skip   2               ! these two bytes will be used later
stack:  .skip   62              ! 62 bytes should be enough for stack
prog:   .skip   180             ! store the program here
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



	


		! general all-purpose temporary value
		! register address (offset into array)
		! source operand value
		! destination operand value
		! instruction decoding register

	.global	main
main:	save	%sp, -96, %sp
start:
        set     prog, %o1               ! read file into program space
        call    read_file
        ld      [%i1+4], %o0            !    file name = argv[1]

        set     prog, %g2           ! point at top of program code
        set     PDPReg, %g5          ! set up pointer to Registers
        st      %g0, [%g5 + 6*2]     ! clear stack pointer and program


fetch:  ldsh	[%g5 + 7*2], %g3	! read program counter
	ldsh	[%g2+%g3],%g4	! read the instruction
	inc	2, %g3
	sth	%g3, [%g5+7*2]	! increment program counter

	tst	%g4			! halt means all done
	be	done
	nop

	srl	%g4, 3*4, %l0 
	and	%l0, 7, %l4	! examine #4 digit
	cmp	%l4, 7		! if equal to 7
	be	do_sevens		!    -- mul, div, ash
	cmp	%l4, 6		! if equal to 6
	be	do_addsub		!    == add or subtract
	tst	%l4			! else if not equal to zero
	bne	do_twos			!    -- mov, cmp, ...

	srl	%g4, 3*3, %l0 
	and	%l0, 7, %l4	! examine #3 digit
	cmp	%l4, 5		! if equal to 5
	be	do_fives		!    -- clr, com, inc, dec, tst...
	cmp	%l4, 4		! if equal to 4
	be	do_jsr			!    -- jsr
	srl	%g4, 8, %l4	! examine top byte of register
	tst	%l4			! if != 0
	bne	do_branch		!    -- branch instruction
	and	%g4,0377,%l3	!    (fetch displacement on the way)
	srl	%g4, 3*2, %l0 
	and	%l0, 7, %l4 	! examine #2 digit
	cmp	%l4, 1		! and see what it is
	be	do_jmp			!    1 is JMP
	and	%g4, 7, %l0	!    (examine register on the way)
	blt	do_rti			! assume 0 is rti 
	nop
	ba	do_rts			! assume RTS  (actually 3 is SWAB,
	nop				!    and 240+ set/clear flags directly)
done:
	mov	1, %g1
	ta	 0

	!  These are the special two-operand instructions that specify
	!  a register as one operand.  They are of the form   07_RSS
	!  0 = MUL	1 = DIV		2 = ASH
do_sevens:
	srl	%g4, 3*1, %l0 
	and	%l0, 7, %o0	!  examine the source operand
	call	fetch_op		!	(  mode  )
	and	%g4, 7, %o1	!   	(register)
	ldsh	[%o0], %l2		!  load source value
	srl	%g4, 3*2, %l0 
	and	%l0, 7, %l1	!  examine register number
	sll	%l1, 1, %l1	!  read destination value
	ldsh	[%g5 + %l1], %l3	
	srl	%g4, 3*3, %l0 
	and	%l0, 7, %l4	!  identify instruction
	sll	%l4, 2, %l4		!  use instruction to
	set	sevens, %l0		!	-1 into case table
	ld	[%l0 + %l4], %l0
	jmpl	%l0, %g0		!  handle cases
	nop
sevens:	.word	MUL, DIV, ASH
MUL:	mov	%l2, %o0		!  multiply source
	call	.mul			!	times destination
	mov	%l3, %o1
	tst	%o0			!  examine product
	call	set_cc			!     and set condition codes
	clr	%l5			!     (after clearing them)
	btst	2, %l1		!  if odd destination
	bne,a	fetch
	sth	%o0, [%g5 + %l1] ! update one register
	ba	fetch			!    else store two
	st	%o0, [%g5 + %l1] !    (at the same time)
DIV:	mov	%l3, %o0
	call	.div			! [dst] = [dst] / [src]
	mov	%l2, %o1
	tst	%o0			! examine quotient
	call	set_cc			!     and set condition codes
	clr	%l5			!     (after clearing them)
	sth	%o0, [%g5 + %l1]	! stored into dest
	inc	2, %l1		! advance to next register address
	mov	%l3, %o0
	call	.rem			! [dst+1] = [dst] % [src]
	mov	%l2, %o1
	ba	fetch
	sth	%o0, [%g5 + %l1]
ASH:	sll	%l2, 32-6, %l2	! last 6 bits of operand
	sra	%l2, 32-6, %l2	!    determine the 
	tst	%l2			! positive,  left
	bge,a	stash
	sll	%l3, %l2, %l3
	neg	%l2			! negative,  right
	srl	%l3, %l2, %l3
stash:	tst	%l3			! examine the result
	call	set_cc			!    to set the condition codes
	clr	%l5
	ba	fetch
	sth	%l3,[%g5+%l1]! and store the result

	!   Add and Subtract both use word operands only
	!   so they are coded separately to keep them simple
do_addsub:
	mov	%g4, %l0	! save instruction
	and     %l0, 07777, %g4	! to nuke the sign bit
	srl	%g4, 3*3, %l0 
	and	%l0, 7, %o0
	srl	%g4, 3*2, %l0 
	and	%l0, 7, %o1	! examine source operand
	call	fetch_op
	nop
	ldsh	[%o0], %l2		! load source value
	srl	%g4, 3*1, %l0 
	and	%l0, 7, %o0
	call	fetch_op		! examine destination operand
	and	%g4, 7, %o1
	ldsh	[%o0], %l3		! load destination value
	tst	%l0			! check original instruction
	bge,a	finish_it		! Either add
	add	%l3, %l2, %l3
	ba	finish_it
	sub	%l3, %l2, %l3
	!   These are the remaining two-operand instructions
	!   Both operands are fetched, the operation is chosen,
	!   and a single catch-all section finishes them off.
do_twos:
	srl	%g4, 3*3, %l0 
	and	%l0, 7, %o0	
	srl	%g4, 3*2, %l0 
	and	%l0, 7, %o1	! examine source operand
	call	fetch_op
	nop
	tst	%g4			! if byte mode
	bl,a	bytesrc	
	ldsb	[%o0], %l2		!    load byte operand
	ldsh	[%o0], %l2		!    else word operand
bytesrc:
	srl	%g4, 3*1, %l0 
	and	%l0, 7, %o0
	call	fetch_op		! examine destination operand
	and	%g4, 7, %o1	!
	tst	%g4			! if byte mode
	bl,a	bytedst
	ldsb	[%o0], %l3		!    load byte operand
	ldsh	[%o0], %l3		!    else word operand
bytedst:
	sll	%l4, 2, %l4		! use instruction
	set	twos, %l0		!     to -1 into case table
	ld	[%l0 + %l4], %l0
	jmpl	%l0, %g0		! handle cases
	nop
twos:	.word	fetch, MOV, CMP, BIT, BIC, BIS
MOV:	ba	finish_it
	mov	%l2, %l3
CMP:	ba	finish_it+4		! don't test dval
	cmp	%l2, %l3
BIT:	ba	finish_it+4		! don't test dval
	btst	%l2, %l3
BIC:	ba	finish_it
	bclr	%l2, %l3
BIS:	ba	finish_it
	bset	%l2, %l3

	!  These are some of the common one-operand instructions.
	!  The operand is fetched, the operation is formed, and
	!  a shared section of code finishes it off.
do_fives:
	srl	%g4, 3*1, %l0 
	and	%l0, 7, %o0
	call	fetch_op		!  examine destination operand
	and	%g4, 7, %o1	! 
	tst	%g4			!  if byte mode
	bl,a	byteone
	ldsb	[%o0], %l3		!     load byte operand
	ldsh	[%o0], %l3		!     else word operand
byteone:
	srl	%g4, 3*2, %l0 
	and	%l0, 7, %l4	!  examine the opcode
	sll	%l4, 2, %l4
	set	fives, %l0		!  -1 into case table
	ld	[%l0 + %l4], %l0	
	jmpl	%l0, %g0		!  handle cases
	nop
fives:	.word	CLR, COM, INC, DEC, NEG, fetch, fetch, finish_it
CLR:	ba	finish_it
	clr	%l3
COM:	ba	finish_it
	not	%l3
INC:	ba	finish_it
	inc	%l3
DEC:	ba	finish_it
	dec	%l3
NEG:	ba	finish_it
	neg	%l3

	!   Branches -- displacement in l3, branch code in l4
	!   The flags are compared with the branch instruction
	!   directly -- Z looking at '2' bit, and N at '4'
do_branch:
	sll	%l3, 24, %l3	!  double and sign-extend
	sra	%l3, 23, %l3	!	displacement
	add	%g3, %l3, %l3	!  determine destination 
	btst	%l4, %l5		!  check PDP flags
	sll	%l4, 2, %l4		!  use opcode to
	set	branches, %l0	!	-1 into case table
	ld	[%l0 + %l4], %l0
	jmpl	%l0, %g0		!  decide how to check
	nop				!       the PDP flags
branches: .word fetch, BE, BE, BNE, BE, BNE, BE, BNE
BNE:	bne,a	fetch			!  Z set --> BEQ
	sth	%l3, [%g5 + 7*2]	!  N set --> BLT
	ba	fetch			!  (N or Z) set -> BLE
	nop
BE:	be,a	fetch			!  no check --> BR
	sth	%l3, [%g5 + 7*2]	!  Z zero   --> BNE
	ba	fetch			!  N zero   --> BGE
	nop				!  (N or Z) zero  --> BGT

	!   JMP --  R7 <-- jump address
do_jmp:
	srl	%g4, 3*1, %l0 
	and	%l0, 7, %o0
	call	fetch_op		! examine branch address
	and	%g4, 7, %o1
					! branch address is in %o0
	sub	%o0, %g2, %l3	! Unfix Sparc address
	ba	fetch
	sth	%l3, [%g5 + 7*2]	! update PC with branch address

	!   JSR --  Stack <-- Rn <-- R7 <-- subroutine address
do_jsr:
	srl	%g4, 3*1, %l0 
	and	%l0, 7, %o0
	call	fetch_op		! examine branch address
	and	%g4, 7, %o1
					! branch address is in %o0
	sub	%o0, %g2, %l3	! Unfix Sparc address
	ldsh	[%g5 + 6*2], %l1 ! prepare to push Rn onto stack
	dec	2, %l1		!
	sth	%l1, [%g5 + 6*2] ! update stack pointer
	srl	%g4, 3*2, %l0 
	and	%l0, 7, %l0	! identify register op
	sll	%l0, 1, %l0
	ldsh	[%g5 + %l0], %l2	! load value and
	sth	%l2, [%g2 + %l1]	! save on stack
	ldsh	[%g5 + 7*2], %g3		! examine latest PC 
	sth	%g3, [%g5 + %l0]	! save PC in register
	ba	fetch
	sth	%l3, [%g5 + 7*2]	! update PC with branch address

	!  RTS - Return from Subroutine   Stack -> R7 -> Rn
do_rts: sll	%l0, 1, %l0
	ldsh	[%g5+%l0], %g3		!  restore old PC
	sth	%g3, [%g5 + 7*2]		!     and save it
	ldsh	[%g5 + 6*2], %l1		!  get top of stack
	ldsh	[%g2 + %l1], %l2	!  read saved value
	sth	%l2, [%g5 + %l0]	!  restore register
	inc	2, %l1
	ba	fetch
	sth	%l1, [%g5 + 6*2]		!  update stack pointer

	!  RTI - Return from Interrupt    Stack -> PC and Flags
do_rti:
	ldsh	[%g5 + 6*2], %l1		!  get top of stack
	ldsh	[%g2 + %l1], %g3	!  read saved PC
	sth	%g3, [%g5 + 7*2]		!  restore PC
	inc	2, %l1			!  adjust stack
	ldsh	[%g2 + %l1], %l5	!  read saved flags
	inc	2, %l1			!  adjust stack
	ba	fetch
	sth	%l1, [%g5 + 6*2]		!  update stack pointer

	!	A commonly used routine to finish most of the
	!	one-operand and two-operand instructions
	!	It assumes %o0 still has the destination address
	!	Note:  this is Not a subroutine or a leaf.
finish_it:
	tst	%l3			!  test the result value
	call	set_cc			!  set condition codes
	clr	%l5			!	(after clearing them)
	tst	%g4			!   test byte flag
	bl,a	fetch			!
	stb	%l3, [%o0] 		!	store a byte 
	ba	fetch
	sth	%l3, [%o0]		!	or store a word

	!	A leaf procedure to set the condition codes
	!	(the register will already have been cleared)
	!	The pattern was chosen to match the branch instructions
set_cc: be,a	skipz			!    if Zero
	inc	2, %l5		!	set Z bit (2)
skipz:	bl,a	skipn			!    if Negative
	inc	4, %l5		!	set N bit (4)
skipn:  retl				!    return from leaf
	nop



        !  Read_file
        !  Given the name of the file (from the command line)
        !  and a pointer to a program space, reads the data from
        !  the file, converts it into binary, and stores it in
        !  that space.  This uses Convert_Octal, which follows
        !
        !  Input:       %i0 = name of the file
        !               %i1 = space in memory to store into
        
        !local variables

        line = -8
        .global	read_file
read_file:	save	%sp, -104, %sp

        mov     %i0, %o0                ! set up call to
        clr     %o1                     !    open file
        clr     %o2                     !    (just as in the text)
        mov     5, %g1
        ta      0                       ! and make the system call
        orcc    %o0, %g0, %l1         ! save the file pointer
        ble     done_read
        nop
readloop:
        add     %fp, line, %o1          ! read into local memory
        mov     7, %o2                  !    (7 bytes)
        mov     3, %g1
        ta      0
        tst     %o0                     ! at end of input
        ble     done_read               !    give up
        nop
        add     %fp, line, %o0          ! pass pointer to routine
        call    conv_oct                !    for output
        stb     %g0, [%o0+6]            ! null character at end to be safe
        sth     %o0, [%i1]              !
        inc     2, %i1                  ! and update pointer
        ba      readloop
        mov     %l1, %o0              ! go back for more data
done_read:
        ret
	restore 

        !  Convert_Octal
        !  Converts a numeric Ascii octal string to binary.
        !  Input:       %i0 = pointer to the numeric string
        !  Output:      %i0 = binary value
        !
        !
        !  Since this only uses three registers, no stack,
        !  and calls no functions, it could be written as a leaf.
        !  In that case, it would use the 'o' registers instead of
        !  the 'l' registers and '%i0'
        
        
        
        .global	conv_oct
conv_oct:	save	%sp, -96, %sp


        mov     %i0, %l0
        ba      cread
        clr     %l2
cloop:                            ! do {
        sub     %l1, "0", %l1   !    convert digit to binary
        sll     %l2, 3, %l2       !    val = val * 8
        add     %l2, %l1, %l2 !       + digit
cread:  ldub    [%l0], %l1       !    read next character
        tst     %l1                 !    advance input (in delay)
        bne,a   cloop                   ! while char != null
        inc     %l0                  !    (advance input)
        mov     %l2, %i0             !        return result

        ret
	restore 


	!  Fetch_op
	!  Fetch a single operand operand for a PDP-11 instruction
	!	input	i0 - addressing mode
	!		i1 - register to use
	!	output	i0 - Sparc address of value
	!
	!  Modes 3,5,7 were implemented but removed from this file.

		! addressming mode (input)
		! register used (input)
		! operand address in Sparc (output)
		! all purpose temporary register
		! value found in a PDP register

	.global	fetch_op
fetch_op:	save	%sp, -96, %sp

	sll	%i1, 1, %i1	!  double register for PDP array
	sll	%i0, 2, %i0	!  -1 jump table with mode
	set	Modes, %l0		!  set up jump table
	ld	[%l0+%i0], %l0	!  get case address
	jmp	%l0			!  handle case
	ldsh	[%g5+%i1],%l1	!  read register value (in delay)
Modes:	.word	M0, M1, M2, done_op, M4, done_op, M6, done_op

M0:	tst	%g4			!  Mode 0, operand in register
	bl,a	byte			!  	(uses bottom byte in byte mode
	inc	%i1			!	 but these programs work anyway)
byte:	ba	done_op
	add	%g5, %i1, %i0	!  return result
M1:	ba	done_op			!  Mode 1, address in register
	add	%g2,%l1,%i0 !	fixed for Sparc addres
M2:	add	%g2,%l1,%i0 !  Mode 2, address in register
	inc	%l1			!	increment register
	tst	%g4			!	   1 for byte
	bge,a	inc2			!	   2 for word
	inc	%l1
inc2:	ba	done_op			!       and save register value
	sth	%l1,[%g5+%i1]
M4:	dec	%l1			!  Mode 4, auto-decrement
	tst	%g4			!	decrement register
	bge,a	dec2			!	   1 for byte
	dec	%l1			!	   2 for word
dec2:	sth	%l1,[%g5+%i1]	!	and save it
	ba	done_op
	add	%g2,%l1,%i0	!	fix address for Sparc
M6:	ldsh	[%g5 + 2*7],%g3	!  Modes 6 Displacement
	ldsh	[%g2+%g3],%l0 !  get displacement
	inc	2,%g3			!  increment and
	sth	%g3,[%g5 + 2*7]	!	store program counter
	ldsh	[%g5+%i1], %l1!  Now get the value
	add	%l0,%l1,%l0	!       and add it to X
        add	%g2,%l0,%i0
done_op:	
	ret
	restore 
