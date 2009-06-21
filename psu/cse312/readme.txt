README file for the Calculator/Clock/Tetris program.
====================================================

Written by Jason F Smith   Spring 2000

	Included is the program 'sec1' which is called by BSVC to generate the 1 second interrupts.  Also included is the source code for it incase you want to change anything.  Use the setup file hw9.setup for BSVC so everything runs smoothly.

	The program uses character sequences starting with ESC to move the cursor around the terminal window.  The directions for Tetris are shown on the screen, and everything else is pretty self explanitory.  I incorporated as much error checking in the calculator as possible, such as overflows, divide by zero, invalid expressions, etc.  The calculator gives the user a choice whether to enter the expressions in infix or postfix notation, it doesn't make much of a difference, just saves the program the trouble of translating the input to postfix.  Much of the error checking is done within the translator, so for the most reliable setup, choose to enter expressions in infix notation.  The clock will run no matter what else you run at the same time, (the alarms and timers are still updated, and the messages displayed even while playing Tetris or running the calculator).