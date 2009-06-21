/*
	Jason F Smith
	CSE 418
*/


#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

/* Preprocesser defines used for various debugging and such */

#define		SHOW_TEXT_STEPS
	// At certain steps in the execution of the program,
	//   various information is printed out

#define		SHOW_OBJECTS_AT_EXIT
	// When the program exits, information on all of the
	//   objects in the list is displayed.

/*------- Defines used in the program. -----------*/

// Program states
#define  ADD_OBJ			1
#define  EDIT_OBJ			2
#define  DELETE_OBJ			3

// Objects in the program
#define  OBJ_BACKGROUND		(-1)
#define  OBJ_RECTANGLE		1
#define	 OBJ_SQUARE			0x02  
#define  OBJ_CIRCLE			3  
#define  OBJ_ELLIPSE		4
#define  OBJ_SUPER_E		5  

// Menu choices
#define  MENU_RECTANGLE		OBJ_RECTANGLE
#define  MENU_SQUARE		0BJ_SQUARE
#define  MENU_CIRCLE		OBJ_CIRCLE
#define  MENU_ELLIPSE		OBJ_ELLIPSE
#define  MENU_SUPER_E		OBJ_SUPER_E

#define  MENU_FILL			7
#define  MENU_EDIT			8
#define  MENU_DELETE		9
#define  MENU_BACKGROUND	10
#define  MENU_CLEAR			11

#define	 MENU_SWAP			98

#define	 MENU_EXIT			99

#define  MENU_YES			1
#define  MENU_NO			2

// Color choices in the menu
#define  COLOR_BLACK		10
#define  COLOR_WHITE		11
#define  COLOR_RED			12
#define  COLOR_GREEN		13
#define  COLOR_BLUE			14
#define  COLOR_YELLOW		15
#define  COLOR_PURPLE		16
#define  COLOR_ORANGE		17
#define  COLOR_CUSTOM		18

// Define Max and Min
#define  Max(A,B)			( (A)>=(B) ? (A) : (B) )
#define  Min(A,B)			( (A)<=(B) ? (A) : (B) )
#define  Abs(A)				( (A)>=0 ? (A) : (-A) )

// The default point size
#define	 POINT_SIZE			8.0
#define  LINE_SIZE			POINT_SIZE

//Window stuff
#define  DEFAULT_WIDTH		300
#define  DEFAULT_HEIGHT		300
#define  INIT_X				200
#define  INIT_Y				200
#define  WINDOW_TITLE		"Jason F. Smith - CSE 418 Project 1"

// Initial background colors
#define  DEFAULT_R			0.5
#define  DEFAULT_G			0.5
#define  DEFAULT_B			0.5
#define  DEFAULT_ALPHA		0.0

/*----------------------------------------------------------*/
// A structure to hold the three color components.
struct MyColor{
	GLfloat r;
	GLfloat g;
	GLfloat b;
};
/*----------------------------------------------------------*/
// A structure to store all of the drawn objects.
struct MyObject{
	int type;			// What type of object is this.
	
	int x0, y0;			// Initial coordinate.
	int x1, y1;			// Second coordinate.

	GLfloat r, b, g;	// The color of the object.
	bool   isFilled;	// Is the object filled.

	MyObject* next;		// Used to link them together.
};
/*----------------------------------------------------------*/

MyObject* 	head;		// Head of the linked list.
MyObject* 	current;	// The current object being 
						//   drawn or edited.
MyObject	options;	// Contains the current options set
						//   for the drawing mode.

int 		mainWin;	// ID of the main window.
int 		colorWin; 	// ID of the color setting window.
int		statusWin;      // ID of the status window.

int		dx=0; 
int		dy=0;		// The coordinates that the mouse button 
					// is pressed down at.  Used to move objects.

int 		p_state;	// The current state of the program.

/*----------------------------------------------------------*/
// Functions to deal with the MyObject data type.

bool addMyObj( MyObject* obj );
	// Adds the object pointed to by obj to the list.

bool removeCurrent( void );
	// Removes the current entry from the list.

void deleteAll( void );
	// Deletes everything but the background object.

MyObject* findFirst( int x, int y );
	// Searches through the list from the 'top' down.
	// Returns a pointer to the first object found which
	//   occupies the pixel at (x,y).  Returns NULL if
	//   there are no objects at the location.

bool isInside( MyObject* obj, int x, int y);
	// A function to assist the FindFirst function.
	// It checks the coordinates depending on the object
	//   type.  This way, ellipses won't be choosen just
	//   because it's bounding box encompasses the point.

void drawAllObj( void );
	// Draws all of the objects in the list.  This does
	//   draw the current object, but in normal view mode only.

void drawObject( MyObject* t );
	// Draws a single object in the main window.

void drawCurrent( void );
	// Draws current object in edit mode.  Should be called
	//   after DrawAllObj has completed (if desired).

void drawRectangle( MyObject* t );
void drawEllipse( MyObject* t );
void drawSuperE( MyObject* t );
void drawCircle( MyObject* t );
void drawSquare( MyObject* t );


/*---------------------------------------------------------------*/
/*---- Function declarations  ------- */
void init( int argc, char **argv );
	// Initialize all data, glut, and the windows.

void displayMain( void );
	// The default display function for the main window.

void mouseMain( int button, int state, int x, int y );
	// The mouse function for the main window.

void reshapeMain( int w, int h );
	// Reshape  function for the main window.

void drawPoint( int x, int y, GLfloat size = POINT_SIZE );
	// Draws a point at (x,y) of size size.

void drawLine( int x0, int y0, int x1, int y1, GLfloat size = LINE_SIZE );
	// Draw the line from (x0,y0) to (x1,y1) with size size.

void motionMain( int x, int y );
	// Handle the mouse movements in the main window.

void handleMenu( int value );
	// Handle the main menu
	
void drawMenu( int value );
	// Changes the current object draw type.

void colorMenu( int value );
	// Changes the current color settings.

void fillMenu( int value );
	// Turns filling on or off.

void backMenu( int value );
	// The background color menu.

void cleanUp( void );
	// Takes care of de-allocation the memory and 
	//   displaying any additional information before 
	//   the window closes.

/*------------------------------------------------------------*/
bool addMyObj( MyObject* obj )
{
	// Add the object into the back of the list
	//   and make it the current object.
#if defined(SHOW_TEXT_STEPS)
	printf( "Adding an object to the list.\n" );
#endif
	
	MyObject* t;
	t = head;
	obj->next = NULL;

	if( t==NULL ){  // Add the item into the front
		head = obj;
	} else {
		while( t->next != NULL )
			t = t->next;
		t->next = obj;
	}
	current = obj;

	return 1;
}
/*------------------------------------------------------------*/
bool removeCurrent( void )
{
	MyObject* t;
	
	if( current == head ){
		current = head->next;
		delete head;
		head = current;
	} else {
		t = head;
		while( t->next != current )
			t = t->next;
		t->next = current->next;
		delete current;
	}

	current = NULL;

	return 1;
}
/*------------------------------------------------------------*/
MyObject* findFirst( int x, int y )
{
	MyObject* t;
	MyObject* f;
	t = head;
	f = NULL;

	while( t != NULL ){
		if( isInside(t, x, y ) )
			f=t;
		t = t->next;	
	}
	return f;
}
/*------------------------------------------------------------*/
bool isInside( MyObject* obj, int x, int y )
{

	switch( obj->type ){
	// Every object uses the base case of a rectangle for now.
		case OBJ_CIRCLE:
		case OBJ_ELLIPSE:
		case OBJ_SUPER_E:

		case OBJ_SQUARE:
		case OBJ_RECTANGLE:
		default:
			if( ( x >= Min( obj->x0, obj->x1 ) )&&
			    ( x <= Max( obj->x1, obj->x1 ) )&&
			    ( y >= Min( obj->y0, obj->y1 ) )&&
			    ( y <= Max( obj->y0, obj->y1 ) )   )
				return 1;
			else
				return 0;
	};

	return 0;
}
/*------------------------------------------------------------*/
void drawAllObj( void )
{
	MyObject* t;
	t=head;
	
	while( t != NULL ){
		switch( t->type ){
			case OBJ_CIRCLE:
			case OBJ_ELLIPSE:
			case OBJ_SUPER_E:
			case OBJ_RECTANGLE:
				drawRectangle(t);
				break;
			case OBJ_SQUARE:
				drawSquare(t);
				break;
			default:
				drawRectangle(t);			
				break;
		};

		t = t->next;
	}
}
/*------------------------------------------------------------*/
void drawObject( MyObject* t )
{
	// not used right now, took the code and entered it 
	// into the drawAllObjects() function
	printf( "Error- drawObject() was called.\n" );
}
/*------------------------------------------------------------*/
void drawCurrent( void )
{
#if defined(SHOW_TEXT_STEPS)
	printf( "Drawing the current item.\n");
#endif
	// Draw a point so that the user may edit the object.
	drawObject( current );

	drawPoint( current->x0, current->y0 );
	drawPoint( current->x1, current->y1 );
}
/*------------------------------------------------------------*/
void drawRectangle( MyObject* t )
{
#if defined(SHOW_TEXT_STEPS)
	printf( "Preparing to draw the rectangle.\n" );
#endif
	// Set the drawing color
	glColor3f( t->r, t->g, t->b );

	glPointSize( LINE_SIZE );
	glBegin( GL_LINES );

	glVertex2i( t->x0, t->y0 );
	glVertex2i( t->x1, t->y0 );

	glVertex2i( t->x1, t->y0 );
	glVertex2i( t->x1, t->y1 );

	glVertex2i( t->x1, t->y1 );
	glVertex2i( t->x0, t->y1 );

	glVertex2i( t->x0, t->y1 );
	glVertex2i( t->x0, t->y0 );

	glEnd();
}

/*------------------------------------------------------------*/
void drawCircle( MyObject *t )
{
	// Not implemented as of yet.
	printf( "**** Error - attempted to call drawCircle() ****" );
}
/*------------------------------------------------------------*/
void drawSquare( MyObject *t )
{
	// a special case of the rectangle.
	int delx, dely;
	int edge;
	// when changing the coordinates, leave (x0, y0) the same.

	delx = (t->x1 - t->x0);
	dely = (t->y1 - t->y0);

	edge = Min( Abs(delx), Abs(dely) );
	t->x1 = t->x0 + edge;
	t->y1 = t->y0 + edge;

	drawRectangle(t);
}
/*------------------------------------------------------------*/
void drawEllipse( MyObject* t )
{
	printf( "**** Error - attempted to call drawEllipse. ****\n" );
	// To be written later.
}
/*------------------------------------------------------------*/
void drawSuperE( MyObject* t )
{
	printf( "**** Error - attempted to call drawSuperE. ****\n" );
	// To be written later.
}
/*------------------------------------------------------------*/
/****** MAIN WINDOW FUNCTIONS *************/
void displayMain( void )
{
#if defined(SHOW_TEXT_STEPS)
	printf( "Refreshing screen.\nClearing background.\n" );
#endif
	glClear( GL_COLOR_BUFFER_BIT );
	// draw everything.
	drawAllObj();
/*
	if( p_state == EDIT_OBJ ){
	// Only draw the object with edit points when in edit mode.
		drawCurrent();
	}
*/
#if defined(SHOW_TEXT_STEPS)
	printf( "Swapping buffers.\n\n" );
#endif
	glutSwapBuffers();
}
/*------------------------------------------------------------*/
void mouseMain( int button, int state, int x, int y )
{
	if( button == GLUT_LEFT_BUTTON ){
		if( state == GLUT_DOWN ){
			switch( p_state ){
				case ADD_OBJ:
					// set up a new item to draw.
					MyObject* t;
					t = new MyObject;
					t->type = options.type;
					t->r = options.r;
					t->b = options.b;
					t->g = options.g;
					t->x0 = t->x1 = x;
					t->y1 = t->y0 = y;
					t->isFilled = options.isFilled;
					t->next = NULL;
					current = t;
					addMyObj( t );
					break;
				case EDIT_OBJ:
					dx = x;
					dy = y;
					current = findFirst( x, y );  // Find the first object that far	down.
					break;
				case DELETE_OBJ:
					current = findFirst( x, y );
					if( current != head ) // then it's not the background
						removeCurrent();
					break;
				default:
					p_state = ADD_OBJ;
			};
		} else { // the left button was raised

			switch( p_state ){
				case ADD_OBJ:
					// check that there are valid coordinates
					if(  (current->x0 == current->x1)
					     || (current->y0 == current->y1 ) ){
						removeCurrent();
					}
					break;
				case EDIT_OBJ:
					// Nothing to do right now.
					break;
				case DELETE_OBJ:
					// Nothing to do here either.
					break;
				default:
					p_state = ADD_OBJ;
					break;
			};
		}// end of else
	}// end of left button
	glutPostRedisplay();
}
/*------------------------------------------------------------*/
void reshapeMain( int w, int h )
{
	glutReshapeWindow( Max(w, 200), Max(h, 200) );
	glViewport( 0, 0, Max(w,200), Max(h,200) );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glutPostRedisplay();
}
/*------------------------------------------------------------*/
void drawPoint( int x, int y, GLfloat size )
{
	glPointSize( size );
	glBegin(GL_POINTS );
	glVertex2i( x, y );
	glEnd();
}
/*------------------------------------------------------------*/
void drawLine( int x0, int y0, int x1, int y1, GLfloat size )
{
	glPointSize( size );
	glBegin( GL_LINES );
	glVertex2i( x0, y0 );
	glVertex2i( x1, y1 );
	glEnd();
}
/*------------------------------------------------------------*/
void motionMain( int x, int y )
{
	int mx;
	int my;

	switch( p_state ){
		case ADD_OBJ: // Then reshape the current object.
			current->x1 = x;
			current->y1 = y;
			break;
		case EDIT_OBJ:
			mx = ( x - dx );
			my = ( y - dy );
			current->x0 += mx;
			current->x1 += mx;
			current->y0 += my;
			current->y1 += my;
			
			dx = x;
			dy = y;
			break;
		case DELETE_OBJ:
			// do nothing for now.
			break;
		default:
			p_state = ADD_OBJ;
	};
	glutPostRedisplay();
}
/*------------------------------------------------------------*/
void handleMenu( int value )
{

	switch( value ){
		case MENU_EDIT:
			p_state = EDIT_OBJ;
			break;
		case MENU_DELETE:
			p_state = DELETE_OBJ;
			break;
		case MENU_CLEAR:
			deleteAll();
			break;
		case MENU_SWAP:
#if defined(SHOW_TEXT_STEPS)
			printf( "Swapping buffers.\n" );
#endif
			glutSwapBuffers();
			glutSwapBuffers();
			break;
		case MENU_EXIT:
			cleanUp();
			exit(0);
	};
}
/*------------------------------------------------------------*/
void drawMenu( int value )
{
#if defined(SHOW_TEXT_STEPS)
	printf( "Changing the type of object to be drawn.\n" );
#endif

	options.type = value;

	p_state = ADD_OBJ;
}
/*------------------------------------------------------------*/
void colorMenu( int value )
{
#if defined(SHOW_TEXT_STEPS)
	printf( "Changing the current drawing color.\n" );
#endif

	switch( value ){
		case COLOR_BLACK:
			options.r = 0.0;
			options.g = 0.0;
			options.b = 0.0;
			break;
		case COLOR_WHITE:
			options.r = 1.0;
			options.g = 1.0;
			options.b = 1.0;
			break;
		case COLOR_RED:
			options.r = 1.0;
			options.g = 0.0;
			options.b = 0.0;
			break;
		case COLOR_GREEN:
			options.r = 0.0;
			options.g = 1.0;
			options.b = 0.0;
			break;
		case COLOR_BLUE:
			options.r = 0.0;
			options.g = 0.0;
			options.b = 1.0;
			break;
		case COLOR_YELLOW:
			options.r = 1.0;
			options.g = 1.0;
			options.b = 0.0;
			break;
		case COLOR_PURPLE:
			options.r = 1.0;
			options.g = 0.0;
			options.b = 1.0;
			break;
		case COLOR_ORANGE:
			options.r = 1.0;
			options.g = 0.5;
			options.b = 0.0;
			break;
		case COLOR_CUSTOM:
			break;
		default:
			options.r = options.b = options.g = 0.0;
			break;
	};
}
/*------------------------------------------------------------*/
void fillMenu( int value )
{
	options.isFilled = ( value==MENU_YES ? 1 : 0 );
#if defined(SHOW_TEXT_STEPS)
	printf( "Turning the fill to on/off.\n" );
#endif
}
/*------------------------------------------------------------*/
void backMenu( int value )
{
	GLfloat r, g, b;
#if defined(SHOW_TEXT_STEPS)
	printf( "Changing the background color.\n" );
#endif
	switch( value ){
		case COLOR_BLACK:
			r = 0.0;
			g = 0.0;
			b = 0.0;
			break;
		case COLOR_WHITE:
			r = 1.0;
			g = 1.0;
			b = 1.0;
			break;
		case COLOR_RED:
			r = 1.0;
			g = 0.0;
			b = 0.0;
			break;
		case COLOR_GREEN:
			r = 0.0;
			g = 1.0;
			b = 0.0;
			break;
		case COLOR_BLUE:
			r = 0.0;
			g = 0.0;
			b = 1.0;
			break;
		case COLOR_YELLOW:
			r = 1.0;
			g = 1.0;
			b = 0.0;
			break;
		case COLOR_PURPLE:
			r = 1.0;
			g = 0.0;
			b = 1.0;
			break;
		case COLOR_ORANGE:
			r = 1.0;
			g = 0.5;
			b = 0.0;
			break;
		case COLOR_CUSTOM:
			break;
	};

	// change the clear color.
	glClearColor( r, g, b, 0.0 );
	glutPostRedisplay();
}
/*------------------------------------------------------------*/
void cleanUp( void )
{
#if defined(SHOW_OBJECTS_AT_EXIT)
	MyObject* t;
	t = head;
	while( t != NULL ){
		switch( t->type ){
			case OBJ_RECTANGLE:
				printf( "\nRectangle with coordinates (%d,%d) and (%d,%d)\n",
					t->x0, t->y0, t->x1, t->y1 );
				break;
			case OBJ_SQUARE:
				printf( "\nSquare with coordinates (%d,%d) and (%d,%d)\n",
					t->x0, t->y0, t->x1, t->y1 );
				break;
			default:
				printf( "\nUnknown object type found.\n" );
				break;
		}; // switch.

		// always show the colors for all objects.
		printf( "Colors: Red - %f, Green - %f, Blue - %f\n",
			t->r, t->g, t->b );

		t = t->next;
	}// while
#endif

	deleteAll();

	// post any quitting messages...
	printf( "Good bye\n" );
}
/*------------------------------------------------------------*/
void deleteAll( void )
{
	MyObject* t;
	t = head;
	while( t ){
		head = t->next;
		delete t;
		t = head;
	}// while
}
/*------------------------------------------------------------*/
/*------------------------------------------------------------*/
/*------------------------------------------------------------*/
void init( int argc, char **argv )
{
	int c_menu, d_menu, f_menu, b_menu;
	// Set things up.

	// Initialize glut
	glutInit( &argc, argv );

	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize( DEFAULT_WIDTH, DEFAULT_HEIGHT );
	glutInitWindowPosition( INIT_X, INIT_Y );
	mainWin = glutCreateWindow( WINDOW_TITLE );

	// Setup the callback functions.
	glutReshapeFunc( reshapeMain );
	glutDisplayFunc( displayMain );
	glutMouseFunc( mouseMain );
	glutMotionFunc( motionMain );

	// Create the color submenu.
	c_menu = glutCreateMenu( colorMenu );
	glutAddMenuEntry( "Black", COLOR_BLACK );
	glutAddMenuEntry( "White", COLOR_WHITE );
	glutAddMenuEntry( "Red", COLOR_RED );
	glutAddMenuEntry( "Green", COLOR_GREEN );
	glutAddMenuEntry( "Blue", COLOR_BLUE );
	glutAddMenuEntry( "Yellow", COLOR_YELLOW );
	glutAddMenuEntry( "Purple", COLOR_PURPLE );
	glutAddMenuEntry( "Orange", COLOR_ORANGE );
	glutAddMenuEntry( "Custom...", COLOR_CUSTOM );

	// setup the object menu
	d_menu = glutCreateMenu( drawMenu );
	glutAddMenuEntry( "Rectangle", MENU_RECTANGLE );
	glutAddMenuEntry( "Square", OBJ_SQUARE );
	glutAddMenuEntry( "Ellipse", MENU_ELLIPSE );
	glutAddMenuEntry( "Super Ellispse", MENU_SUPER_E );
	glutAddMenuEntry( "Circle", MENU_CIRCLE );

	// Set up the fill sub menu
	f_menu = glutCreateMenu( fillMenu );
	glutAddMenuEntry( "Fill On", MENU_YES );
	glutAddMenuEntry( "Fill Off", MENU_NO );

	b_menu = glutCreateMenu( backMenu );
	glutAddMenuEntry( "Black", COLOR_BLACK );
	glutAddMenuEntry( "White", COLOR_WHITE );
	glutAddMenuEntry( "Red", COLOR_RED );
	glutAddMenuEntry( "Green", COLOR_GREEN );
	glutAddMenuEntry( "Blue", COLOR_BLUE );
	glutAddMenuEntry( "Yellow", COLOR_YELLOW );
	glutAddMenuEntry( "Purple", COLOR_PURPLE );
	glutAddMenuEntry( "Orange", COLOR_ORANGE );
	glutAddMenuEntry( "Custom...", COLOR_CUSTOM );

	// Create the main menu controls
	glutCreateMenu( handleMenu );
	glutAddSubMenu( "Draw Object", d_menu );
	glutAddSubMenu( "Draw Color", c_menu );
	glutAddSubMenu( "Set Fill", f_menu );
	glutAddSubMenu( "Change Background", b_menu );
	glutAddMenuEntry( "Edit Mode", MENU_EDIT );
	glutAddMenuEntry( "Delete Mode", MENU_DELETE );
	glutAddMenuEntry( "Clear All", MENU_CLEAR );
	glutAddMenuEntry( "Swap Buffers", MENU_SWAP );
	glutAddMenuEntry( "Exit", MENU_EXIT );

	glutAttachMenu( GLUT_RIGHT_BUTTON );

	// Set the options for drawing.
	options.type = OBJ_RECTANGLE;
	options.r = 1.0;
	options.g = 0.0;
	options.b = 0.0;
	options.isFilled = 0;

	// Current program state
	p_state = ADD_OBJ;

	// set the initial clearing color
	glClearColor( DEFAULT_R, DEFAULT_G, DEFAULT_B, DEFAULT_ALPHA );


	// attempt to change the camera angle so that drawn objects start showing up.

}
/*------------------------------------------------------------*/
int main( int argc, char **argv )
{
	// post a startup banner for the user.

	printf( "\n+----------------------+\n|  Jason F. Smith      |" );
	printf( "\n|  CSE 418  Fall 2000  |\n|  Project 1           |" );
	printf( "\n+----------------------+\n" );

	init( argc, argv);

	glutMainLoop();

	return 0;
}
