/*
	Jason F Smith	4588
	CSE 418   Project 2
	Fall 2000
	
	Global defines used throughout.
*/


#ifndef PROJECT_2_DEFS_H
#define PROJECT_2_DEFS_H

#include "myBug.h"


#define		Max(A,B)		( (A)>=(B) ? (A) : (B) )
#define		Min(A,B)		( (A)<(B) ? (A) : (B) )
#define		Abs(A)			( (A)>=0.0 ? (A) : (-(A)) )
#define		Sign(A)			( (A)>=0.0 ? (1.0) : (-1.0) )

// Menu Selections ----------------------------------------------
enum 	{	MENU_EXIT,
		MENU_CLEAR,
		MENU_FOOD,
		MENU_PREY,
		MENU_PRED,
		MENU_FASTER,
		MENU_SLOWER,
		MENU_DEFAULT,
		MENU_EXTRA
	};

// Pred/Prey/Food defs -----------------------------------------
#define		FOOD_NAME		"Power Pellet"
#define		FOOD_SIZE		4
#define		FOOD_TYPE		0x01
#define		FOOD_ROTATE_STEP	(6.0)
#define		ELECTRIC_STEP		(1.0)

// Used to determine how the prey react to eating the food.
enum	{	MENU_GROW,
			MENU_REV
		};

#define		PREY_NAME		"PacMan"
#define		PREY_SIZE		15
#define		PREY_RESOLUTION		7
#define		PREY_R			1.0
#define		PREY_G			0.66
#define		PREY_B			0.33
#define		PREY_STEP		0.31417
#define		PREY_TYPE		0x02
#define		PREY_MAX_SIZE	40
#define		SIZE_STEP		2

#define		PREY_CHARGE		(100.0*PI)
#define		CHARGE_STEP		(PI/10.0)

#define		PRED_NAME		"Ghost"
#define		PRED_TYPE		0x04
#define		PRED_SIZE		PREY_SIZE
#define		PRED_RESOLUTION	(PREY_RESOLUTION*2+1)
#define		PRED_MAX_SIZE	PREY_MAX_SIZE
#define		PRED_STEP		0.31417
#define		PRED_CAPE_RES	4

#define		EXTRA_STEP		(PI/30.0)
#define		EXTRA_TYPE		0x0010

#define		DEAD_GHOST		0x0200
#define		GLARE_STEP		(0.1);

// Should add in support for a timer, and although
// I know how to do it in Windows, I don't have time
// to figure it out on the Sparcs.
#if defined( _WIN32 )	// then it runs too fast
#define		DEFAULT_SPEED	0.25
#else // running on a Sparc
#define		DEFAULT_SPEED	1.0
#endif

// Current state of the game ------------------------------------
enum	{	
			NORMAL_GM,
			REV_GM
		};

//Window stuff --------------------------------------------------
#define		DEFAULT_WIDTH		400
#define		DEFAULT_HEIGHT		400
#define		INIT_X			200
#define		INIT_Y			000
#define		WINDOW_TITLE		"Jason F Smith - CSE 418 Project 2"

// Initial background colors ------------------------------------
#define		DEFAULT_R		0.0
#define		DEFAULT_G		0.0
#define		DEFAULT_B		0.0
#define		DEFAULT_ALPHA		0.0

// The initial drawing color ------------------------------------
#define		INITIAL_R		1.0
#define		INITIAL_G		0.0
#define		INITIAL_B		0.0

// Define the default point and line size -----------------------
#define		POINT_SIZE		1.0
#define		LINE_SIZE		1.0

// Function Declarations ----------------------------------------

extern void reshape( int w, int h );
extern void display( void );
extern void handleMenu( int value );
extern void mouseclick( int button, int state, int x, int y );
extern void userFeedback( int x, int y );
extern void init( void );
extern void cleanup( void );
extern void keyboard(unsigned char c, int x, int y );
extern void idleFunc( void );
extern void speedMenu( int value );
extern void foodMenu( int value );


// Various math functions.
double sin(double);
double cos(double);
double atan2(double, double);
#if defined( _WIN32 )
	#define		drand48()	((double)rand()/(RAND_MAX+1))
#else
	double drand48();
#endif
double fabs(double);
double sqrt(double);


// Global Data ---------------------------------------------------

extern int winWidth;
extern int winHeight;
extern int pState;  // determines what objects are added to the screen

extern int gmState;
class BugList;
extern BugList animals;
extern double dt;
extern int foodReaction;

#endif /* defined PROJECT_2_DEFS_H */
