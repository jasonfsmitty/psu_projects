/*----------------------------------------------------------------------+

	Jason F Smith  4588
	CSE 418  Project 3

 +----------------------------------------------------------------------*/

#include <GL/glut.h>
#include <stdio.h>

#include <string>
#include <fstream>

using namespace std;


// Debug and control defines

#define		USE_EXTRA_DISPLAY_LISTS
#define		USE_SPLINE_DISPLAY_LISTS

//#define		FRAME_RATE_TEST_MODE

#define		USE_TEXTURES
#define		USE_HIGHLIGHTED_MENU
//#define		USE_SELECTION_BY_NAME  /* not implemented */
//#define		USE_MIDDLE_BUTTON_HACK

/*-------------------------------------------------------------------------+
	These macros control which textures are used.
 +-------------------------------------------------------------------------*/
#if defined( USE_TEXTURES )
	#define		USE_IMAGE_TEXTURES

	#if defined( USE_IMAGE_TEXTURES )
		#define USE_ARTWORK_IMAGES
		#if defined(USE_ARTWORK_IMAGES)
			const char *textureFiles[] = { "textures/dbz07.bmp",
					 "textures/face2.bmp",
					 "textures/figure.bmp",
					 "textures/hand1.bmp",
					 "textures/ken.bmp",
					 //"textures/nintendo.bmp",
					 "textures/stubby.bmp",
					 "textures/sunset.bmp",
					 //"textures/ifrit.bmp"
					};
			#define NUM_TEXTURES	 7
		#else // use sample pictures
			const char *textureFiles[] = { "textures/00.bmp",
						"textures/01.bmp",
						"textures/02.bmp",
						"textures/03.bmp",
						"textures/flowers.bmp",
						"textures/mandrill.bmp",
						"textures/tree.bmp"
						};
			#define	NUM_TEXTURES	7
		#endif // USE_ARTWORK_TEXTURES
	#endif // USE_IMAGE_TEXTURES

	#if !defined( USE_IMAGE_TEXTURES )
		#define	NUM_TEXTURES	2	// the number of manually computed textures
	#endif // !defined( USE_IMAGE_TEXTURES )

	#define	TurnTextureOn(A)		glBindTexture(GL_TEXTURE_2D,(A))

#else // not using textures
	#define	NUM_TEXTURES	1
#endif	// using textures



// Include files here -------------------------------------------------+

#include "mySpline2.h"
#if defined( USE_IMAGE_TEXTURES )
	#include "myTexture1.h"  // for the hammond lab
#endif

// Defines used in the project ----------------------------------------+

#define		Max(A,B)		( (A)>=(B) ? (A) : (B) )
#define		Min(A,B)		( (A)<(B) ? (A) : (B) )
#define		Abs(A)			( (A)>=0.0 ? (A) : (-(A)) )
#define		Sign(A)			( (A)>=0.0 ? (1.0) : (-1.0) )

 // for the window
#define		DEFAULT_WIDTH			400
#define		DEFAULT_HEIGHT			400
#define		WINDOW_TITLE			"Spline Editor V0.01"

#define		WINDOW_WIDTH			500
#define		WINDOW_HEIGHT			500

#if 0 //defined( USE_RANDOM_TEXTURE )
#define		BACK_RED				0.2
#define		BACK_GREEN				0.2
#define		BACK_BLUE				0.4
#define		BACK_ALPHA				1.0
#else
#define		BACK_RED				0.4
#define		BACK_GREEN				0.2
#define		BACK_BLUE				0.2
#define		BACK_ALPHA				1.0
#endif

#define		MIN_WIDTH				200
#define		MIN_HEIGHT				200
#define		BUTTON_WIDTH			50
#define		BUTTON_HEIGHT			17

#define		SPHERE_RADIUS			0.1
#define		SPHERE_STACKS			9
#define		SPHERE_SLICES			9

#define		TEAPOT_SIZE			0.2
 // modes of the program
enum{
		VIEW_MODE,
		EDIT_MODE,
		MOVE_MODE,
		SELECT_MODE,
		JOIN_MODE,
		SETC1_MODE,
		ADD_MODE,
		SAVE_MODE,
		LOAD_MODE,
		DELETE_MODE
	};

 // Menu choices
enum{
		MENU_FINER=0,
		MENU_COARSER,
		MENU_MAX,
		MENU_MIN,
		MENU_FLAT,
		MENU_SMOOTH,
		MENU_LIGHTS_ON,
		MENU_LIGHTS_OFF,
		MENU_TEXTURE_ON,
		MENU_TEXTURE_OFF,
		MENU_DELETE_ALL,
		MENU_X_ONLY,
		MENU_Y_ONLY,
		MENU_Z_ONLY,
		MENU_XY_PLANE,
		MENU_XZ_PLANE,
		MENU_YZ_PLANE,
		MENU_PARALLEL_PLANE,
		MENU_VIEWPORT,
		MENU_UNDO,
		MENU_VIEW_MODE,
		MENU_ADD_MODE,
		MENU_EDIT_MODE,
		MENU_DELETE_MODE,
		MENU_JOIN_MODE,
		MENU_SETC1_MODE,
		MENU_SAVE,
		MENU_LOAD,

		MENU_X_AXIS,
		MENU_Y_AXIS,
		MENU_Z_AXIS,
		MENU_NEG_Z_AXIS,
		MENU_RESET,

		MENU_ROTATE_XY,
		MENU_ROTATE_XZ,
		MENU_ROTATE_YZ,

		MENU_MORE_SENSITIVE,
		MENU_LESS_SENSITIVE,
		MENU_DEFAULT_SENSITIVE,

		MENU_BACK_0,
		MENU_BACK_1,
		MENU_BACK_2,
		MENU_BACK_BLACK,
		MENU_BACK_WHITE,

		MENU_CHANGE_LIGHT,

		MENU_EXIT
	};

const char *menuText[] = {
			"Finer (f)",		// MENU_FINER=0,
			"Coarser (c)",		// MENU_COARSER,
			"Maximum",		// MENU_MAX
			"Minimum",		// MENU_MIN
			"Off",			// MENU_FLAT
			"On",			// MENU_SMOOTH
			"On",			// MENU_LIGHTING_ON
			"Off",			// MENU_LIGHTING_OFF
			"On",			// MENU_TEXTURE_ON
			"Off",			// MENU_TEXTURE_OFF
			"Delete all",		// MENU_DELETE_ALL

			"X-axis only (x)",	// MENU_X_ONLY,
			"Y-axis only (y)",	// MENU_Y_ONLY,
			"Z-axis only (z)",	// MENU_Z_ONLY,
			"XY plane (p)",		// MENU_XY_PLANE,
			"XZ plane (p)",		// MENU_XZ_PLANE,
			"YZ plane (p)",		// MENU_YZ_PLANE,
			"Parallel plane (p)",	// MENU_PARALLEL_PLANE,
			"Change windows (w)",	// MENU_VIEWPORT,
			"Undo edit (u)",	// MENU_UNDO,
			"&View mode",		// MENU_VIEW_MODE,
			"Add mode (a)",		// MENU_ADD_MODE,
			"Edit mode (e)",	// MENU_EDIT_MODE,
			"Delete mode (d)",	// MENU_DELETE_MODE,
			"Join mode (j)",	// MENU_JOIN_MODE
			"Set C1 mode (s)",	// MENU_SETC1_MODE,
			"Save data",		// MENU_SAVE
			"Load data",		// MENU_LOAD

			"X-axis",		// MENU_X_AXIS,
			"Y-axis",		// MENU_Y_AXIS,
			"Z-axis", 		// MENU_Z_AXIS
			"Neg Z-Axis", 		// MENU_NEG_Z_AXIS,
			"Reset"	,		// MENU_RESET,

			"X-axis and Y-axis",	// MENU_ROTATE_XY
			"X-axis and Z-axis",	// MENU_ROTATE_XZ
			"Y-axis and Z-axis",	// MENU_ROTATE_YZ

			"More sensitive",	// MENU_MORE_SENSITIVE
			"Less sensitive",	// MENU_LESS_SENSITIVE
			"Default",		// MENU_DEFAULT_SENSITIVE

			"Background 1 (red)",	// MENU_BACK_0,
			"Background 2 (blue)",	// MENU_BACK_1,
			"Background 3 (green)",	// MENU_BACK_2
			"Black",		// MENU_BACK_BLACK,
			"White",		// MENU_BACK_WHITE,

			"Change light color", // MENU_CHANGE_LIGHT

			"Exit"			// MENU_EXIT
};


// how the mouse movement effects the control point
enum{
		PARALLEL_PLANE=0,
		XY_PLANE,
		XZ_PLANE,
		YZ_PLANE,
		X_ONLY,
		Y_ONLY,
		Z_ONLY
	};

// The four different viewports
enum{
		MAIN_WIN = 0,
		SMALL_1 = 1,
		SMALL_2 = 2,
		SMALL_3 = 3
	};

// Used to set the projections for the viewports
enum {
		Z_AXIS = 0,
		X_AXIS = 1,
		Y_AXIS = 2,
		NEG_Z_AXIS = 3
	};

// Global Variables --------------------------------------------------+

int pState; // what mode the program is in

mySpline* listHead=NULL;
mySpline* listTail=NULL;
mySpline* current=NULL;

mySpline* addOne;
mySpline* addTwo;
GLuint flagsOne, flagsTwo;

GLuint listSize;

struct vertexUndo{
	GLfloat oldV[3];
	int row, col;
	int currentRow, currentCol;
	bool entireSpline; // for moving everything
	bool newEdit;
	mySpline* s;
};

vertexUndo editedVertex;

int isControlPoint=0;

int oldWidth;
int oldHeight;
int winWidth; // Width of the window
int winHeight; // Height of the window

int moving=0, zooming=0, editing=0, editPlane=Y_ONLY, xStart, yStart, zStart, adding=0;
int choosingButton=0;
int highLightSpline=0;

GLfloat centerX = 0.0;
GLfloat centerY = 0.0;
GLfloat centerZ = 0.0;

GLfloat editReaction = 20.0;

// used to switch the projection view between the four windows
int projections[4] = { Z_AXIS, X_AXIS, Y_AXIS, NEG_Z_AXIS };
const char* axisNames[4] = { "Z-axis", "X-axis", "Y-axis", "Neg Z" };

GLuint displayList=0;
GLuint frameList=0;

GLfloat xAngle = 20.0;
GLfloat yAngle = 30.0;
GLfloat zAngle = 0.0;
GLuint rotateAxis=XY_PLANE;
GLfloat zoom = 0.0;

GLuint textureNames[NUM_TEXTURES];
GLuint defaultTextureName = 0;

bool globalModified = true;
bool centerObj = true;
bool shading = true;
bool globalLightingOn = true;
bool globalTexturingOn = true;
bool fakeMiddleButton = false;	// my laptop doesn't have a middle mouse button

GLfloat lightZeroPosition[] = { 5.0, 7.0, 5.0, 1.0 };
GLfloat lightZeroColor[][4] = {
				{ 0.0, 0.0, 2.0, 1.0 },
				{ 0.5, 0.0, 0.0, 1.0 },
				{ 0.0, 0.5, 0.5, 1.0 },
				{ 0.0, 0.2, 0.0, 1.0 },
				{ 0.8, 0.8, 0.0, 1.0 },
				{ 0.2, 0.2, 0.2, 1.0 },
				{ 0.8, 0.8, 0.8, 1.0 },
				{ 0.0, 0.0, 0.0, 1.0 }	};

GLfloat lightZeroAmbient[][4] = {
	{ 0.0, 0.0, 2.0, 1.0 },
	{ 0.0, 0.0, 2.0, 1.0 },
	{ 0.0, 0.0, 0.0, 1.0 },
	{ 0.0, 0.0, 0.0, 1.0 },
	{ 2.5, 0.0, 0.0, 1.0 },
	{ 0.0, 0.0, 4.0, 1.0 }
};
GLfloat lightZeroDiffuse[][4] = {
	{ 1.0, 1.0, 1.0, 1.0 },
	{ 1.0, 0.3, 0.0, 1.0 },
	{ 0.7, 0.7, 0.7, 1.0 },
	{ 1.0, 0.0, 0.0, 1.0 },
	{ 0.0, 0.0, 1.0, 1.0 },
	{ 1.0, 0.0, 0.0, 1.0 }
};

GLfloat lightRed[][4] = { {50.0, 0.0, 0.0, 1.0 }, {0.5, 0.5, 0.8, 1.0 } };
GLfloat lightGreen[][4] = { {0.0, 50.0, 0.0, 1.0 }, { 0.8, 0.4, 0.5, 1.0} };
GLfloat lightYellow[][4] = { {50.0, 50.0, 0.0, 1.0 }, { 0.4, 0.9, 0.8, 1.0 } };
GLfloat lightBlue[][4] = { {0.0, 0.0, 50.0, 1.0 }, { 0.4, 0.8, 0.6, 1.0 } };

#define NUM_LIGHTS	6
int lightIndex=0;
GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };

char testString[] = "ABCDEFGHIJKLMNOPQRSTUVWXY Z";

GLfloat controlPoints2[4][4][3] = {
	{ {2.0, 0.0, -2.0}, {1.0, 0.0, -2.0}, {-1.0, 0.0, -2.0}, {-2.0, 0.0, -2.0} },
	{ {2.0, 0.0, -1.0}, {1.0, 0.0, -1.0}, {-1.0, 0.0, -1.0}, {-2.0, 0.0, -1.0} },
	{ {2.0, 0.0, 1.0}, {1.0, 0.0, 1.0}, {-1.0, 0.0, 1.0}, {-2.0, 0.0, 1.0} },
	{ {2.0, 0.0, 2.0}, {1.0, 0.0, 2.0}, {-1.0, 0.0, 2.0}, {-2.0, 0.0, 2.0} }
};

GLfloat controlPoints[4][4][3] = {
	{ { -1.5, -1.5, 4.0 }, { -0.5, -1.5, 2.0 }, { 0.5, -1.5, -1.0 }, { 1.5, -1.5, 2.0 } },
	{ { -1.5, -0.5, 1.0 }, {-0.5, -0.5, 3.0 }, {0.5, 0.5, 0.0}, {1.5, -0.5, 0.0 } 	},
	{ { -1.5, 0.5, 4.0 }, { -0.5, 0.5, 0.0 }, {0.5, 0.5, 3.0 }, {1.5, 0.5, 4.0 } 	},
	{ {-1.5, 1.5, -2.0}, {-0.5, 1.5, -2.0 }, {0.5, 1.5, 0.0}, {1.5, 1.5, -1.0} 		}
};

// Choices for the on-screen menu
#define		NUM_BUTTONS		10
const char* buttons[] = { "Add", "View", "Edit", "Delete", "Save", "Load", "Finer", "Coarser", "Undo", "Exit" };
enum {
	ADD_BUTTON=0, VIEW_BUTTON, EDIT_BUTTON, DELETE_BUTTON,
	SAVE_BUTTON, LOAD_BUTTON, FINER_BUTTON, COARSER_BUTTON,
	UNDO_BUTTON, EXIT_BUTTON
};


GLfloat defaultTextColor[] = { 1.0, 0.8, 0.0 };
GLfloat defaultLineColor[] = { 1.0, 0.8, 0.0 };
GLfloat highLightTextColor[] = {1.0, 0.0, 0.0 };
int highLightText=-1;

const char* statusLine;
char defaultFileName[]="spline.dat";
char fileName[101];

// Function Declarations ----------------------------------------------+
void InitTextures( void );
void InitCallbacks( void );

void reshape( int w, int h );
void display( void );
void keyboard( unsigned char c, int x, int y );
void init( void );
void mouseClick( int button, int state, int x, int y );
void mouseMotion( int x, int y );
void cleanUp( void );
void passiveMotion( int x, int y );

// Menus
void InitMenus( void );
void mainMenu( int value );
void shadeMenu( int value );
void lightMenu( int value );
void resolutionMenu( int value );
void modeMenu( int value );
void editMenu( int value );
void textureMenu( int value );
void windowMenu( int value );
void defaultTexMenu( int value );
void rotationMenu( int value );
void sensitivityMenu( int value );
void backgroundMenu( int value );
void viewAxisMenu( int value );

void output( int x, int y, const char *string, GLfloat* color );
void beginOutput( void );
void bareOutput( int x, int y, const char* string );
void endOutput( void );

void undoChange( void );
void DrawAll( void );
void SetCurrent( int x, int y );
void AddSpline( mySpline* me );
void AddSpline( GLfloat pts[4][4][3] );
void IncreaseDivisions( void );
void DecreaseDivisions( void );
void MaxDivisions( void );
void MinDivisions( void );
void DeleteAll( void );
void SaveList( char *file );
void LoadList( char *file );
void SetModified( void );
void SetMode( int pMode );
void DeleteCurrent( void );
inline void HighLightOn( int x, int y );
inline void HighLightOff( void );
inline void viewProj( int direction );
void SetupAdd( int x, int y );
void SetPoints( int x, int y );

#if defined( FRAME_RATE_TEST_MODE )

	#define		TIME_PERIOD		1000
	char frameRateDisplay[] = "Frame rate: 000.00  Frames: 000    ";
	#define		FRAME_INDEX	17
	#define		FRAME2_INDEX	30
	double frameRate=0;
	long frameSum = 0;
	long period=TIME_PERIOD;	// in milliseconds
	double timePassed;		// in seconds

	void timerFunc( int value )
	{
		timePassed += period / 1000.0;
		frameRate = (double)frameSum / (double)( timePassed );
		int rate = frameRate*100.0+0.5;

		// edit string
		for( int i=0; i<6; i++){
			if( i!=2 ){
				frameRateDisplay[FRAME_INDEX-i] = (char)( rate%10+'0' );
				rate /= 10.0;
			}
		}

		rate = frameSum;
		for( int j=0; j<3; j++ ){
			frameRateDisplay[FRAME2_INDEX-j] = (char)( rate%10+'0' );
			rate /= 10;
		}

		glutTimerFunc( TIME_PERIOD, timerFunc, 0 );
	} // timerFunc

	void frameRateIdle( void )
	{
		glutPostRedisplay();
	}

#endif // defined FRAME_RATE_TEST


/*----------------------------------------------------------------------+
	Join two splines together
 +----------------------------------------------------------------------*/
void JoinSplines( mySpline* one, GLuint oneEdge, mySpline* two, GLuint twoEdge )
{
	if( one==two )
		return;


}

/*----------------------------------------------------------------------+
	Reshape the window.
 +----------------------------------------------------------------------*/
void reshape( int w, int h )
{
	winWidth = Max( w, MIN_WIDTH );
	winHeight = Max( h, MIN_HEIGHT );


	glutReshapeWindow( winWidth, winHeight );

	//globalModified=true;

#if defined( USE_EXTRA_DISPLAY_LISTS )
	glDeleteLists( frameList, 1 );
	frameList=0;
#endif

	glutPostRedisplay();
} // reshape


/*---------------------------------------------------------------------+
	Write characters to the screen
 +---------------------------------------------------------------------*/
void output(int x, int y, const char *string, GLfloat* color = defaultTextColor )
{
	int len, i;

	glColor3fv( color );
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	glViewport( 0, 0, winWidth, winHeight );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D( 0, winWidth, winHeight, 0 );
		glScalef( 1, -1, 1 );
		glTranslatef( 0, -winHeight, 0 );
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
			glLoadIdentity();
			glRasterPos2f(x, y);
			len = (int) strlen(string);
			for (i = 0; i < len; i++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
		glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	if( globalLightingOn )
		glEnable( GL_LIGHTING );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glEnable(GL_DEPTH_TEST );
}

// ---------------------------------------------------------------------
void beginOutput( void )
{
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	glColor3fv( defaultTextColor );
	glViewport( 0, 0, winWidth, winHeight );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D( 0, winWidth, winHeight, 0 );
		glScalef( 1, -1, 1 );
		glTranslatef( 0, -winHeight, 0 );
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
			glLoadIdentity();
}

// ---------------------------------------------------------------------
void endOutput( void )
{
		glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	if( globalLightingOn )
		glEnable( GL_LIGHTING );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glEnable(GL_DEPTH_TEST );
}

// ---------------------------------------------------------------------
void bareOutput( int x, int y, const char* string )
{
	glRasterPos2f(x, y);
	int len = (int) strlen(string);
	for ( int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
} // bareOutput


/*----------------------------------------------------------------------+
	depending on the requested direction, create the appropriate
	viewing transformations
 +----------------------------------------------------------------------*/
inline void viewProj( int direction, int x, int y, int width, int height )
{

	if( centerObj && current && pState!=ADD_MODE && !isControlPoint ){
		centerX = centerY = centerZ = 0.0;
		for( int i=0; i<4; i++ ){
			for( int j=0; j<4; j++ ){
				centerX += current->cntrlPts[i][j][0];
				centerY += current->cntrlPts[i][j][1];
				centerZ += current->cntrlPts[i][j][2];
			}// for j
		}// for i
		centerX /= 16.0;
		centerY /= 16.0;
		centerZ /= 16.0;
	}



	switch( direction ){
		case NEG_Z_AXIS:
			gluLookAt( 0.0, 0.0, -8.0,
				   //centerX, centerY, centerZ,
				   0.0, 0.0, 0.0,
				   0.0, 1.0, 0.0 );
			glViewport( x, y, width, height );
			glTranslatef( 0.0, 0.0, -zoom );
			glRotatef( -xAngle, 1.0, 0.0, 0.0 );
			glRotatef( yAngle, 0.0, 1.0, 0.0 );
			glRotatef( zAngle, 0.0, 0.0, 1.0 );
			break;

		case X_AXIS:
			gluLookAt( 8.0, 0.0, 0.0,
				   0.0, 0.0, 0.0,
				   //centerX, centerY, centerZ,
				   0.0, 1.0, 0.0 );
			glViewport( x, y, width, height );
			glTranslatef( zoom, 0.0, 0.0 );
			glRotatef( xAngle, 0.0, 0.0, 1.0 );
			glRotatef( yAngle, 0.0, 1.0, 0.0 );
			glRotatef( zAngle, 1.0, 0.0, 0.0 );
			break;

		case Y_AXIS:
			gluLookAt( 0.0, 8.0, 0.0,
				   //centerX, centerY, centerZ,
				   0.0, 0.0, 0.0,
				   0.0, 0.0, -1.0 );
			glViewport( x, y, width, height );
			glTranslatef( 0.0, zoom, 0.0 );
			glRotatef( -xAngle, 1.0, 0.0, 0.0 );
			glRotatef( yAngle, 0.0, 0.0, 1.0 );
			glRotatef( zAngle, 0.0, 1.0, 0.0 );
			break;

		case Z_AXIS:
		default:
			gluLookAt( 0.0, 0.0, 8.0,
				   0.0, 0.0, 0.0,
				   //centerX, centerY, centerZ,
				   0.0, 1.0, 0.0 );
			glViewport( x, y, width, height );
			glTranslatef( 0.0, 0.0, zoom );
			glRotatef( xAngle, 1.0, 0.0, 0.0 );
			glRotatef( yAngle, 0.0, 1.0, 0.0 );
			glRotatef( zAngle, 0.0, 0.0, 1.0 );
			break;
	};
	glTranslatef( -centerX, -centerY, -centerZ );
}

/*----------------------------------------------------------------------+
	Refresh the screen.
 +----------------------------------------------------------------------*/
void display( void )
{
	int w, h, i;
	w = winWidth/3;
	h = (winHeight-BUTTON_HEIGHT)/3+BUTTON_HEIGHT;
	int bottomy = BUTTON_HEIGHT+1;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

#if defined( USE_TEXTURES )
	if( globalTexturingOn && pState!=SELECT_MODE ){
		glEnable( GL_TEXTURE_2D );
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	} // if( texturing on )
#endif

	if( globalModified && displayList ) {
		glDeleteLists( displayList, 1 );
		displayList = 0;
	}
	globalModified=false;

	glColor3f( 1.0, 1.0, 1.0 );
	glLoadIdentity();

	// Draw the spline in the main window
	glPushMatrix();
		viewProj( projections[MAIN_WIN], 0, h, winWidth, (2*h) );
		DrawAll();
	glPopMatrix();

	glColor3f( 1.0, 0.0, 0.0 );

	if( pState!=SELECT_MODE ){

		// Draw the spline viewed from the x-axis -----------------
		glPushMatrix();
			viewProj( projections[SMALL_1], 1, bottomy, w, h-bottomy );
			DrawAll();
		glPopMatrix();

		// Draw the spline from the y-axis -----------------------
		glColor3f( 0.0, 1.0, 0.0 );
		glPushMatrix();
			viewProj( projections[SMALL_2], w, bottomy, w, h-bottomy );
			DrawAll();
		glPopMatrix();


		// Draw the spline from the z-axis --------------------
		glColor3f( 0.0, 0.0, 1.0 );
		glPushMatrix();
			viewProj( projections[SMALL_3], 2*w, bottomy, w-1, h-bottomy );
			DrawAll();
		glPopMatrix();

	// On to the on screen text and controls

#if defined( USE_TEXTURES )
	glDisable( GL_TEXTURE_2D );
#endif // USE_TEXTURES

		// Draw Text and controls
		beginOutput();

		int x = winWidth-BUTTON_WIDTH;
		int y = winHeight-((int)BUTTON_HEIGHT*NUM_BUTTONS);

		float step = (float)(winHeight-y) / (float)NUM_BUTTONS;


#if defined( USE_EXTRA_DISPLAY_LISTS )
		if( frameList==0 ){
			frameList=glGenLists( 1 );
			cout << "frameList = " << frameList << "\n";
			glNewList( frameList, GL_COMPILE_AND_EXECUTE );
#endif // USE_EXTRA_DISPLAY_LISTS
				glColor3fv( defaultLineColor );

				glBegin( GL_LINE_LOOP );
					glVertex2f( x, y );
					glVertex2f( x, winHeight-1 );
					glVertex2f( winWidth-1, winHeight-1 );
					glVertex2f( winWidth-1, y );
				glEnd();

				glBegin( GL_LINES );
					for( i=1; i<NUM_BUTTONS; i++ ){
						glVertex2f( (float)(x), (float)y+step*((float)i) );
						glVertex2f( (float)(winWidth-1), (float)y+step*((float)i) );
					} // for

					// Draw horizontal line
					glVertex2i( 1, h );
					glVertex2i( winWidth-1, h );

					glVertex2i( w, bottomy );
					glVertex2i( w, h );

					glVertex2i( 2*w, bottomy );
					glVertex2i( 2*w, h );

					glVertex2i( 1, h );
					glVertex2i( 1, 1 );

					glVertex2i( winWidth-1, h );
					glVertex2i( winWidth-1, 1 );

				glEnd();

					// Draw the status line
				glBegin( GL_LINE_LOOP );
					glVertex2i( 1, 1 );
					glVertex2i( 1, BUTTON_HEIGHT );
					glVertex2i( winWidth-1, BUTTON_HEIGHT );
					glVertex2i( winWidth-1, 1 );
				glEnd();

				glColor3fv( defaultTextColor );


#if defined( USE_EXTRA_DISPLAY_LISTS )
			glEndList();
		} else
			glCallList( frameList );
#endif // USE_EXTRA_DISPLAY_LISTS
		for( i=0; i<NUM_BUTTONS; i++ ){
#if defined(USE_HIGHLIGHTED_MENU)
			if( i==(NUM_BUTTONS-highLightText-1) ){
				glColor3fv( highLightTextColor );
				bareOutput( x+3, (int)(y+step*i+4), buttons[NUM_BUTTONS-i-1] );
				glColor3fv( defaultTextColor );
			} else
#endif // USE_HIGHTLIGHTED_MENU
				bareOutput( x+3, (int)(y+step*i+4), buttons[NUM_BUTTONS-i-1] );
		}// for
				// Draw labels for axis
		bareOutput( winWidth-40, h+3, axisNames[ projections[ MAIN_WIN ] ] );
		bareOutput( w-40, bottomy+3, axisNames[ projections[SMALL_1] ] );
		bareOutput( 2*w-40, bottomy+3, axisNames[ projections[SMALL_2] ] );
		bareOutput( winWidth-40, bottomy+3, axisNames[ projections[SMALL_3] ] );

#if defined( FRAME_RATE_TEST_MODE )
		frameSum++;
		bareOutput( 4, 5, frameRateDisplay );
#else // !defined( FRAME_RATE_TEST_MODE )
		bareOutput( 4, 5, statusLine );
#endif // FRAME_RATE_TEST_MODE

		if( listSize==0 )
			bareOutput( winWidth/3+10, winHeight/2+16, "No spline loaded" );
	}
	endOutput();

	if( pState!=SELECT_MODE )
		glutSwapBuffers();
} // display


/*----------------------------------------------------------------------+
	Functions to handle the spline list
 +----------------------------------------------------------------------*/

void DrawAll( void )
{
	mySpline* temp;
	long index=1;
	bool drawMode;
	bool doEndList = false;

#if 0
	// This display list is causing a fault when editing the spline.  Not
	// sure why, but running without it doesn't seem to hurt performance.
	if( displayList ) {
		glCallList( displayList );
		return;
	} else {
		displayList = glGenLists( 1 );
		cout << "displayList = " << displayList << "\n";
		glNewList( displayList, GL_COMPILE_AND_EXECUTE );
		doEndList = true;
	}
#endif

	temp = listHead;

	if( pState==SELECT_MODE )
		glDisable( GL_LIGHTING );
	else if( globalLightingOn )
		glEnable( GL_LIGHTING );
	else
		glDisable( GL_LIGHTING );

	glColor3f( 1.0, 1.0, 1.0 );

	while( temp!=NULL ){
		if( pState==SELECT_MODE ){
			glColor3ub( (index>>16)%255,
				   (index>>8)%255,
				   (index)%255	);
			index++;
			temp->modified=true;
			drawMode = temp->drawSolid;
			temp->drawSolid=true;
		} else if( pState==EDIT_MODE ){
			if( current ) {
				temp->drawSolid = (temp==current);
			}
		} else if( !highLightSpline )
			temp->drawSolid = true;

		temp->Draw();

		if( pState==SELECT_MODE ){
			temp->modified=true;
			temp->drawSolid = drawMode;
		}
		temp = temp->next;
	} // while

	if( current && (pState==EDIT_MODE || pState==SELECT_MODE) ){ // draw control points
		if( pState==SELECT_MODE )
			glDisable( GL_LIGHTING );
		else
			glEnable( GL_LIGHTING );
		glDisable( GL_TEXTURE_2D );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		for( int i=0; i<4; i++){
			switch( i ){
				case 0:
					glDisable( GL_LIGHT0 );
					glEnable( GL_LIGHT1 );
					break;
				case 1:
					glDisable( GL_LIGHT1 );
					glEnable( GL_LIGHT2 );
					break;
				case 2:
					glDisable( GL_LIGHT2 );
					glEnable( GL_LIGHT3 );
					break;
				case 3:
				default:
					glDisable( GL_LIGHT3 );
					glEnable( GL_LIGHT4 );
					break;
			}; // switch

			for( int j=0; j<4; j++ ){
				if( pState==SELECT_MODE ){
					glColor3ub( (index>>16)%256,
								(index>>8)%256,
								(index)%256		);
					index++;
				}

				glPushMatrix();
				glTranslatef( current->cntrlPts[i][j][0],
								current->cntrlPts[i][j][1],
								current->cntrlPts[i][j][2] );
				if( editedVertex.currentRow == i && editedVertex.currentCol==j && isControlPoint && pState!=SELECT_MODE ){
					glutSolidTeapot( TEAPOT_SIZE+Abs(zoom)/100.0 );
				} else {
					glutSolidSphere( SPHERE_RADIUS+Abs(zoom)/100.0, SPHERE_STACKS, SPHERE_SLICES );
				} // if else
				glPopMatrix();
			} // for j
		} // for i
		glDisable( GL_LIGHT4 );
		glEnable( GL_LIGHT0 );
		if( globalTexturingOn )
			glEnable( GL_TEXTURE_2D );
	}

	if( doEndList ) {
		glEndList();
	}
} // DrawAll( void )


/*----------------------------------------------------------------------+
	Motion by the mouse when not clicked
 +----------------------------------------------------------------------*/
void passiveMotion( int x, int y )
{
	y = winHeight-y;
	if( (winWidth-x)<BUTTON_WIDTH && y>(NUM_BUTTONS*BUTTON_HEIGHT) ){
		highLightText = NUM_BUTTONS-1-(BUTTON_HEIGHT*NUM_BUTTONS+y-winHeight)/BUTTON_HEIGHT;
	} else {
		highLightText=-1;
		if( highLightSpline && !isControlPoint ){
			HighLightOn(x, y);
			isControlPoint=false;
		}// if
	}
	glutPostRedisplay();
}

/*----------------------------------------------------------------------+
	Process mouse clicks.
 +----------------------------------------------------------------------*/
void mouseClick( int button, int state, int x, int y )
{
	mySpline* t;

	// check if the click is within the menu
	if( x>(winWidth-BUTTON_WIDTH) && (winHeight-y)>(winHeight-BUTTON_HEIGHT*NUM_BUTTONS) ){
		if( button==GLUT_LEFT_BUTTON ){
			if( state==GLUT_DOWN ){
				// enter menu selection mode
				choosingButton =1;
				return;
				// on up click, process menu choice
			} else if( state==GLUT_UP && choosingButton==1){
				int button = NUM_BUTTONS-1-(BUTTON_HEIGHT*NUM_BUTTONS-y)/BUTTON_HEIGHT;
				globalModified=true;
				switch( button ){
					case ADD_BUTTON:
						SetMode( ADD_MODE );
						break;
					case EDIT_BUTTON:
						SetMode( EDIT_MODE );
						//HighLightOn( x, winHeight-y );
						break;
					case VIEW_BUTTON:
						SetMode( VIEW_MODE );
						break;
					case FINER_BUTTON:
						resolutionMenu( MENU_FINER );
						break;
					case COARSER_BUTTON:
						resolutionMenu( MENU_COARSER );
						break;
					case SAVE_BUTTON:
						glDrawBuffer( GL_FRONT );
						output( winWidth/4, winHeight/2, "Enter filename on the command line." );
						glDrawBuffer( GL_BACK );
						cout <<"Enter filename to save (default: "<<defaultFileName<<") - ";
						cin  >>fileName;
						SaveList( fileName );
						break;
					case LOAD_BUTTON:
						glDrawBuffer( GL_FRONT );
						output( winWidth/4, winHeight/2, "Enter filename on the command line." );
						glDrawBuffer( GL_BACK );
						cout <<"Enter filename to load (default: "<<defaultFileName<<") - ";
						cin  >>fileName;
						LoadList( fileName );
						break;
					case UNDO_BUTTON:
						undoChange();
						break;
					case DELETE_BUTTON:
						//statusLine = "Button disabled.";
						SetMode( DELETE_MODE );
						break;
					case EXIT_BUTTON:
						mainMenu( MENU_EXIT );
						break;
					default:
						statusLine = "Error, unknown button pressed.";
						cout <<"Unknown menu button press, ignoring button "<<button<<endl;
						break;
				}; // switch
				// turn off
				choosingButton = 0;
				glutPostRedisplay();
				return;
			}
		}

	} else if( choosingButton==1 ){
		choosingButton=0;
		return;
	} else if( (winHeight-y)<( (winHeight-BUTTON_HEIGHT)/3+BUTTON_HEIGHT ) &&
		   (winHeight-y)>( (BUTTON_HEIGHT) ) &&
		   button==GLUT_LEFT_BUTTON		){
		if( state==GLUT_DOWN ){
			int holder;
			if( x<winWidth/3 ){
				holder = projections[MAIN_WIN];
				projections[MAIN_WIN] = projections[SMALL_1];
				projections[SMALL_1] = holder;
			} else if( x>winWidth/3 && x<2*winWidth/3 ){
				holder = projections[MAIN_WIN];
				projections[MAIN_WIN] = projections[SMALL_2];
				projections[SMALL_2] = holder;
			} else {
				holder = projections[MAIN_WIN];
				projections[MAIN_WIN] = projections[SMALL_3];
				projections[SMALL_3] = holder;
			}
			statusLine = "The view in the main window has been changed.";
			glutPostRedisplay();
			return;
		}
	}

#if defined(USE_MIDDLE_BUTTON_HACK)
	if( fakeMiddleButton && button==GLUT_LEFT_BUTTON ){
		button=GLUT_MIDDLE_BUTTON;
	}
#endif

	switch( pState ){
		case VIEW_MODE:
			if( button == GLUT_LEFT_BUTTON ){
				if( state == GLUT_DOWN ){
					moving = 1;
					xStart = x;
					yStart = y;
					statusLine = "Drag mouse to rotate object(s).";
				} else {
					statusLine = "Returned to normal viewing mode.";
					moving = 0;
				}
			} else if( button== GLUT_MIDDLE_BUTTON ){
				if( state==GLUT_DOWN ){
					zooming=1;
					zStart = y;
					statusLine = "Drag mouse to zooom in and out.";
				} else { // upclick
					zooming = 0;
					statusLine = "Returned to normal viewing mode.";
				} // else
			}
			break;

		case MOVE_MODE:
			if( button==GLUT_LEFT_BUTTON ){
				if( state==GLUT_DOWN ){
					cout <<"Illegal button press in move mode.  Should not be in move mode."<<endl;
				} else { // upclick
					mouseMotion( x, y ); // one last time.
					pState=EDIT_MODE;
					glutPostRedisplay();
					statusLine = "Returned to normal editing mode.";
				}// if else
			}// if
			break;

		case EDIT_MODE:
			// User is attempting to select a spline or control point
			if( button==GLUT_LEFT_BUTTON ){
				if( state==GLUT_DOWN ){
					// select the points
					SetCurrent( x, winHeight-y );
					editing=1;
					xStart=x;  yStart=y;  zStart=y;
					pState = EDIT_MODE;
					if( current && !isControlPoint ){
						statusLine = "Drag mouse to move the entire spline.";
						pState = MOVE_MODE; // move the spline!!
					} else if( !current ){
						highLightSpline=1;
					} else {
						editedVertex.newEdit=true;
					}
				} else {
					if( !isControlPoint )
						SetCurrent( x, winHeight-y);
					editing=highLightSpline=0;
					if( current==NULL )
						highLightSpline=1;
					pState = EDIT_MODE;
				} // if else
				globalModified=true;
			} else if( button == GLUT_MIDDLE_BUTTON ){
				if( state==GLUT_DOWN ){
					moving=1;
					xStart=x;
					yStart=y;
					statusLine = "Drag the mouse to rotate the object.";
				} else {
					moving = 0;
					statusLine = "Returned to normal editing mode.";
				} // else
			}
			break;

		case DELETE_MODE:
			if( button==GLUT_LEFT_BUTTON ){
				if( state==GLUT_DOWN ){
					// do nothing??
					HighLightOn( x, winHeight-y);
				} else { // on up click
					SetCurrent( x, winHeight-y );
					// if a current selected, then delete
					if( current ){
						DeleteCurrent();
						statusLine = "Spline deleted.";
					}
					//highLightSpline=0;
					globalModified=true;
				}
			} else if( button==GLUT_MIDDLE_BUTTON ){
				if( state==GLUT_DOWN ){
					moving=1;
					xStart=x;
					yStart=y;
					statusLine = "Drag the mouse to rotate the object(s).";
				} else {
					moving = 0;
					statusLine = "Returned to normal deletion mode.";
				}
			}
			break;

		case ADD_MODE:
			if( button==GLUT_LEFT_BUTTON ){
				if( state==GLUT_DOWN ){
					// Create a new spline and add it to the list.
					current = new mySpline;
					SetupAdd(x, winHeight-y  );
					adding=1;
					t=listHead;
					while( t ){
						t->drawSolid=false;
						t=t->next;
					} // while
					current->drawSolid=true;
					AddSpline( current );
					statusLine = "Drag the mouse to size the spline on the Y=0 plane.";
				} else { //up-click
					adding=0;
					// Make sure all of the points are set accordingly
					SetPoints( x, winHeight-y);
					pState = EDIT_MODE;
					statusLine = "Entered normal editing mode.";
				}
				glutPostRedisplay();
				globalModified=true;
			} else if( button==GLUT_MIDDLE_BUTTON ){
				if( state==GLUT_DOWN ){
					zooming=1;
					zStart = y;
					statusLine = "Drag the mouse to zoom in/out.";
				} else { // upclick
					zooming = 0;
					statusLine = "Returned to normal add mode.";
				} // else
			} // if-else
			break;
/*
		case JOIN_MODE:
			if( button==GLUT_LEFT_BUTTON ){
				if( state==GLUT_DOWN ){
					SetCurrent(x, winHeight-y);
					if( current ){
						choosingJoinSide=true;
						if( addOne ){ // add second
							if( current != addOne ){
								addTwo = current;
								flagsTwo = UP_EDGE;
							} else {
								choosingJoinSide = false;
								statusLine = "Must choose two different splines to join.";
							}
						} else {
							addOne = current;
							flagsOne = UP_EDGE;
						}
						statusLine = "Use arrow keys to choose edge to join.";
					} else {
						choosingJoinSide=0;
					}
				} else { // upclick
					if( choosingJoinSide ){
						if( addTwo ){
							// Set the second spline's edge flags and join the two

						} else { // save the first one's data
							// do nothing
							statusLine = "Click on second spline to finish the join.";
						}
					} // else not choosing, ignore
				} // else upclick of left button
			} else if( button==GLUT_MIDDLE_BUTTON ){
				if( state==GLUT_DOWN ){
					zooming=1;
					zStart = y;
					statusLine = "Drag the mouse to zoom in/out.";
				} else { // upclick
					zooming = 0;
					statusLine = "Returned to normal add mode.";
				} // else
			} // if-else
			break;
*/

		default:
			pState = VIEW_MODE;
			statusLine = "Unknown state, returning to view mode.";
			break;
	}; // switch
	//globalModified=true;
	glutPostRedisplay();
} // mouseClick


/*----------------------------------------------------------------------+
	Handle the mouse motion.
 +----------------------------------------------------------------------*/
void mouseMotion( int x, int y )
{
	GLdouble temp[3];	// temporarily store ??
	GLfloat dm[3];
	GLfloat *v;		// vertex to edit
	GLdouble sx, sy, sz;
	int h, j, i;

	if( moving ){
		switch( rotateAxis ){
			case XY_PLANE:
				yAngle += (x-xStart);
				xAngle += (y-yStart);
				xStart = x;
				yStart = y;
				break;
			case XZ_PLANE:
				zAngle += (x-xStart);
				xAngle += (y-yStart);
				xStart=x;  yStart=y;
				break;
			case YZ_PLANE:
				yAngle += (x-xStart);
				zAngle += (y-yStart);
				xStart=x;  yStart=y;
				break;
		};// switch
	} else if( zooming ) {
		zoom += (y-zStart)/10.0;
		zStart=y;
	} else if( editing && isControlPoint ){ // moving a control point

		if( editedVertex.newEdit && current ){
			editedVertex.entireSpline=false;
			editedVertex.newEdit=false;
			editedVertex.s = current;
			editedVertex.col = editedVertex.currentCol;
			editedVertex.row = editedVertex.currentRow;

			editedVertex.oldV[0] = current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][0];
			editedVertex.oldV[1] = current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][1];
			editedVertex.oldV[2] = current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][2];
		} // if( s!=current )

		switch( editPlane ){

			case PARALLEL_PLANE: //----------------------------------------------

				h = (winHeight-BUTTON_HEIGHT)/3 + BUTTON_HEIGHT;

				// Set the current projection to the main window
				glLoadIdentity();
				glPushMatrix();
					viewProj( projections[MAIN_WIN], 0, h, winWidth, (2*h) );

				// Get the current transformation matricies
				GLdouble model[16];
				GLdouble proj[16];
				GLint view[16];

				glGetDoublev( GL_MODELVIEW_MATRIX, model );
				glGetDoublev( GL_PROJECTION_MATRIX, proj );
				glGetIntegerv( GL_VIEWPORT, view );

				v = current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ];
				gluProject( v[0], v[1], v[2], model, proj, view, &sx, &sy, &sz );
				gluUnProject( x, winHeight-y, sz, model, proj, view, &temp[0], &temp[1], &temp[2] );

				v[0] = (float)temp[0];
				v[1] = (float)temp[1];
				v[2] = (float)temp[2];

				glPopMatrix();
				break;

			case XY_PLANE: //--------------------------------------------------------
				current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][0] +=(x-xStart)/editReaction;
				xStart=x;
				current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][1] +=(yStart-y)/editReaction;
				yStart=y;
				break;

			case XZ_PLANE: //--------------------------------------------------------
				current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][0] +=(x-xStart)/editReaction;
				xStart=x;
				current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][2] +=(y-zStart)/editReaction;
				zStart=y;
				break;

			case YZ_PLANE: //--------------------------------------------------------
				current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][1] +=(yStart-y)/editReaction;
				yStart=y;
				current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][2] +=(x-xStart)/editReaction;
				xStart=x;
				break;

			case X_ONLY: //----------------------------------------------------------
				current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][0] +=(x-xStart)/editReaction;
				xStart=x;
				break;

			case Y_ONLY:
				current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][1] +=(yStart-y)/editReaction;
				yStart=y;
				break;

			case Z_ONLY: //----------------------------------------------------------
				current->cntrlPts[ editedVertex.currentRow ][ editedVertex.currentCol ][2] +=(y-zStart)/editReaction;
				zStart=y;
				break;
		}; // switch

		SetModified();
		globalModified=true;
	} else if( (pState==DELETE_MODE || pState==EDIT_MODE) && highLightSpline ) {
		HighLightOn( x, winHeight-y);
	} else if( pState==ADD_MODE && adding ){
		SetPoints( x, winHeight-y );
	} else if( pState == MOVE_MODE ){
		switch( editPlane ){
			case PARALLEL_PLANE: //----------------------------------------------

				h = (winHeight-BUTTON_HEIGHT)/3 + BUTTON_HEIGHT;

				// Set the current projection to the main window
				glLoadIdentity();
				glPushMatrix();
					viewProj( projections[MAIN_WIN], 0, h, winWidth, (2*h) );

				// Get the current transformation matricies
				GLdouble model[16];
				GLdouble proj[16];
				GLint view[16];

				glGetDoublev( GL_MODELVIEW_MATRIX, model );
				glGetDoublev( GL_PROJECTION_MATRIX, proj );
				glGetIntegerv( GL_VIEWPORT, view );

				v = current->cntrlPts[0][0];
				gluProject( v[0], v[1], v[2], model, proj, view, &sx, &sy, &sz );
				gluUnProject( x, winHeight-y, sz, model, proj, view, &temp[0], &temp[1], &temp[2] );

				dm[0] = (float)temp[0] - v[0];
				dm[1] = (float)temp[1] - v[1];
				dm[2] = (float)temp[2] - v[2];

				glPopMatrix();
				break;

			case XY_PLANE: //--------------------------------------------------------
				dm[0] = (x-xStart)/editReaction;
				xStart = x;
				dm[1] = (yStart-y)/editReaction;
				yStart = y;
				dm[2] = 0.0;
				break;

			case XZ_PLANE: //--------------------------------------------------------
				dm[0] = (x-xStart)/editReaction;
				xStart = x;
				dm[1] = 0.0;
				dm[2] = (y-zStart)/editReaction;
				zStart = y;
				break;

			case YZ_PLANE: //--------------------------------------------------------
				dm[0] = 0.0;
				dm[1] = (yStart-y)/editReaction;
				yStart = y;
				dm[2] = (x-xStart)/editReaction;
				xStart = x;
				break;

			case X_ONLY: //----------------------------------------------------------
				dm[0] = (x-xStart)/editReaction;
				xStart = x;
				dm[1] = 0.0;
				dm[2] = 0.0;
				break;

			case Y_ONLY:
				dm[0] = 0.0;
				dm[1] = (yStart-y)/editReaction;
				yStart = y;
				dm[2] = 0.0;
				break;

			case Z_ONLY: //----------------------------------------------------------
				dm[0] = 0.0;
				dm[1] = 0.0;
				dm[2] = (y-zStart)/editReaction;
				zStart = y;
				break;
		}; // switch

		if( editedVertex.newEdit && current ){
			editedVertex.s = current;
			editedVertex.newEdit=false;
			editedVertex.entireSpline=true;

			editedVertex.oldV[0] = v[0];
			editedVertex.oldV[1] = v[1];
			editedVertex.oldV[2] = v[2];
		}

		for( i=0; i<4; i++ ){
			for( j=0; j<4; j++ ){
				current->cntrlPts[i][j][0] += dm[0];
				current->cntrlPts[i][j][1] += dm[1];
				current->cntrlPts[i][j][2] += dm[2];
			}// for j;
		} // for i
		globalModified=true;
		SetModified();
	}

	glutPostRedisplay();
} // mouseMotion


/*----------------------------------------------------------------------+
	set the second set of points for the current spline.
 +----------------------------------------------------------------------*/
void SetPoints( int x, int y )
{
	GLdouble t[3];
	GLfloat *v;
	GLfloat myz;
	GLdouble model[16];
	GLdouble proj[16];
	GLint view[4];

	int h = (winHeight-BUTTON_HEIGHT)/3 +BUTTON_HEIGHT;

	if( !current )
		return;

	glLoadIdentity();
	glPushMatrix();
		viewProj( projections[MAIN_WIN], 0, h, winWidth, 2*h );
		glGetDoublev( GL_MODELVIEW_MATRIX, model );
		glGetDoublev( GL_PROJECTION_MATRIX, proj );
		glGetIntegerv( GL_VIEWPORT, view );

		v = current->cntrlPts[3][3]; // get the final control point

		gluProject( 0.0, 0.0, 0.0, model, proj, view, &t[0], &t[1], &t[2] );
		myz = t[2];
		gluUnProject( x, y, myz, model, proj, view, &t[0], &t[1], &t[2] );

		v[0] = (float) t[0];
		v[1] = 0.0;
		v[2] = (float) t[2];

		// P(0,3)
		current->cntrlPts[0][3][0] = current->cntrlPts[3][3][0];
		current->cntrlPts[0][3][1] = 0.0;
		current->cntrlPts[0][3][2] = current->cntrlPts[0][0][2];

		GLfloat xStep = ( current->cntrlPts[0][0][0] - current->cntrlPts[0][3][0] ) / 3.0;
		GLfloat zStep = ( current->cntrlPts[0][3][2] - current->cntrlPts[3][3][2] ) / 3.0;

		// P(3.0)
		current->cntrlPts[3][0][2] = current->cntrlPts[3][3][2];
		current->cntrlPts[3][0][0] = current->cntrlPts[0][0][0];

		// P(0,1)
		current->cntrlPts[0][1][0] = current->cntrlPts[0][0][0]-xStep;
		current->cntrlPts[0][1][2] = current->cntrlPts[0][0][2];

		// P(0,2)
		current->cntrlPts[0][2][0] = current->cntrlPts[0][3][0]+xStep;
		current->cntrlPts[0][2][2] = current->cntrlPts[0][0][2];

		// P(1,0)
		current->cntrlPts[1][0][0] = current->cntrlPts[0][0][0];
		current->cntrlPts[1][0][2] = current->cntrlPts[0][0][2]-zStep;

		// P(2,0)
		current->cntrlPts[2][0][0] = current->cntrlPts[0][0][0];
		current->cntrlPts[2][0][2] = current->cntrlPts[3][0][2]+zStep;

		// P(3,1)
		current->cntrlPts[3][1][0] = current->cntrlPts[0][1][0];
		current->cntrlPts[3][1][2] = current->cntrlPts[3][0][2];

		// P(3,2)
		current->cntrlPts[3][2][0] = current->cntrlPts[0][2][0];
		current->cntrlPts[3][2][2] = current->cntrlPts[3][0][2];

		// P(1,3)
		current->cntrlPts[1][3][0] = current->cntrlPts[0][3][0];
		current->cntrlPts[1][3][2] = current->cntrlPts[1][0][2];

		// P(2,3)
		current->cntrlPts[2][3][0] = current->cntrlPts[0][3][0];
		current->cntrlPts[2][3][2] = current->cntrlPts[2][0][2];

		// P(1,1)
		current->cntrlPts[1][1][0] = current->cntrlPts[0][1][0];
		current->cntrlPts[1][1][2] = current->cntrlPts[1][0][2];

		// P(1,2)
		current->cntrlPts[1][2][0] = current->cntrlPts[0][2][0];
		current->cntrlPts[1][2][2] = current->cntrlPts[1][0][2];

		// P(2,2)
		current->cntrlPts[2][2][0] = current->cntrlPts[0][2][0];
		current->cntrlPts[2][2][2] = current->cntrlPts[2][0][2];

		// P(2,1)
		current->cntrlPts[2][1][0] = current->cntrlPts[0][1][0];
		current->cntrlPts[2][1][2] = current->cntrlPts[2][0][2];

	glPopMatrix();
	current->modified = globalModified = true;
}

/*---------------------------------------------------------------------+
	Setup the initial point of the spline
 +---------------------------------------------------------------------*/
void SetupAdd( int x, int y )
{
	GLdouble model[16];
	GLdouble proj[16];
	GLint view[4];
	GLdouble t[3], myz;

	int h = (winHeight-BUTTON_HEIGHT)/3 +BUTTON_HEIGHT;

	if( !current )
		return;

	glLoadIdentity();
	glPushMatrix();
		viewProj( projections[MAIN_WIN], 0, h, winWidth, 2*h );
		glGetDoublev( GL_MODELVIEW_MATRIX, model );
		glGetDoublev( GL_PROJECTION_MATRIX, proj );
		glGetIntegerv( GL_VIEWPORT, view );

		gluProject( 0.0, 0.0, 0.0, model, proj, view, &t[0], &t[1], &t[2] );
		myz = t[2];
		gluUnProject( x, y, myz, model, proj, view, &t[0], &t[1], &t[2] );

	glPopMatrix();


	for( int i=0; i<16; i++ ){
		current->cntrlPts[ (int)(i/16) ][ i%4 ][0] = (float)t[0];
		current->cntrlPts[ (int)(i/16) ][ i%4 ][1] = 0.0;
		current->cntrlPts[ (int)(i/16) ][ i%4 ][2] = (float)t[2];
	}

	current->modified = true;
	globalModified=true;
}

/*----------------------------------------------------------------------+
	Clean up any loose ends
 +----------------------------------------------------------------------*/
void cleanUp( void )
{
	DeleteAll();
} // cleanUp

/*----------------------------------------------------------------------+
	main function body
 +----------------------------------------------------------------------*/
int main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitWindowSize( DEFAULT_WIDTH, DEFAULT_HEIGHT );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutCreateWindow( WINDOW_TITLE );

/*
	winWidth = 640;
	winHeight = 480;
	glutGameModeString( "640x480:24@60" );
	glutEnterGameMode();
*/
/*
GLUTAPI void APIENTRY glutGameModeString(const char *string);
GLUTAPI int APIENTRY glutEnterGameMode(void);
GLUTAPI void APIENTRY glutLeaveGameMode(void);
GLUTAPI int APIENTRY glutGameModeGet(GLenum mode);
*/

	init();

	glutMainLoop();

	return 0; // never reached
} // main


/*----------------------------------------------------------------------+
	SetCurrent depending on the x,y value indicated
 +----------------------------------------------------------------------*/
void SetCurrent( int x, int y )
{
	GLfloat back[4];
	GLubyte temp[12];

	// save background color
	glGetFloatv( GL_COLOR_CLEAR_VALUE, back );

	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );

	int saveState = pState;
	pState = SELECT_MODE;
	globalModified=true;
	display();  // draw everything!!
	globalModified=true;
	pState = saveState;

	glReadPixels( x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, temp );

	int index = (GLuint)(temp[0]<<16) | (GLuint)(temp[1]<<8) | (GLuint)(temp[2]) ;

	// reset the back buffer
	glClearColor( back[0], back[1], back[2], back[3] );

	glutPostRedisplay();

	if( index==0 ){
		current = NULL;
		isControlPoint=0;
		editedVertex.currentRow = -1;
		editedVertex.currentCol = -1;
		return;
	} // if

	if( index <= (signed)listSize ){
		// use index and set the current
		mySpline* temp;
		temp = listHead;
		for( int i=1; i<index && temp!=NULL; i++ ){
			temp = temp->next;
		}
		current = temp;
		editedVertex.currentRow = -1;
		editedVertex.currentCol = -1;
		isControlPoint=0;
	} else if( index <= (signed)listSize+16 ){ // a control point
		isControlPoint=1;
		// keep current the same
		editedVertex.currentRow = (int)( (index-listSize-1)/4 );
		editedVertex.currentCol = (index-listSize-1)%4;
	}

}


/*---------------------------------------------------+
	Add a spline into the list.
 +---------------------------------------------------*/
void AddSpline( mySpline* me )
{
	if( listHead==NULL ){
		listHead=listTail=me;
		me->next=me->prev=NULL;
		listSize=1;
	} else {
		listTail->next = me;
		me->prev = listTail;
		me->next = NULL;
		listTail = me;
		listSize++;
	}
	globalModified=true;
}


/*---------------------------------------------------+
	Add a spline into the list.
 +---------------------------------------------------*/
void AddSpline( GLfloat pts[4][4][3] )
{
	mySpline *temp;
	temp = new mySpline;
	temp->prev = temp->next = NULL;
	temp->SetCntrlPts( pts );
	AddSpline( temp );
}

/*---------------------------------------------------+
	Increase the subdivisions for all splines.
 +---------------------------------------------------*/
void IncreaseDivisions( void )
{
	if( pState==EDIT_MODE && current )
		current->IncreaseDivisions();
	else {
		mySpline* temp;
		temp = listHead;
		while( temp!=NULL ){
			temp->IncreaseDivisions();
			temp = temp->next;
		}
	}
	globalModified=true;
}


/*---------------------------------------------------+
	Decrease the subdivisions for all splines.
 +---------------------------------------------------*/
void DecreaseDivisions( void )
{
	if( pState==EDIT_MODE && current )
		current->DecreaseDivisions();
	else {
		mySpline* temp;
		temp = listHead;
		while( temp!=NULL ){
			temp->DecreaseDivisions();
			temp = temp->next;
		}
	}
	globalModified=true;
}


/*---------------------------------------------------+
	Maximize the subdivisions for all splines.
 +---------------------------------------------------*/
void MaxDivisions( void )
{
	if( pState==EDIT_MODE && current )
		current->MaxDivisions();
	else {
		mySpline* temp;
		temp = listHead;
		while( temp!=NULL ){
			temp->MaxDivisions();
			temp = temp->next;
		}
	}
	globalModified=true;
}

/*---------------------------------------------------+
	Minimize the subdivisions for all splines.
 +---------------------------------------------------*/
void MinDivisions( void )
{
	if( pState==EDIT_MODE && current )
		current->MinDivisions();
	else {
		mySpline* temp;
		temp = listHead;
		while( temp!=NULL ){
			temp->MinDivisions();
			temp = temp->next;
		}
	}
	globalModified=true;
}

/*---------------------------------------------------+
	Delete all splines from the list.
 +---------------------------------------------------*/
void DeleteAll( void )
{
	mySpline* temp;
	temp = listHead;
	while( listHead!= NULL ){
		temp = listHead;
		listHead=listHead->next;
		delete temp;
	}
	current=listHead=listTail=NULL;
	globalModified=true;
	listSize=0;
	isControlPoint=0;
}

/*----------------------------------------------------------------------+
	Save
 +----------------------------------------------------------------------*/
void SaveList( char* file )
{
	ofstream outfile;
	outfile.open(file);
	if( outfile.fail() ){
		statusLine="Error opening file, save aborted.";
	} else {
		mySpline* t;
		t = listHead;
		while( t ){
			for( int i=0; i<4; i++){
				for( int j=0; j<4; j++ ){
					outfile <<(GLfloat)t->cntrlPts[i][j][0]<<" "
							<<(GLfloat)t->cntrlPts[i][j][1]<<" "
							<<(GLfloat)t->cntrlPts[i][j][2];
					if( j<3 || t->next!=NULL )
						outfile <<"   ";
				} // for j
				if( i<3 || t->next!=NULL )
					outfile <<"\n";
			} // for i
			t=t->next;
		} // while t
		outfile.close();
		statusLine="Spline data saved.";
	}
	glutPostRedisplay();
}

/*----------------------------------------------------------------------+
	Load the file
 +----------------------------------------------------------------------*/
void LoadList( char* file )
{
	ifstream infile;

	if( listHead!=NULL )
		DeleteAll();

	infile.open( file );

	if( infile.fail() ){
		statusLine="Error opening file, load aborted.";
	} else {
		mySpline* t;
		while( !infile.eof() ){
			t = new mySpline;
			for( int i=0; i<4; i++){
				for( int j=0; j<4; j++ ){
					infile >>t->cntrlPts[i][j][0]
						   >>t->cntrlPts[i][j][1]
						   >>t->cntrlPts[i][j][2];
				} // for j
			} // for i
			AddSpline( t );
		} // while t
		infile.close();
		statusLine="Spline data loaded.";
	}
	SetMode( VIEW_MODE );
	globalModified=true;
	glutPostRedisplay();
}

/*----------------------------------------------------------------------+
	Set all splines as modified
 +----------------------------------------------------------------------*/
void SetModified( void )
{
	globalModified=true;
	mySpline* temp;
	temp=listHead;
	while( temp ){
		temp->SetModified();
		temp=temp->next;
	}
}

/*----------------------------------------------------------------------+
	Make any neccessary changes to the global variables and such
	on a state change
 +----------------------------------------------------------------------*/
void SetMode( int pMode )
{
	switch( pMode ){
		case DELETE_MODE:
			HighLightOn(0, 0);
			highLightSpline=1;
			statusLine = "Click on a spline to delete.  Click on a spline to delete.";
			current=NULL;
			break;
		case VIEW_MODE:
			HighLightOff();
			statusLine = "Entered viewing mode.";
			current=NULL;
			break;
		case EDIT_MODE:
			current = NULL;
			HighLightOn(0,0);
			statusLine = "Entered editing mode.  Click on a spline to edit.";
			break;

		case JOIN_MODE:
			current=NULL;
			pMode = VIEW_MODE;
			statusLine = "Join mode currently unavailable.";
			break;

		case SETC1_MODE:
			current=NULL;
			pMode=VIEW_MODE;
			statusLine = "SetC1 mode currently unavailable.";
			break;

		case ADD_MODE:
			current=NULL;
			HighLightOff();
			xAngle=yAngle=zAngle=0.0;
			centerX=centerY=centerZ=0.0;
			viewAxisMenu( MENU_Y_AXIS );
			statusLine = "Entered add mode.  Click and drag a bounding box for the spline.";
			break;

		case SELECT_MODE:
			break;

		case SAVE_MODE:
			statusLine = "Entered save mode.";
			pMode=VIEW_MODE;
			break;

		case LOAD_MODE:
			pMode=VIEW_MODE;
			statusLine = "Entered load mode.";
			break;

		default:
			statusLine = "Unknown mode, entering view mode.";
			pMode = VIEW_MODE;
			break;
	}; // switch

	globalModified=true;
	pState = pMode;
}

/*---------------------------------------------------+
	Delete the current (if there is one)
 +---------------------------------------------------*/
void DeleteCurrent( void )
{
	if( !current )
		return;

	// change the previous spline's pointer
	if( current->prev )
		current->prev->next = current->next;
	else // then it's the head
		listHead = current->next;

	// change the next spline's pointer
	if( current->next )
		current->next->prev = current->prev;
	else // it's the tail
		listTail = current->prev;

	// delete
	delete current;

	globalModified=true;

	current = NULL;
	listSize--;
	glutPostRedisplay();
}

/*-------------------------------------------------------------+
	Turn the highlight option on.
 +-------------------------------------------------------------*/
inline void HighLightOn( int x, int y )
{
	mySpline* k;
	k=current;

	SetCurrent( x, y );

		mySpline* t;
		t=listHead;
		while( t ){
			t->drawSolid = (t==current);
			t = t->next;
		}

	current=k;
	highLightSpline=1;
	globalModified=true;
	glutPostRedisplay();
}

/*-------------------------------------------------------------+
	Turn the highlight option off.
 +-------------------------------------------------------------*/
inline void HighLightOff( void )
{
	mySpline* t;
	t=listHead;
	while( t ){
		t->drawSolid = true;
		t = t->next;
	}
	highLightSpline=0;
	globalModified=true;
	glutPostRedisplay();
}


/*-------------------------------------------------------------+
	Undo the changes to the moved control point.
 +-------------------------------------------------------------*/
void undoChange( void )
{
	GLfloat t[3], dm[3];
	GLfloat *v;
	if( editedVertex.s ){
		if( editedVertex.entireSpline ){
			t[0] = editedVertex.oldV[0];
			t[1] = editedVertex.oldV[1];
			t[2] = editedVertex.oldV[2];

			v = editedVertex.s->cntrlPts[0][0];
			editedVertex.oldV[0] = v[0];
			editedVertex.oldV[1] = v[1];
			editedVertex.oldV[2] = v[2];

			dm[0] = v[0] - t[0];
			dm[1] = v[1] - t[1];
			dm[2] = v[2] - t[2];

			for( int i=0; i<4; i++ ){
				for( int j=0; j<4; j++ ){
					editedVertex.s->cntrlPts[i][j][0] -= dm[0];
					editedVertex.s->cntrlPts[i][j][1] -= dm[1];
					editedVertex.s->cntrlPts[i][j][2] -= dm[2];
				}
			}

			SetModified();
			globalModified=true;
			statusLine = "Spline movement undone.";
		} else {
			t[0] = editedVertex.oldV[0];
			t[1] = editedVertex.oldV[1];
			t[2] = editedVertex.oldV[2];

			v = editedVertex.s->cntrlPts[ editedVertex.row ][ editedVertex.col ];
			editedVertex.oldV[1] = v[1];
			editedVertex.oldV[0] = v[0];
			editedVertex.oldV[2] = v[2];

			v[0] = t[0];
			v[1] = t[1];
			v[2] = t[2];

			SetModified();
			globalModified=true;
			statusLine = "Control point movement undone.";
		}
	} else {
		statusLine = "Could not undo control point movement.";
	}
} // undoChange()


/*----------------------------------------------------------------------+
	handle keyboard events.
 +----------------------------------------------------------------------*/
void keyboard( unsigned char c, int x, int y )
{
	int holder;
	switch( c ){
		case 27: /* escape */
			statusLine = "Exiting...";
			cleanUp();
			exit(0);
#if !defined( FRAME_RATE_TEST_MODE )

#if defined( USE_TEXTURES )
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			holder = c - '0';
			if( holder>=NUM_TEXTURES )
				break;
			if( pState==EDIT_MODE && current ){
				statusLine = "The texture for the current spline has been changed.";
				current->myTexName = textureNames[holder];
			} else {
				statusLine = "The default texture has been changed.";
				defaultTextureName = textureNames[holder];
			}
			glutPostRedisplay();
			break;
		case 't':
			globalTexturingOn = !globalTexturingOn;
			if( globalTexturingOn )
				statusLine = "Texturing turned on.";
			else
				statusLine = "Texturing turned off.";
			SetModified();
			globalModified=true;
			break;
	#endif // USE_TEXTURES
		case 'l':
			if( globalLightingOn ){
				lightIndex++;
				if( lightIndex==NUM_LIGHTS ){
					globalLightingOn=false;
				} else {
					glLightfv( GL_LIGHT0, GL_AMBIENT, lightZeroAmbient[lightIndex] );
					glLightfv( GL_LIGHT0, GL_DIFFUSE, lightZeroDiffuse[lightIndex] );
				}
			} else {
				globalLightingOn = true;
				glLightfv( GL_LIGHT0, GL_AMBIENT, lightZeroAmbient[ (lightIndex=0) ] );
				glLightfv( GL_LIGHT0, GL_DIFFUSE, lightZeroDiffuse[ lightIndex ] );
			}
			glutPostRedisplay();
			break;
		case 'u':
			undoChange();
			break;
		case 'f': // make resolution finer
			resolutionMenu( MENU_FINER );
			break;
		case 'c':
			resolutionMenu( MENU_COARSER );
			break;
	#if defined(USE_MIDDLE_BUTTON_HACK)
		case 'm': // turn pseudo middle mouse button on/off
			fakeMiddleButton = !fakeMiddleButton;
			if( fakeMiddleButton )
				statusLine = "Fake middle button turned on.";
			else
				statusLine = "Fake middle button turned off.";
			break;
	#endif // USE_MIDDLE_BUTTON_HACK
		case 's':
			SetMode( SETC1_MODE );
			break;
		case 'a':
			SetMode( ADD_MODE );
			break;
		case 'j':
			SetMode( JOIN_MODE );
			break;
		case 'e':
			SetMode( EDIT_MODE );
			break;
		case 'v':
			SetMode( VIEW_MODE );
			break;
		case 'd':
			SetMode( DELETE_MODE );
			break;

		case 'x':
			editPlane = X_ONLY;
			statusLine = "Control point editing set to X-axis only.";
			break;
		case 'y':
			editPlane = Y_ONLY;
			statusLine = "Control point editing set to Y-axis only.";
			break;
		case 'z':
			editPlane = Z_ONLY;
			statusLine = "Control point editing set to Z-axis only.";
			break;

		case 'p':
			if( editPlane == PARALLEL_PLANE ){
				editPlane = XY_PLANE;
				statusLine = "Control point editing set parallel to the Z=0 plane.";
			} else if( editPlane == XY_PLANE ){
				editPlane = XZ_PLANE;
				statusLine = "Control point editing set parallel to the Y=0 plane.";
			} else if( editPlane == XZ_PLANE ){
				editPlane = YZ_PLANE;
				statusLine = "Control point editing is set parallel to the X=0 plane.";
			} else {
				editPlane = PARALLEL_PLANE;
				statusLine = "Control point editing is set parallel to the viewing plane.";
			}
			break;

#endif // !defined( FRAME_RATE_TEST_MODE )
		default:
			break;
	}; // switch

	glutPostRedisplay();
} // keyboard


/*----------------------------------------------------------------------+
	Initialize the program.
 +----------------------------------------------------------------------*/
void init( void )
{



	statusLine = "Welcome to Spline Editor V0.01";

	//AddSpline( controlPoints );
	//AddSpline( controlPoints2 );

	glClearColor( BACK_RED, BACK_GREEN, BACK_BLUE, BACK_ALPHA );

	// Initialize the various lighting
	// Blue Light
	glLightfv( GL_LIGHT0, GL_AMBIENT, lightZeroAmbient[0] );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lightZeroDiffuse[0] );
	glLightfv( GL_LIGHT0, GL_POSITION, lightZeroPosition );
	glEnable( GL_LIGHT0 );

	glLightfv( GL_LIGHT1, GL_AMBIENT, lightRed[0] );
	glLightfv( GL_LIGHT1, GL_DIFFUSE, lightRed[1] );
	glLightfv( GL_LIGHT1, GL_POSITION, lightZeroPosition );

	glLightfv( GL_LIGHT2, GL_AMBIENT, lightYellow[0] );
	glLightfv( GL_LIGHT2, GL_DIFFUSE, lightYellow[1] );
	glLightfv( GL_LIGHT2, GL_POSITION, lightZeroPosition );

	glLightfv( GL_LIGHT3, GL_AMBIENT, lightBlue[0] );
	glLightfv( GL_LIGHT3, GL_DIFFUSE, lightBlue[1] );
	glLightfv( GL_LIGHT3, GL_POSITION, lightZeroPosition );

	glLightfv( GL_LIGHT4, GL_AMBIENT, lightGreen[0] );
	glLightfv( GL_LIGHT4, GL_DIFFUSE, lightGreen[1] );
	glLightfv( GL_LIGHT4, GL_POSITION, lightZeroPosition );

	glEnable( GL_LIGHTING );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
	glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );

	// Init drawing methods
	glEnable( GL_DEPTH_TEST );
	glShadeModel( GL_SMOOTH );

#if defined( USE_TEXTURES )
	InitTextures();

	globalTexturingOn = true;

#endif // USE_TEXTURES
	// Initialize the viewing projection
	glMatrixMode( GL_PROJECTION );
	gluPerspective( 40.0, 1.0, 1.0, 80.0 );

	glMatrixMode( GL_MODELVIEW );


	InitCallbacks();

	InitMenus();



	// Initialize state variables
	SetMode( VIEW_MODE );
	editedVertex.s = NULL;

} // init


/*---------------------------------------------------------------------+
	Initialize all of the callback functions
 +---------------------------------------------------------------------*/
void InitCallbacks( void )
{
	// Initialize the callback functions
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );

#if defined( FRAME_RATE_TEST_MODE )

	glutIdleFunc( frameRateIdle );
	glutTimerFunc( TIME_PERIOD, timerFunc, 0 );
	frameRate=0;
	frameSum=0;

	MaxDivisions();

#else  // !defined FRAME_RATE_TEST_MODE

	glutMouseFunc( mouseClick );
	glutMotionFunc( mouseMotion );

#if defined(USE_HIGHLIGHTED_MENU)
	glutPassiveMotionFunc( passiveMotion );

#endif // USE_HIGHTLIGHTED_MENU
#endif // !defined FRAME_RATE_TEST_M0DE
}

/*---------------------------------------------------------------------+
	Initialize all of the menus
 +---------------------------------------------------------------------*/
void InitMenus( void )
{
	// Create the menus for the program
	int res_menu = glutCreateMenu( resolutionMenu );
	glutAddMenuEntry( menuText[MENU_MIN], MENU_MIN );
	glutAddMenuEntry( menuText[MENU_COARSER], MENU_COARSER );
	glutAddMenuEntry( menuText[MENU_FINER], MENU_FINER );
	glutAddMenuEntry( menuText[MENU_MAX], MENU_MAX );

	int shade_menu = glutCreateMenu( shadeMenu );
	glutAddMenuEntry( menuText[MENU_FLAT], MENU_FLAT );
	glutAddMenuEntry( menuText[MENU_SMOOTH], MENU_SMOOTH );

	int light_menu = glutCreateMenu( lightMenu );
	glutAddMenuEntry( menuText[MENU_CHANGE_LIGHT], MENU_CHANGE_LIGHT );
	glutAddMenuEntry( menuText[MENU_LIGHTS_ON], MENU_LIGHTS_ON );
	glutAddMenuEntry( menuText[MENU_LIGHTS_OFF], MENU_LIGHTS_OFF );


	int rotate_menu = glutCreateMenu( rotationMenu );
	glutAddMenuEntry( menuText[MENU_ROTATE_XY], MENU_ROTATE_XY );
	glutAddMenuEntry( menuText[MENU_ROTATE_XZ], MENU_ROTATE_XZ );
	glutAddMenuEntry( menuText[MENU_ROTATE_YZ], MENU_ROTATE_YZ );

	int back_menu = glutCreateMenu( backgroundMenu );
	glutAddMenuEntry( menuText[MENU_BACK_0], MENU_BACK_0 );
	glutAddMenuEntry( menuText[MENU_BACK_1], MENU_BACK_1 );
	glutAddMenuEntry( menuText[MENU_BACK_2], MENU_BACK_2 );
	glutAddMenuEntry( menuText[MENU_BACK_BLACK], MENU_BACK_BLACK );
	glutAddMenuEntry( menuText[MENU_BACK_WHITE], MENU_BACK_WHITE );

	int viewAxis_menu = glutCreateMenu( viewAxisMenu );
	glutAddMenuEntry( menuText[MENU_Z_AXIS], MENU_Z_AXIS );
	glutAddMenuEntry( menuText[MENU_X_AXIS], MENU_X_AXIS );
	glutAddMenuEntry( menuText[MENU_Y_AXIS], MENU_Y_AXIS );
	glutAddMenuEntry( menuText[MENU_NEG_Z_AXIS], MENU_NEG_Z_AXIS );

	int win_menu = glutCreateMenu( windowMenu );
	glutAddSubMenu( "Background...", back_menu );
	glutAddSubMenu( "Rotations...", rotate_menu );
	glutAddSubMenu( "View from...", viewAxis_menu );
	glutAddMenuEntry( menuText[MENU_RESET], MENU_RESET );


#if defined( USE_TEXTURES )
	int tex2_menu = glutCreateMenu( defaultTexMenu );
	for( int i=0; i<NUM_TEXTURES; i++ ){
		glutAddMenuEntry( textureFiles[i], i );
	}
	int tex_menu = glutCreateMenu( textureMenu );
	glutAddMenuEntry( menuText[MENU_TEXTURE_OFF], MENU_TEXTURE_OFF );
	glutAddMenuEntry( menuText[MENU_TEXTURE_ON], MENU_TEXTURE_ON );
	glutAddSubMenu( "Change texture...", tex2_menu );
#endif //USE_TEXTURES


	int movement_menu = glutCreateMenu( sensitivityMenu );
	glutAddMenuEntry( menuText[MENU_LESS_SENSITIVE], MENU_LESS_SENSITIVE );
	glutAddMenuEntry( menuText[MENU_MORE_SENSITIVE], MENU_MORE_SENSITIVE );
	glutAddMenuEntry( menuText[MENU_DEFAULT_SENSITIVE], MENU_DEFAULT_SENSITIVE );

	int edit_axis = glutCreateMenu( editMenu );
	glutAddSubMenu( "Movement sensitivity...", movement_menu );
	glutAddMenuEntry( menuText[MENU_X_ONLY], MENU_X_ONLY );
	glutAddMenuEntry( menuText[MENU_Y_ONLY], MENU_Y_ONLY );
	glutAddMenuEntry( menuText[MENU_Z_ONLY], MENU_Z_ONLY );
	glutAddMenuEntry( menuText[MENU_XY_PLANE], MENU_XY_PLANE );
	glutAddMenuEntry( menuText[MENU_XZ_PLANE], MENU_XY_PLANE );
	glutAddMenuEntry( menuText[MENU_YZ_PLANE], MENU_YZ_PLANE );
	glutAddMenuEntry( menuText[MENU_PARALLEL_PLANE], MENU_PARALLEL_PLANE );

	int mode_menu = glutCreateMenu( modeMenu );
	glutAddMenuEntry( menuText[MENU_VIEW_MODE], MENU_VIEW_MODE );
	glutAddMenuEntry( menuText[MENU_ADD_MODE], MENU_ADD_MODE );
	glutAddMenuEntry( menuText[MENU_EDIT_MODE], MENU_EDIT_MODE );
	glutAddMenuEntry( menuText[MENU_DELETE_MODE], MENU_DELETE_MODE );
	glutAddMenuEntry( menuText[MENU_JOIN_MODE], MENU_JOIN_MODE );
	glutAddMenuEntry( menuText[MENU_SETC1_MODE], MENU_SETC1_MODE );


	// Create the final top level menu
	glutCreateMenu( mainMenu );
	glutAddSubMenu( "Mode...", mode_menu );
	glutAddSubMenu( "Window...", win_menu );
	glutAddSubMenu( "Edit with respect to...", edit_axis );
	glutAddSubMenu( "Resolution...", res_menu );
	glutAddSubMenu( "Gouraurd Shading...", shade_menu );
	glutAddSubMenu( "Lighting (l)...", light_menu );

#if defined(USE_TEXTURES)
	glutAddSubMenu( "Texturing (t)...", tex_menu );
#endif // USE_TEXTURES
	glutAddMenuEntry( menuText[MENU_SAVE], MENU_SAVE );
	glutAddMenuEntry( menuText[MENU_LOAD], MENU_LOAD );
	glutAddMenuEntry( menuText[MENU_UNDO], MENU_UNDO );
	glutAddMenuEntry( menuText[MENU_DELETE_ALL], MENU_DELETE_ALL );
	glutAddMenuEntry( menuText[MENU_EXIT], MENU_EXIT );

	// Attach the menu to a mouse button
	glutAttachMenu( GLUT_RIGHT_BUTTON );
}


/*----------------------------------------------------------------------+
	Process shading requests
 +----------------------------------------------------------------------*/
void shadeMenu( int value )
{
	switch( value ){
		case MENU_FLAT:
			glShadeModel( GL_FLAT );
			shading = false;
			break;
		case MENU_SMOOTH:
			shading=true;
			glShadeModel( GL_SMOOTH );
			break;
		default:
			break;
	}; // switch
	if( shading )
		statusLine = "Gouraud shading turned on.";
	else
		statusLine = "Gouraud shading turned off.";
	glutPostRedisplay();
} // shadeMenu


/*----------------------------------------------------------------------+
	Process lighting requests
 +----------------------------------------------------------------------*/
void lightMenu( int value )
{
	switch( value ){
		case MENU_LIGHTS_OFF:
			glDisable( GL_LIGHTING );
			globalLightingOn = false;
			break;
		case MENU_LIGHTS_ON:
			glEnable( GL_LIGHTING );
			globalLightingOn = true;
			break;
		case MENU_CHANGE_LIGHT:
			lightIndex++;
			if( lightIndex>=NUM_LIGHTS ){
				lightIndex=0;
			}
			glLightfv( GL_LIGHT0, GL_AMBIENT, lightZeroAmbient[lightIndex] );
			glLightfv( GL_LIGHT0, GL_DIFFUSE, lightZeroDiffuse[lightIndex] );
			break;
		default:
			break;
	}; // switch
	if( globalLightingOn )
		statusLine="Lighting turned on.";
	else
		statusLine="Lighting turned off.";
	//globalModified=true;
	glutPostRedisplay();
} // lightMenu


/*----------------------------------------------------------------------+
	Process resolutions requests
 +----------------------------------------------------------------------*/
void resolutionMenu( int value )
{
	switch( value ){
		case MENU_FINER:
			IncreaseDivisions();
			statusLine = "Resolution increased by factor of 2.";
			break;
		case MENU_COARSER:
			statusLine = "Resolution decreased by factor of 2.";
			DecreaseDivisions();
			break;
		case MENU_MAX:
			statusLine = "Resolution set to maximum.";
			MaxDivisions();
			break;
		case MENU_MIN:
			statusLine = "Resolution set to minimum.";
			MinDivisions();
			break;
		default:
			break;
	}; // switch
	globalModified=true;
	glutPostRedisplay();
} // resolutionMenu

/*----------------------------------------------------------------------+
	Handle the main menu options.
 +----------------------------------------------------------------------*/
void mainMenu( int value )
{
	switch( value ){
		case MENU_EXIT:
			cleanUp();
			exit(0);
			break;
		case MENU_DELETE_ALL:
			DeleteAll();
			globalModified=true;
			break;
		case MENU_UNDO:
			undoChange();
			break;
		case MENU_SAVE:
			glDrawBuffer( GL_FRONT );
			output( winWidth/4, winHeight/2, "Enter filename on the command line." );
			glDrawBuffer( GL_BACK );
			cout <<"Enter filename to save (default: "<<defaultFileName<<") - ";
			cin  >>fileName;
			SaveList( fileName );
			break;
		case MENU_LOAD:
			glDrawBuffer( GL_FRONT );
			output( winWidth/4, winHeight/2, "Enter filename on the command line." );
			glDrawBuffer( GL_BACK );
			cout <<"Enter filename to load (default: "<<defaultFileName<<") - ";
			cin  >>fileName;
			LoadList( fileName );
			break;
		default:
			break;
	}; // switch

	glutPostRedisplay();
} // mainMenu

/*----------------------------------------------------------------------+
	Menu to change the background color
 +----------------------------------------------------------------------*/
void backgroundMenu( int value )
{
	GLfloat r, g, b;
	switch( value ){
		case MENU_BACK_0:
			r = 0.4;
			g = 0.2;
			b = 0.2;
			break;
		case MENU_BACK_1:
			r = 0.2;
			g = 0.2;
			b = 0.4;
			break;
		case MENU_BACK_2:
			r = 0.2;
			g = 0.4;
			b = 0.2;
			break;
		case MENU_BACK_BLACK:
			r = g = b = 0.0;
			break;
		case MENU_BACK_WHITE:
			r = g = b = 1.0;
			break;
	}; // switch
	glClearColor( r, g, b, 1.0 );
	glutPostRedisplay();
}

/*----------------------------------------------------------------------+
	Menu to change modes
 +----------------------------------------------------------------------*/
void modeMenu( int value )
{
	switch( value ){
		case MENU_VIEW_MODE:
			SetMode( VIEW_MODE );
			break;
		case MENU_ADD_MODE:
			SetMode( ADD_MODE );
			break;
		case MENU_EDIT_MODE:
			SetMode( EDIT_MODE );
			break;
		case MENU_DELETE_MODE:
			SetMode( DELETE_MODE );
			break;
		case MENU_JOIN_MODE:
			SetMode( JOIN_MODE );
			break;
		case MENU_SETC1_MODE:
			SetMode( SETC1_MODE );
			break;
	}; // switch
	glutPostRedisplay();
}

/*----------------------------------------------------------------------+
 +----------------------------------------------------------------------*/
void editMenu( int value )
{
	switch( value ){
		case MENU_X_ONLY:
			editPlane = X_ONLY;
			statusLine = "Control point movement is restricted to the X direction.";
			break;
		case MENU_Y_ONLY:
			statusLine = "Control point movement is restricted to the Y direction.";
			editPlane = Y_ONLY;
			break;
		case MENU_Z_ONLY:
			statusLine = "Control point movement is restricted to the Z direction.";
			editPlane = Z_ONLY;
			break;
		case MENU_XY_PLANE:
			statusLine = "Control point movement is restricted to the X/Y directions.";
			editPlane = XY_PLANE;
			break;
		case MENU_XZ_PLANE:
			statusLine = "Control point movement is restricted to the X/Z directions.";
			editPlane = XZ_PLANE;
			break;
		case MENU_YZ_PLANE:
			statusLine = "Control point movement is restricted to the Y/Z directions.";
			editPlane = YZ_PLANE;
			break;
		case MENU_PARALLEL_PLANE:
			statusLine = "Control point movement is restricted to the plane parallel to the viewing plane.";
			editPlane = PARALLEL_PLANE;
			break;
	}; // switch
	glutPostRedisplay();
}


/*----------------------------------------------------------------------+
	How sensitive the control point editing is to mouse
	movements.
 +----------------------------------------------------------------------*/
void sensitivityMenu( int value )
{
	switch( value ){
		case MENU_LESS_SENSITIVE:
			statusLine = "Control point movement sensitivity decreased.";
			editReaction += 10.0;
			break;
		case MENU_MORE_SENSITIVE:
			editReaction -= 10.0;
			if( editReaction<10.0 )
				editReaction = 10.0;
			statusLine = "Control point movement sensitivity increased.";
			break;
		case MENU_DEFAULT_SENSITIVE:
		default:
			editReaction = 25.0;
			statusLine = "Control point movement sensitivity set to default.";
			break;
	};
	glutPostRedisplay();
}

/*----------------------------------------------------------------------+
	Texturing on/off
 +----------------------------------------------------------------------*/
void textureMenu( int value )
{
	switch( value ){
		case MENU_TEXTURE_OFF:
			globalTexturingOn=false;
			//globalModified=true;
			statusLine = "Texturing turned off.";
			break;
		case MENU_TEXTURE_ON:
			globalTexturingOn=true;
			//globalModified=true;
			statusLine = "Texturing turned on.";
			break;
	}; // switch
	glutPostRedisplay();
}

/*----------------------------------------------------------------------+
	Rotations
 +----------------------------------------------------------------------*/
void rotationMenu( int value )
{
	switch( value ){
		case MENU_ROTATE_XY:
			rotateAxis = XY_PLANE;
			break;
		case MENU_ROTATE_XZ:
			rotateAxis = XZ_PLANE;
			break;
		case MENU_ROTATE_YZ:
			rotateAxis = YZ_PLANE;
			break;
	};  // switch
} // rotateMenu

/*----------------------------------------------------------------------+
	Reset the window
 +----------------------------------------------------------------------*/
void windowMenu( int value )
{
	if( value == MENU_RESET ){
		xAngle = yAngle = zAngle = 0.0;
		centerX = centerY = centerZ = zoom = 0.0;
		projections[MAIN_WIN] = Z_AXIS;
		projections[SMALL_1] = X_AXIS;
		projections[SMALL_2] = Y_AXIS;
		projections[SMALL_3] = NEG_Z_AXIS;
//		backgroundMenu( MENU_BACK_0 ); 	// reset the background
	}// if
}

/*---------------------------------------------------------------------+
	The viewing from options
 +---------------------------------------------------------------------*/
void viewAxisMenu( int value )
{
	int win = projections[MAIN_WIN];

	switch( value ){
		case MENU_X_AXIS:
			if( win != X_AXIS ){
				if( projections[SMALL_1]==X_AXIS )
					projections[SMALL_1] = projections[MAIN_WIN];
				else if( projections[SMALL_2]==X_AXIS )
					projections[SMALL_2] = projections[MAIN_WIN];
				else
					projections[SMALL_3] = projections[MAIN_WIN];
				projections[MAIN_WIN] = X_AXIS;
			} // if
			statusLine = "The main window if the view from down the X-axis.";
			break;
		case MENU_Y_AXIS:
			if( win != Y_AXIS ){
				if( projections[SMALL_1]==Y_AXIS )
					projections[SMALL_1] = projections[MAIN_WIN];
				else if( projections[SMALL_2]==Y_AXIS )
					projections[SMALL_2] = projections[MAIN_WIN];
				else
					projections[SMALL_3] = projections[MAIN_WIN];
				projections[MAIN_WIN] = Y_AXIS;
			} // if
			statusLine = "The main window if the view from down the Y-axis.";
			break;
		case MENU_Z_AXIS:
			if( win != Z_AXIS ){
				if( projections[SMALL_1]==Z_AXIS )
					projections[SMALL_1] = projections[MAIN_WIN];
				else if( projections[SMALL_2]==Z_AXIS )
					projections[SMALL_2] = projections[MAIN_WIN];
				else
					projections[SMALL_3] = projections[MAIN_WIN];
				projections[MAIN_WIN] = Z_AXIS;
			} // if
			statusLine = "The main window if the view from down the Z-axis.";
			break;
		case MENU_NEG_Z_AXIS:
			if( win != NEG_Z_AXIS ){
				if( projections[SMALL_1]==NEG_Z_AXIS )
					projections[SMALL_1] = projections[MAIN_WIN];
				else if( projections[SMALL_2]==NEG_Z_AXIS )
					projections[SMALL_2] = projections[MAIN_WIN];
				else
					projections[SMALL_3] = projections[MAIN_WIN];
				projections[MAIN_WIN] = NEG_Z_AXIS;
			} // if
			statusLine = "The main window if the view from down the negative Z-axis.";
			break;
	}; // switch
	glutPostRedisplay();
}

/*----------------------------------------------------------------------+
	defaultTexMenu
 +----------------------------------------------------------------------*/
void defaultTexMenu( int value )
{
	if( value>=NUM_TEXTURES || value<0 )
		return;
	defaultTextureName = textureNames[value];
	statusLine = "Default texture changed.";
	glutPostRedisplay();
}

/*----------------------------------------------------------------------+
	Initialize the textures
 +----------------------------------------------------------------------*/
void InitTextures( void )
{
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

#if defined( USE_IMAGE_TEXTURES )	// Only available on Windows!

	for( int i=0; i<NUM_TEXTURES; i++ ){
		textureNames[i] = LoadGLTexture( (char*) textureFiles[i] );
		if( textureNames[i] ){
			if( defaultTextureName==0 )
				defaultTextureName = textureNames[i];
			cout <<"Loaded '"<<textureFiles[i]<<"' as texture number "<<i<<endl;
		} else
			cout <<"Could not load texture file '"<<textureFiles[i]<<"'"<<endl;
	}

#else // !defined( USE_IMAGE_TEXTURES)
	// Manually compute the textures

	int i, j;
	GLubyte textureData[128][64][4];

	// Create the checkerboard texture --------------------
	unsigned char c;
	for( i=0; i<128; i++ ){
		for( j=0; j<64; j++ ){
			c = ((((i&0x8)==0)^((j&0x8))==0))*(0xaa)+0x55;
			textureData[i][j][0] = (GLubyte) c;
			textureData[i][j][1] = (GLubyte) c;
			textureData[i][j][2] = (GLubyte) c;
			textureData[i][j][3] = (GLubyte) 0xff;
		} // for j
	} // for i

	glGenTextures( 1, &defaultTextureName );
	glBindTexture( GL_TEXTURE_2D, defaultTextureName );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 64, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData );
	textureNames[0] = defaultTextureName;


	// Create the random texture -----------
	srand( 0 );
	c = rand();
	for( i=0; i<128; i++ ){
		for( j=0; j<64; j++ ){
			textureData[i][j][0] = (GLubyte) c | rand();
			textureData[i][j][1] = (GLubyte) c & rand();
			textureData[i][j][2] = (GLubyte) c & rand();
			textureData[i][j][3] = (GLubyte) 0xff;
		} // for j
	} // for i

	glGenTextures( 1, &defaultTextureName );
	glBindTexture( GL_TEXTURE_2D, defaultTextureName );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 64, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData );
	textureNames[1] = defaultTextureName;


	// Add in other computed textures here!!


	defaultTextureName = textureNames[0];
#endif // USE_IMAGE_TEXTURES
}



/*----------------------------------------------------------------------+
	EOF
 +----------------------------------------------------------------------*/

