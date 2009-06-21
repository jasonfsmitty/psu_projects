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

        define(`prog_r', g2)            ! global pointer to input
        define(`pc_r', g3)              ! global instruction pointer
                                        !    (as an offset off of prog_r)
        define(`inst_r', g4)            ! global instruction
        define(`PDP_r',g5)              ! pointer to Register Array

define(ext_dig, `ifelse($2,0,`and	$1, 7, $3',`srl	$1, 3*$2, %temp_r 
	and	%temp_r, 7, $3')')

	define(`flag_r', l5)


	define(`temp_r', l0)	! general all-purpose temporary value
	define(`radr_r', l1)	! register address (offset into array)
	define(`sval_r', l2)	! source operand value
	define(`dest_r', l3)	! destination operand value
	define(`op_r', l4)	! instruction decoding register

	begin_main
start:
        set     prog, %o1               ! read file into program space
        call    read_file
        ld      [%i1+4], %o0            !    file name = argv[1]

        set     prog, %prog_r           ! point at top of program code
        set     PDPReg, %PDP_r          ! set up pointer to Registers
        st      %g0, [%PDP_r + 6*2]     ! clear stack pointer and program


fetch:  ldsh	[%PDP_r + 7*2], %pc_r	! read program counter
	ldsh	[%prog_r+%pc_r],%inst_r	! read the instruction
	inc	2, %pc_r
	sth	%pc_r, [%PDP_r+7*2]	! increment program counter

	tst	%inst_r			! halt means all done
	be	done
	nop

	ext_dig( %inst_r, 4, %op_r)	! examine #4 digit
	cmp	%op_r, 7		! if equal to 7
	be	do_sevens		!    -- mul, div, ash
	cmp	%op_r, 6		! if equal to 6
	be	do_addsub		!    == add or subtract
	tst	%op_r			! else if not equal to zero
	bne	do_twos			!    -- mov, cmp, ...

	ext_dig( %inst_r, 3, %op_r)	! examine #3 digit
	cmp	%op_r, 5		! if equal to 5
	be	do_fives		!    -- clr, com, inc, dec, tst...
	cmp	%op_r, 4		! if equal to 4
	be	do_jsr			!    -- jsr
	srl	%inst_r, 8, %op_r	! examine top byte of register
	tst	%op_r			! if != 0
	bne	do_branch		!    -- branch instruction
	and	%inst_r,0377,%dest_r	!    (fetch displacement on the way)
	ext_dig( %inst_r, 2, %op_r )	! examine #2 digit
	cmp	%op_r, 1		! and see what it is
	be	do_jmp			!    1 is JMP
	ext_dig( %inst_r, 0, %temp_r)	!    (examine register on the way)
	blt	do_rti			! assume 0 is rti 
	nop
	ba	do_rts			! assume RTS  (actually 3 is SWAB,
	nop				!    and 240+ set/clear flags directly)
done:
	end_main

	!  These are the special two-operand instructions that specify
	!  a register as one operand.  They are of the form   07_RSS
	!  0 = MUL	1 = DIV		2 = ASH
do_sevens:
	ext_dig( %inst_r, 1, %o0)	!  examine the source operand
	call	fetch_op		!	(  mode  )
	ext_dig( %inst_r, 0, %o1)	!   	(register)
	ldsh	[%o0], %sval_r		!  load source value
	ext_dig( %inst_r, 2, %radr_r)	!  examine register number
	sll	%radr_r, 1, %radr_r	!  read destination value
	ldsh	[%PDP_r + %radr_r], %dest_r	
	ext_dig( %inst_r, 3, %op_r)	!  identify instruction
	sll	%op_r, 2, %op_r		!  use instruction to
	set	sevens, %temp_r		!	index into case table
	ld	[%temp_r + %op_r], %temp_r
	jmpl	%temp_r, %g0		!  handle cases
	nop
sevens:	.word	MUL, DIV, ASH
MUL:	mov	%sval_r, %o0		!  multiply source
	call	.mul			!	times destination
	mov	%dest_r, %o1
	tst	%o0			!  examine product
	call	set_cc			!     and set condition codes
	clr	%flag_r			!     (after clearing them)
	btst	2, %radr_r		!  if odd destination
	bne,a	fetch
	sth	%o0, [%PDP_r + %radr_r] ! update one register
	ba	fetch			!    else store two
	st	%o0, [%PDP_r + %radr_r] !    (at the same time)
DIV:	mov	%dest_r, %o0
	call	.div			! [dst] = [dst] / [src]
	mov	%sval_r, %o1
	tst	%o0			! examine quotient
	call	set_cc			!     and set condition codes
	clr	%flag_r			!     (after clearing them)
	sth	%o0, [%PDP_r + %radr_r]	! stored into dest
	inc	2, %radr_r		! advance to next register address
	mov	%dest_r, %o0
	call	.rem			! [dst+1] = [dst] % [src]
	mov	%sval_r, %o1
	ba	fetch
	sth	%o0, [%PDP_r + %radr_r]
ASH:	sll	%sval_r, 32-6, %sval_r	! last 6 bits of operand
	sra	%sval_r, 32-6, %sval_r	!    determine the shift
	tst	%sval_r			! positive, shift left
	bge,a	stash
	sll	%dest_r, %sval_r, %dest_r
	neg	%sval_r			! negative, shift right
	srl	%dest_r, %sval_r, %dest_r
stash:	tst	%dest_r			! examine the result
	call	set_cc			!    to set the condition codes
	clr	%flag_r
	ba	fetch
	sth	%dest_r,[%PDP_r+%radr_r]! and store the result

	!   Add and Subtract both use word operands only
	!   so they are coded separately to keep them simple
do_addsub:
	mov	%inst_r, %temp_r	! save instruction
	and     %temp_r, 07777, %inst_r	! to nuke the sign bit
	ext_dig( %inst_r, 3, %o0)
	ext_dig( %inst_r, 2, %o1)	! examine source operand
	call	fetch_op
	nop
	ldsh	[%o0], %sval_r		! load source value
	ext_dig( %inst_r, 1, %o0)
	call	fetch_op		! examine destination operand
	ext_dig( %inst_r, 0, %o1)
	ldsh	[%o0], %dest_r		! load destination value
	tst	%temp_r			! check original instruction
	bge,a	finish_it		! Either add
	add	%dest_r, %sval_r, %dest_r
	ba	finish_it
	sub	%dest_r, %sval_r, %dest_r
	!   These are the remaining two-operand instructions
	!   Both operands are fetched, the operation is chosen,
	!   and a single catch-all section finishes them off.
do_twos:
	ext_dig( %inst_r, 3, %o0)	
	ext_dig( %inst_r, 2, %o1)	! examine source operand
	call	fetch_op
	nop
	tst	%inst_r			! if byte mode
	bl,a	bytesrc	
	ldsb	[%o0], %sval_r		!    load byte operand
	ldsh	[%o0], %sval_r		!    else word operand
bytesrc:
	ext_dig( %inst_r, 1, %o0)
	call	fetch_op		! examine destination operand
	ext_dig( %inst_r, 0, %o1)	!
	tst	%inst_r			! if byte mode
	bl,a	bytedst
	ldsb	[%o0], %dest_r		!    load byte operand
	ldsh	[%o0], %dest_r		!    else word operand
bytedst:
	sll	%op_r, 2, %op_r		! use instruction
	set	twos, %temp_r		!     to index into case table
	ld	[%temp_r + %op_r], %temp_r
	jmpl	%temp_r, %g0		! handle cases
	nop
twos:	.word	fetch, MOV, CMP, BIT, BIC, BIS
MOV:	ba	finish_it
	mov	%sval_r, %dest_r
CMP:	ba	finish_it+4		! don't test dval
	cmp	%sval_r, %dest_r
BIT:	ba	finish_it+4		! don't test dval
	btst	%sval_r, %dest_r
BIC:	ba	finish_it
	bclr	%sval_r, %dest_r
BIS:	ba	finish_it
	bset	%sval_r, %dest_r

	!  These are some of the common one-operand instructions.
	!  The operand is fetched, the operation is formed, and
	!  a shared section of code finishes it off.
do_fives:
	ext_dig( %inst_r, 1, %o0)
	call	fetch_op		!  examine destination operand
	ext_dig( %inst_r, 0, %o1)	! 
	tst	%inst_r			!  if byte mode
	bl,a	byteone
	ldsb	[%o0], %dest_r		!     load byte operand
	ldsh	[%o0], %dest_r		!     else word operand
byteone:
	ext_dig( %inst_r, 2, %op_r)	!  examine the opcode
	sll	%op_r, 2, %op_r
	set	fives, %temp_r		!  index into case table
	ld	[%temp_r + %op_r], %temp_r	
	jmpl	%temp_r, %g0		!  handle cases
	nop
fives:	.word	CLR, COM, INC, DEC, NEG, fetch, fetch, finish_it
CLR:	ba	finish_it
	clr	%dest_r
COM:	ba	finish_it
	not	%dest_r
INC:	ba	finish_it
	inc	%dest_r
DEC:	ba	finish_it
	dec	%dest_r
NEG:	ba	finish_it
	neg	%dest_r

	!   Branches -- displacement in dest_r, branch code in op_r
	!   The flags are compared with the branch instruction
	!   directly -- Z looking at '2' bit, and N at '4'
do_branch:
	sll	%dest_r, 24, %dest_r	!  double and sign-extend
	sra	%dest_r, 23, %dest_r	!	displacement
	add	%pc_r, %dest_r, %dest_r	!  determine destination 
	btst	%op_r, %flag_r		!  check PDP flags
	sll	%op_r, 2, %op_r		!  use opcode to
	set	branches, %temp_r	!	index into case table
	ld	[%temp_r + %op_r], %temp_r
	jmpl	%temp_r, %g0		!  decide how to check
	nop				!       the PDP flags
branches: .word fetch, BE, BE, BNE, BE, BNE, BE, BNE
BNE:	bne,a	fetch			!  Z set --> BEQ
	sth	%dest_r, [%PDP_r + 7*2]	!  N set --> BLT
	ba	fetch			!  (N or Z) set -> BLE
	nop
BE:	be,a	fetch			!  no check --> BR
	sth	%dest_r, [%PDP_r + 7*2]	!  Z zero   --> BNE
	ba	fetch			!  N zero   --> BGE
	nop				!  (N or Z) zero  --> BGT

	!   JMP --  R7 <-- jump address
do_jmp:
	ext_dig( %inst_r, 1, %o0)
	call	fetch_op		! examine branch address
	ext_dig( %inst_r, 0, %o1)
					! branch address is in %o0
	sub	%o0, %prog_r, %dest_r	! Unfix Sparc address
	ba	fetch
	sth	%dest_r, [%PDP_r + 7*2]	! update PC with branch address

	!   JSR --  Stack <-- Rn <-- R7 <-- subroutine address
do_jsr:
	ext_dig( %inst_r, 1, %o0)
	call	fetch_op		! examine branch address
	ext_dig( %inst_r, 0, %o1)
					! branch address is in %o0
	sub	%o0, %prog_r, %dest_r	! Unfix Sparc address
	ldsh	[%PDP_r + 6*2], %radr_r ! prepare to push Rn onto stack
	dec	2, %radr_r		!
	sth	%radr_r, [%PDP_r + 6*2] ! update stack pointer
	ext_dig( %inst_r, 2, %temp_r)	! identify register op
	sll	%temp_r, 1, %temp_r
	ldsh	[%PDP_r + %temp_r], %sval_r	! load value and
	sth	%sval_r, [%prog_r + %radr_r]	! save on stack
	ldsh	[%PDP_r + 7*2], %pc_r		! examine latest PC 
	sth	%pc_r, [%PDP_r + %temp_r]	! save PC in register
	ba	fetch
	sth	%dest_r, [%PDP_r + 7*2]	! update PC with branch address

	!  RTS - Return from Subroutine   Stack -> R7 -> Rn
do_rts: sll	%temp_r, 1, %temp_r
	ldsh	[%PDP_r+%temp_r], %pc_r		!  restore old PC
	sth	%pc_r, [%PDP_r + 7*2]		!     and save it
	ldsh	[%PDP_r + 6*2], %radr_r		!  get top of stack
	ldsh	[%prog_r + %radr_r], %sval_r	!  read saved value
	sth	%sval_r, [%PDP_r + %temp_r]	!  restore register
	inc	2, %radr_r
	ba	fetch
	sth	%radr_r, [%PDP_r + 6*2]		!  update stack pointer

	!  RTI - Return from Interrupt    Stack -> PC and Flags
do_rti:
	ldsh	[%PDP_r + 6*2], %radr_r		!  get top of stack
	ldsh	[%prog_r + %radr_r], %pc_r	!  read saved PC
	sth	%pc_r, [%PDP_r + 7*2]		!  restore PC
	inc	2, %radr_r			!  adjust stack
	ldsh	[%prog_r + %radr_r], %flag_r	!  read saved flags
	inc	2, %radr_r			!  adjust stack
	ba	fetch
	sth	%radr_r, [%PDP_r + 6*2]		!  update stack pointer

	!	A commonly used routine to finish most of the
	!	one-operand and two-operand instructions
	!	It assumes %o0 still has the destination address
	!	Note:  this is Not a subroutine or a leaf.
finish_it:
	tst	%dest_r			!  test the result value
	call	set_cc			!  set condition codes
	clr	%flag_r			!	(after clearing them)
	tst	%inst_r			!   test byte flag
	bl,a	fetch			!
	stb	%dest_r, [%o0] 		!	store a byte 
	ba	fetch
	sth	%dest_r, [%o0]		!	or store a word

	!	A leaf procedure to set the condition codes
	!	(the register will already have been cleared)
	!	The pattern was chosen to match the branch instructions
set_cc: be,a	skipz			!    if Zero
	inc	2, %flag_r		!	set Z bit (2)
skipz:	bl,a	skipn			!    if Negative
	inc	4, %flag_r		!	set N bit (4)
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
        define(`fd_r', l1)
        local_var
        var(line, 1, 8)
        begin_fn(read_file)
        mov     %i0, %o0                ! set up call to
        clr     %o1                     !    open file
        clr     %o2                     !    (just as in the text)
        mov     5, %g1
        ta      0                       ! and make the system call
        orcc    %o0, %g0, %fd_r         ! save the file pointer
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
        mov     %fd_r, %o0              ! go back for more data
done_read:
        end_fn(read_file)

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
        define(`ptr_r', l0)
        define(`char_r', l1)
        define(`val_r', l2)
        begin_fn(conv_oct)

        mov     %i0, %ptr_r
        ba      cread
        clr     %val_r
cloop:                            ! do {
        sub     %char_r, "0", %char_r   !    convert digit to binary
        sll     %val_r, 3, %val_r       !    val = val * 8
        add     %val_r, %char_r, %val_r !       + digit
cread:  ldub    [%ptr_r], %char_r       !    read next character
        tst     %char_r                 !    advance input (in delay)
        bne,a   cloop                   ! while char != null
        inc     %ptr_r                  !    (advance input)
        mov     %val_r, %i0             !        return result

        end_fn(conv_oct)


	!  Fetch_op
	!  Fetch a single operand operand for a PDP-11 instruction
	!	input	i0 - addressing mode
	!		i1 - register to use
	!	output	i0 - Sparc address of value
	!
	!  Modes 3,5,7 were implemented but removed from this file.

	define(`mode_r', i0)	! addressming mode (input)
	define(`reg_r', i1)	! register used (input)
	define(`addr_r', i0)	! operand address in Sparc (output)
	define(`temp_r', l0)	! all purpose temporary register
	define(`rval_r', l1)	! value found in a PDP register

	begin_fn(fetch_op)
	sll	%reg_r, 1, %reg_r	!  double register for PDP array
	sll	%mode_r, 2, %mode_r	!  index jump table with mode
	set	Modes, %temp_r		!  set up jump table
	ld	[%temp_r+%mode_r], %temp_r	!  get case address
	jmp	%temp_r			!  handle case
	ldsh	[%PDP_r+%reg_r],%rval_r	!  read register value (in delay)
Modes:	.word	M0, M1, M2, done_op, M4, done_op, M6, done_op

M0:	tst	%inst_r			!  Mode 0, operand in register
	bl,a	byte			!  	(uses bottom byte in byte mode
	inc	%reg_r			!	 but these programs work anyway)
byte:	ba	done_op
	add	%PDP_r, %reg_r, %addr_r	!  return result
M1:	ba	done_op			!  Mode 1, address in register
	add	%prog_r,%rval_r,%addr_r !	fixed for Sparc addres
M2:	add	%prog_r,%rval_r,%addr_r !  Mode 2, address in register
	inc	%rval_r			!	increment register
	tst	%inst_r			!	   1 for byte
	bge,a	inc2			!	   2 for word
	inc	%rval_r
inc2:	ba	done_op			!       and save register value
	sth	%rval_r,[%PDP_r+%reg_r]
M4:	dec	%rval_r			!  Mode 4, auto-decrement
	tst	%inst_r			!	decrement register
	bge,a	dec2			!	   1 for byte
	dec	%rval_r			!	   2 for word
dec2:	sth	%rval_r,[%PDP_r+%reg_r]	!	and save it
	ba	done_op
	add	%prog_r,%rval_r,%addr_r	!	fix address for Sparc
M6:	ldsh	[%PDP_r + 2*7],%pc_r	!  Modes 6 Displacement
	ldsh	[%prog_r+%pc_r],%temp_r !  get displacement
	inc	2,%pc_r			!  increment and
	sth	%pc_r,[%PDP_r + 2*7]	!	store program counter
	ldsh	[%PDP_r+%reg_r], %rval_r!  Now get the value
	add	%temp_r,%rval_r,%temp_r	!       and add it to X
        add	%prog_r,%temp_r,%addr_r
done_op:	
	end_fn(fetch_op)
