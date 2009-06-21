/*

	Jason F. Smith
	4588
	
	CSE 331 HW #2
	
	
	
	This program computes the greatest common factor of two
	numbers, x and y.
*/

	define( `x_val', 60 )
	define( `y_val', 39 )
	
	define( `x_r', l0 )
	define( `y_r', l1 )
	
	.global main
main:
	save 	%sp, -64, %sp
	mov	y_val, %y_r	! set the y value
	mov 	x_val, %x_r	! set the x value
do_loop:
	mov	%x_r, %o0	! move x in for the call
	call	.rem		! same as modulus operation
	mov	%y_r, %o1	!   remainder is in %o0 when done
	
	tst 	%o0		! test the remainder to see if zero
	mov	%y_r, %x_r	! else, x = y
	bne	do_loop		! go back and try again
	mov	%o0, %y_r	!       y = rem
	
end_prog:	! the answer is in x_r
	mov	1, %g1		! prepare for...
	ta	0		! ...system trap to end program