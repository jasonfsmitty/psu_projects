/*

	Jason F. Smith
	4588
	
	CSE 331 HW #2
	
	
	
	This program computes the greatest common factor of two
	numbers, x and y.
*/

	
	
	
	
	
	
	.global main
main:
	save 	%sp, -64, %sp
	mov	39 , %l1 	! set the y value
	mov 	60 , %l0 	! set the x value
do_loop:
	mov	%l0 , %o0	! move x in for the call
	call	.rem		! same as modulus operation
	mov	%l1 , %o1	!   remainder is in %o0 when done
	
	tst 	%o0		! test the remainder to see if zero
!	be	end_prog	! if remainder==0, then goto end
!	nop
	mov	%l1 , %l0 	! else, x = y
	bne	do_loop		! go back and try again
	mov	%o0, %l1 	!       y = rem
	
end_prog:	! the answer is in %o0!!
	mov	1, %g1		! prepare for...
	ta	0		! ...system trap to end program