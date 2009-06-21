/*

	Jason F. Smith  4588
	CSE 418 Project 2
	Fall 2000

	This file contains all of the 2D model transformation routines.
	These routines only effects points rendered using the included
	myVertex() function.  Every point drawn through myVertex will
	be tranformed through the current model view matrix.

*/

#ifndef MATRIX_TRANSFORMATIONS_H
#define	MATRIX_TRANSFORMATIONS_H

#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
 
/*------------------------------------------------------------+
	Chooses which functions will be used by including or
	commenting the following define.  Without this define,
	the program uses the re-defined modeling transformations.
 +------------------------------------------------------------*/
//#define  USE_OPENGL_TRANSFORMATIONS

/*-----------------------------------------+
	Definitions of functions without
	the underscore.  Controlled by the
	define above.
 +-----------------------------------------*/
	#if defined( USE_OPENGL_TRANSFORMATIONS )
		#define  myLoadIdentity()		glLoadIdentity()
		#define  myPushMatrix()			glPushMatrix()
		#define  myPopMatrix()			glPopMatrix()
		#define  myTranslate(A,B)		glTranslated(A,B,1.0)
		#define  myRotate(A)			glRotated(A,0.0,0.0,1.0)
		#define  myScale(A,B)			glScaled(A,B,1.0)
		#define  myVertex2i(A,B)		glVertex2i(A,B)
		#define  myVertex2d(A,B)		glVertex2d(A,B)
	#else

/*---------------------------------------------------------------------------+
	Here is the actual implementation of the re-defined
	transformation functions.
 +---------------------------------------------------------------------------*/
double sin(double);
double cos(double);
double acos(double);
// Get pi.
const double PI = acos(-1.0);

/*----------------------------------------------------+
	The following macro causes a debugging test to
	check to make sure there is something on the
	stack when calling myPopMatrix.
 +----------------------------------------------------*/
//#define CHECK_FOR_MATRIX_STACK

/*----------------------------------------------------+
	The myMatrix structure is simply a list of
	the pushed matricies.  Does NOT include the 
	current matrix.
 +----------------------------------------------------*/
struct myMatrix{
	double m[2][3];
	/*
		m[2][3] =	|  m(0,0)  m(0,1)  m(0,2)  |
				|  m(1,0)  m(1,1)  m(1,2)  |
	*/
	myMatrix *next; // next object on the stack;
};

// The current myMatrix stack.
myMatrix* myStack=NULL;

/*----------------------------------------------------+
	A stand-along matrix is used to store the 
	current matrix.  This avoids the structure/pointer
	overhead while performing math/tranformations and
	also while computing the final vertex point.
 +----------------------------------------------------*/
double c[2][3];  // same as the myMatrix->m matrix.

/*----------------------------------------------------+
	Clears the current transformation matrix.
 +----------------------------------------------------*/
inline void myLoadIdentity( void )
{
	myMatrix *t;
	while( myStack ){
		t = myStack;
		myStack = t->next;
		delete t;
	}// while
	myStack=NULL;
	// set identity properties
	c[0][0]=1;  c[0][1]=0;  c[0][2]=0;
	c[1][0]=0;  c[1][1]=1;  c[1][2]=0;
} // myLoadIdentity

/*----------------------------------------------------+
	Copies the current transformation matrix and 
	stores one copy on the stack.
 +----------------------------------------------------*/
inline void myPushMatrix( void )
{
	myMatrix *t;
	t = new myMatrix;
	t->next = myStack;
	myStack=t;

	t->m[0][0]=c[0][0];  t->m[0][1]=c[0][1];  t->m[0][2]=c[0][2];
	t->m[1][0]=c[1][0];  t->m[1][1]=c[1][1];  t->m[1][2]=c[1][2];
} // myPushMatrix

/*----------------------------------------------------+
	Replaces the current transformation matrix with
	the one on top of the stack.
 +----------------------------------------------------*/
inline void myPopMatrix( void )
{
#if defined( CHECK_FOR_MATRIX_STACK )
	if( !myStack ){
		printf( "\n\n *** Error *** - myPopMatrix called with nothing on stack.\n\n" );
		exit(0);
	}
#endif
	// Get matrix off stack.
	myMatrix *t;
	t=myStack;
	myStack = t->next;

	// Copy matrix.
	c[0][0]=t->m[0][0];  c[0][1]=t->m[0][1];  c[0][2]=t->m[0][2];
	c[1][0]=t->m[1][0];  c[1][1]=t->m[1][1];  c[1][2]=t->m[1][2];

	// Delete structure.
	delete t;
} // myPopMatrix

/*----------------------------------------------------+
	Translate the point by dx in x direction and
	dy in the y direction.

				|	1	0	dx	|
	M(new) = M(old) *	|	0	1	dy	|
				|	0	0	1	|
 +----------------------------------------------------*/
inline void myTranslate( double dx, double dy )
{
	// The upper left 2x2 matrix remains undisturbed.
	// Only effects c[0][2] and c[1][2].
	c[0][2] = dx*c[0][0] + dy*c[0][1] + c[0][2];
	c[1][2] = dx*c[1][0] + dy*c[1][1] + c[1][2];
} // myTranslate

/*----------------------------------------------------+
	Rotate the point by theta
	from the x-axis (rotation around z-axix).

			  	|  cos	-sin	0	|
	M(new) = M(old) *  	|  sin	cos	0	|
				|   0    0      1	|
 +----------------------------------------------------*/
inline void myRotate( double theta )
{
	// myRotate takes theta as degrees, but cos and sin
	//   use radians.  Conversion required.
	theta = theta*PI/180.0;
	double cos_theta = cos(theta); // calculate only once.
	double sin_theta = sin(theta); // calculate only once.
	double t00 = c[0][0];  // temporarily save value
	double t10 = c[1][0];  // temporarily save value
	double t01 = c[0][1];
	double t11 = c[1][1];

	c[0][0] = t00*cos_theta + t01*sin_theta;
	c[0][1] = t01*cos_theta - t00*sin_theta;
	c[1][0] = t10*cos_theta + t11*sin_theta;
	c[1][1] = t11*cos_theta - t10*sin_theta;
} // myRotate

/*----------------------------------------------------+
	Scale the point by (sx, sy)

				|	sx	0	0	|
	M(new) = M(old) *	|	0	sy	0	|
				|	0	0	1	|
 +----------------------------------------------------*/
inline void myScale( double sx, double sy )
{
	c[0][0]*=sx;	c[0][1]*=sy;
	c[1][0]*=sx;	c[1][1]*=sy;
} // myScale

/*----------------------------------------------------+
	Function to draw a single point.

	| x' |	     | x |
	| y' | = M * | y |
	| 1  |       | 1 |
 +----------------------------------------------------*/
inline void myVertex2d( double x, double y )
{
	double x2 = x*c[0][0] + y*c[0][1] + c[0][2];
	double y2 = x*c[1][0] + y*c[1][1] + c[1][2];
	glVertex2d( x2, y2 );
} // myVertex

#define		myVertex(A,B)		myVertex2d(A,B)

inline void myVertex2i( int x, int y )
{
	double x2 = x*c[0][0] + y*c[0][1] + c[0][2];
	double y2 = x*c[1][0] + y*c[1][1] + c[1][2];
	glVertex2i( (int)(x2+0.5), (int)(y2+0.5) );
} // myVertex2i

inline void myRect( double x1, double y1, double x2, double y2 )
{
	glBegin( GL_QUADS );
		myVertex2d( x1, y1 );
		myVertex2d( x1, y2 );
		myVertex2d( x2, y2 );
		myVertex2d( x2, y1 );
	glEnd();
}// myRectd

inline void myTriangle( double x1, double y1,
						double x2, double y2,
						double x3, double y3	)
{
	glBegin( GL_TRIANGLES );
		myVertex2d( x1, y1 );
		myVertex2d( x2, y2 );
		myVertex2d( x3, y3 );
	glEnd();
}// myTriangle

	#endif /* !defined USE_OPENGL_TRANSFORMATIONS */
#endif /* defined MATRIX_TRANSFORMATIONS_H */
