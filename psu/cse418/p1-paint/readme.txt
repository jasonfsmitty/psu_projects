Jason F. Smith
CSE 418 Project 1
Fall 2000

+--- INSTRUCTIONS ---+
	The program can be considered a state machine, much like
	OpenGL itself.  The current state can be edited though a
	series of menus (accessable by a right click in the window).

	NOTE: The right mouse button is only used to access the menu.
	If this file references a mouse button without specifying 
	either right or left, then it is referring to the left mouse
	button.

+-- MENU CHOICES --+
Draw-
	Sets the current type of object to draw.  Choices are:
	rectangles, squares, ellipses, super ellipses, circles,
	lines, and a special pen drawing mode.

Draw Color-
	Sets the current drawing color that will be applied to
	future objects.  There are the basic choices shown, and
	also a feature to allow customization of the color by
	entering the red, green, and blue components at the 
	command line.

Fill-
	Determines if the object is drawn as just an outline, or if
	it is filled with the drawing color.  Use this sub-menu
	to turn the fill on or off.

Width-
	Sets the width of the points/lines used when drawing objects.
	It is meaningless for objects that are filled (except for the
	pen and lines).

Background-
	Determines the color of the background in the picture.  May
	be changed at any point.  Just as in Draw Color, provides the
	option to manually enter color components on the command line.

Move Mode-
	Allows the cursor to select and move drawn objects.  Once
	in move mode, use the left mouse button to select an object
	(just click down on top of the object) and then drag the
	object to the desired location.  Let up the mouse button to
	release the object.

Delete Mode-
	Cause program to enter into delete mode.  To delete an object
	in delete mode, use the left mouse button to click on an the
	object to delete.  The click down is not important, but the
	object underneath the mouse when the button is released will
	be deleted.

Clear Dislay-
	Simply erases the entire display.  Does not change the 
	current state or any of the current options.

State Information-
	Use this sub-menu to turn on/off the state information
	printouts at the command line.  When turned on, every time
	the current state is updated or changed, a printout of the 
	current state and it's options are displayed at the command
	line.  May also be turned on or off by the 'i' and 'n' keys
	respectively.

Exit-
	Closes the program.  Can also press escape to quit.


+-- DRAWING --+
	Once in drawing mode (the default mode), press the left
	mouse button down to start the object.  While depressed,
	the mouse may be drug around the screen to size and shape
	the object.  Once released, the object can not be edited
	other than through the Move or Delete modes.  The following
	is a list of the objects and what the down click and up
	click points represent.

	Rectangle - The two opposite corners are determined by
		a down click and up click.  These two points
		defined the rectangle.
	Square - Same as in a rectangle, only the size of the
		square is limited to the minimum of the the
		edges (either the difference between the y's
		or between the x's).
	Ellipse - The down click becomes the center of the ellipse,
		and the up-click determines Rx and Ry.
	Circle - Down click becomes the center, and the radius is
		the minimum of the distances to the up-click (x,y).
	Super Ellipse - Down click is the center, Rx and Ry are
		determined by the up-click coordinates.
	Line - The down-click is the beginning of the line, and the
		up-click is the end.
	Pen - The down click begins the pen line, which follows the
		mouse around the screen.  The 'pen' is essence is the
		path that the cursor takes over the window until the
		mouse button is released.

+--- IMPLEMENTATION ---+
	All objects are created using classes.  All of the object
	classes are derived from a simple Shape class.  This allows
	the single linked list class, ObjectList, to store all of the
	different types of objects while only using Shape pointers.

	The derived class scheme also allows for the Shape::Draw()
	function to be overwritten by derived classes.  Each derived
	class overloads this function so that the correct drawing
	is drawn for the type of object.  A companion function, 
	DrawValue( byte red, byte green, byte blue ), is used when
	implementing the Way2 algorithm for object selection as
	discussed in class.  The function is exactly the same as
	the Draw function for each respective class with one small
	difference.  The parameters passed into the function become
	the color value that is used to draw the object.  This was
	used to bypass any difficulties that might arise implementing
	the Way2 method on systems which might round the floating
	point values that are normally used.

	The class setup allows for easy addition of extra features,
	such as the pen class.  All that is needed is for a new class
	to be written, an addition to the menu choices, and a new case
	statement inside of the mouseclick function.

Pen Object-
	Originally, this was implemented by adding points to the list
	on every callback of the motion function.  However, this was
	inefficient and also produced horrible results.  The final
	version uses a linked list of coordinates inside of the class.
	At each motion callback, another point is added to the list,
	and the points are drawn using glBegin( GL_LINE_STRIP ) to 
	connect them.  Because the default line conversion algorithm
	of OpenGL does not produce high quality results at the
	endpoints, the pen width is limited to a maximum of 4.0 pixels
	wide.  This is done internally to the class as not to effect
	the current width state of the program.  The quality of the
	line is adversely effected by several factors, most importantly
	the refresh time.  The motion callback seems to wait until the 
	refresh is finished, however the mouse continues to move.  As
	conditions decrease (either due to the number of objects
	already drawn or due to system performance outside of the
	program) the time between motion callbacks increases, thus 
	decreasing the smoothness of the pen drawing.