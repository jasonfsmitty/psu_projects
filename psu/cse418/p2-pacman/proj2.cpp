/*
	Jason F. Smith  4588
	CSE 418  Project 2
	Fall 2000
*/


#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#if defined( _WIN32 )
#include <time.h>
#endif

#include "myTransform.h"	// Transformation routines
#include "myBug.h"
#include "p2_defs.h"		// Defines used throughout.


/*---------------------------------------------------------------+
	Global Data
 +---------------------------------------------------------------*/
int winWidth = DEFAULT_WIDTH;
int winHeight = DEFAULT_HEIGHT;

BugList animals;

int gmState;
int pState;
int foodReaction;
double dt;
BasicBug *current;


/*---------------------------------------------------------------+
	Called while program is idling.
 +---------------------------------------------------------------*/
void idleFunc( void )
{
	animals.MoveAll();

	glutPostRedisplay();
}

/*---------------------------------------------------------------+
 +---------------------------------------------------------------*/
void reshape( int w, int h )
{
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	myLoadIdentity();
	gluOrtho2D( 0, w, 0, h );
	glMatrixMode(GL_MODELVIEW );
	winWidth = w;
	winHeight = h;

	glutPostRedisplay();
} // reshape

/*---------------------------------------------------------------+
 +---------------------------------------------------------------*/
void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT );

	animals.DrawAll();

	glutSwapBuffers();
} // display

/*---------------------------------------------------------------+
	Chooses what type of bug to produce by clicks.
 +---------------------------------------------------------------*/
void bugMenu( int value )
{
	switch( value ){
		case MENU_FOOD:
			pState = MENU_FOOD;
			break;
		case MENU_PREY:
			pState = MENU_PREY;
			break;
		case MENU_PRED:
			pState = MENU_PRED;
			break;
		default:
			pState = MENU_PREY;
	};// switch
}// bugMenu

/*---------------------------------------------------------------+
	Main menu handling function.
 +---------------------------------------------------------------*/
void handleMenu( int value )
{
	switch( value ){
		case MENU_CLEAR:
			animals.DeleteAll();
			break;
		case MENU_EXIT:
			cleanup();
			exit(0);
			break;  // not reached
		default:
			break;
	}; // switch

	glutPostRedisplay();
} // handleMenu


/*---------------------------------------------------------------+
	Handle mouse clicks.
 +---------------------------------------------------------------*/
void mouseclick( int button, int state, int x, int y )
{
	if( button == GLUT_LEFT_BUTTON ){
		if( state==GLUT_DOWN ){
			if( current == NULL ){
				switch( pState ){
					case MENU_FOOD:
						current = new Food;
						break;
					case MENU_PREY:
						current = new Prey;
						break;
					case MENU_PRED:
						current = new Predator;
						break;
				}; // switch
				// initialize some of the data of the bug!!
				current->x = (GLfloat)x;	current->y = (GLfloat)(winHeight-y);
				current->vx = 0.0;		current->vy = 0.0;
				current->stride = 0.0;
				current->theta = 0.0;
				if( animals.isThere( current ) ){
					delete current;
					current = NULL;
				} else {
					if( pState==MENU_FOOD )
						animals.AddFirst(current);
					else
						animals.Add( current );
				} // if else
			}// end if(t)
		} else { // up click
			if(current){
				current->vx = x - current->x;
				current->vy = (winHeight-y) - current->y;
				if( current->vx==0.0 && current->vy==0.0 ) {
					current->vx = 0.707;
					current->vy = 0.707;
				}// if
				current = NULL;
			} // if(current)
		}// end up click
	}// button == GLUT_LEFT_BUTTON

} // mouseclick


/*---------------------------------------------------------------+
	Choose what happens when PacMan eats a pellet.
 +---------------------------------------------------------------*/
void foodMenu( int value )
{
	switch( value ){
	
		case MENU_REV:
			foodReaction = MENU_REV;
			break;
		case MENU_GROW:
		default:
			foodReaction = MENU_GROW;
			break;
	}; // switch
}// foodMenu

/*---------------------------------------------------------------+
 +---------------------------------------------------------------*/
void speedMenu( int value )
{
	switch( value ){
		case MENU_FASTER:
			dt *= 2.0;
			break;
		case MENU_SLOWER:
			dt *= 0.5;
			break;
		case MENU_DEFAULT:
		default:
			dt = DEFAULT_SPEED;
			break;
	}; // switch
}// speedMenu

/*---------------------------------------------------------------+
 +---------------------------------------------------------------*/
void userFeedback( int x, int y )
{
	if( current ){
		current->vx = x - current->x;
		current->vy = (winHeight-y) - current->y;
		if( current->vx==0.0 && current->vy==0.0 ) {
			current->vx = 0.707;
			current->vy = 0.707;
		}// if
	}// if(current)
} // userFeedback

/*---------------------------------------------------------------+
 +---------------------------------------------------------------*/
void init( void )
{
	// initialize window -----------------------------------
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize( DEFAULT_WIDTH, DEFAULT_HEIGHT );
	glutInitWindowPosition( INIT_X, INIT_Y );
	glutCreateWindow( WINDOW_TITLE );
	
	// clearing background color ---------------------------
	glClearColor( DEFAULT_R, DEFAULT_G, DEFAULT_B, DEFAULT_ALPHA );
	
	// Create menus ----------------------------------------
	int b_menu = glutCreateMenu( bugMenu );
	glutAddMenuEntry( FOOD_NAME, MENU_FOOD );
	glutAddMenuEntry( PREY_NAME, MENU_PREY );
	glutAddMenuEntry( PRED_NAME, MENU_PRED );

	int s_menu = glutCreateMenu( speedMenu );
	glutAddMenuEntry( "Faster", MENU_FASTER );
	glutAddMenuEntry( "Slower", MENU_SLOWER );
	glutAddMenuEntry( "Default", MENU_DEFAULT );
	
	int f_menu = glutCreateMenu( foodMenu );
	glutAddMenuEntry( "Grow Larger", MENU_GROW );
	glutAddMenuEntry( "Chase Ghosts", MENU_REV );

	// main menu
	glutCreateMenu( handleMenu );
	glutAddSubMenu( "Add...", b_menu );
	glutAddSubMenu( "Speed...", s_menu );
	glutAddSubMenu( "Food Reaction...", f_menu );
	glutAddMenuEntry( "Clear", MENU_CLEAR );
	glutAddMenuEntry( "Exit", MENU_EXIT );

	glutAttachMenu( GLUT_RIGHT_BUTTON );

	// Post event handlers ---------------------------------
	glutReshapeFunc( reshape );
	glutDisplayFunc( display );
	glutMouseFunc( mouseclick );
	glutMotionFunc( userFeedback );
	glutKeyboardFunc( keyboard );
	glutIdleFunc( NULL );
	glutIdleFunc( idleFunc );

	// Initialize data -------------------------------------
	dt = DEFAULT_SPEED;
	pState=MENU_PREY;
	gmState = NORMAL_GM;
	foodReaction = MENU_GROW;
} // init


/*---------------------------------------------------------------+
 +---------------------------------------------------------------*/
void cleanup( void )
{
	animals.DeleteAll();
} // cleanup


/*---------------------------------------------------------------+
 +---------------------------------------------------------------*/
void keyboard( unsigned char c, int x, int y )
{
	switch( c ){
		case 27: // escape
			cleanup();
			exit(0);
		default:
			break;
	}; // switch

} // keyboard()


/*---------------------------------------------------------------+
 +---------------------------------------------------------------*/
int main( int narg, char **argv )
{
	glutInit( &narg, argv );
	init();
	glutMainLoop();

	return 0; //never reached
} // main
