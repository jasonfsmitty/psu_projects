Jason F Smith
CSE 418  Fall 2000
Project 2


My predator/prey/food relationship is derived from the game PacMan.
PacMan eats the Power Pellets for food, and the Ghosts kill PacMan
if they can catch him.  Depending on the current food reaction
setting, when PacMan eats a pellet, he will either grow larger, or
he will power up and chase after the ghosts (just like in the
original game).  Ghosts always grow larger when they destroy a PacMan.

Controls ----------------------------------------------------------
	Escape - Quit.
	Left mouse button click - Adds another object to the screen.
		The type of object depends on the most recently
		chosen from the menu, which is accessable by pressing
		the right mouse button.  The default type is PacMan.
	Right mouse button - Brings up the main menu.  The menu and
		it's choices are described below.

	Menu Choices ------------------------------------------
		Add... - Allows selection of the three types of
			object to add to the screen:
			Power Pellets:  Food for PacMan.
			PacMan: The 'prey.'
			Ghosts: The 'predator.'
		Speed - Allows the objects to move faster or slower.
			Select 'Default' to reset the speed.  The 
			speed is not real-time speed.  Hence, if
			there is slowdown on the system, the objects
			will also slowdown.
		
		Food Reaction - This determines what happens to PacMan
			when he eats a Power Pellet.  If 'Grow Bigger'
			is selected, then PacMan will increase in size
			each time he eats a pellet.  If 'Chase Ghosts'
			is selected, the Pellet will give PacMan the
			ability to chase after and kill ghosts.  The
			default is 'Grow Bigger.'
		Clear - Deletes all objects from the screen.
		Exit - Quit program.


Files --------------------------------------------------------------
		proj2.cpp - Contains main() and other windowing
				functions.
		myTransform.h - Contains the redefined transformation
				functions.
		myBug.h - Contains the classes for the different
			  objects, and a linked list to store them.
		p2_defs.h - Contains macros and function definitions
			    for the program.

Transformation Details ---------------------------------------------

The myTransform.h file contains all of the re-defined OpenGL
transformation functions.  In order for them to actually transform
vertecies, the point must be rendered using one of the provided
myVertex*( x, y ) functions.  Also provided is a myTriangle(...) and
myRect(...) function (each taking 3 and 4 vertices respectively).  A
current matrix is maintained, and whenever a myPushMatrix() is called,
the matrix is copied onto a stack.  A myPopMatrix() pulls the top
matrix off of the stack and sets the current matrix to it.
myLoadIdentity() clears the stack, and resets the current matrix to
the identity matrix.  The myRotate(theta), myTranslate(dx, dy), and myScale( sx, sy ) do just as they are expected to.  


Other Details -------------------------------------------------------

All points for PacMan, the Ghosts, and the Power Pellets are computed
at run-time when the class is initialized.  Also, they are recomputed
when their size changes.  By varying the macros in 'p2_defs.h',
the resolution and size of all objects can be changed, and will go in
effect at the next compile.  All objects are derived from a BasicBug
class, and the objects are stored in a BugList class (basically a linked list).  When an object is destroyed (either a ghost or PacMan),
the object is copied into a special class (called either Extra or
DyingGhost) and is faded into nothing to disappear into the background.
Also, rotating sparks surrounding the power pellets can be turned on or
off by commented/uncommenting a macro at the begining of the myBug.h
file.  Also, the bounding box algorithm was altered to allow more 
realistic deflections.  For instance, if two objects are going in the
same direction, but the back one is moving faster and collides with
the front one, only the back one changes direction.  Whenever possible,
inlining was used to increase the efficency of the program.  Given a few more days, the code could have been made even more efficient, and
cleaned up some.