/*

	Jason F. Smith
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
 
/*------------------------------------------------------------+
	Chooses which functions will be used by including or
	commenting the following define.  Without this define,
	the program uses the redefined modeling transformations.
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

		// Matrix manipulation routines.
		// These replace glLoadIdentity(), glPushMatrix(), and glPopMatrix()
		extern inline void _myLoadIdentity( void );  // Clears the stack of matricies
		extern inline void _myPushMatrix( void );
		extern inline void _myPopMatrix( void );

		// Functions to actually manipulate the modeling transformations.
		// Replace glTranslate(), glRotate(), and glScale.
		extern inline void _myTranslate( double dx, double dy );
		extern inline void _myRotate( double theta );
		extern inline void _myScale( double sx, double sy );

		// Use this routine to draw the point.
		// The current transformation is performed on the point,
		//   and then it is drawn using the translated values.
		extern inline void _myVertex2i( int x, int y );
		extern inline void _myVertex2d( double x, double y ); // same thing with doubles

		#define  myLoadIdentity()		_myLoadIdentity()
		#define  myPushMatrix()			_myPushMatrix()
		#define  myPopMatrix()			_myPopMatrix()
		#define  myTranslate(A,B)		_myTranslate(A,B)
		#define  myRotate(A)			_myRotate(A)
		#define  myScale(A,B)			_myScale(A,B)
		#define  myVertex2i(A,B)		_myVertex2i(A,B)
		#define  myVertex2d(A,B)		_myVertex2d(A,B)
	#endif /* !defined USE_OPENGL_TRANSFORMATIONS */
#endif /* defined MATRIX_TRANSFORMATIONS_H */