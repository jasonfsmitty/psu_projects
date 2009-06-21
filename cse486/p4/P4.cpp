/*-----------------------------------------------------------------------------+
	Jason F Smith

	CSE 486   Project 4
	April 2001

	Using GLUI V2 in conjunction with GLUT for the GUI.

 +-----------------------------------------------------------------------------*/
#if defined( _WIN32 )	// glut in current directory
#	include "glut.h"
#	pragma comment( lib, "glui32.lib" )
#else // we're in 304 Hammond, which has GLUT installed to the include directory
#include <GL/glut.h>
#endif

#include "glui.h"

#include <fstream.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>


//////////////////////////////////////////////////////////////////////////////
//	Program defines
//////////////////////////////////////////////////////////////////////////////

// Determines if program uses the GLUI interface or not
#define		USE_GLUI

// show debug info from a reshape
//#define		SHOW_DEBUG_RESHAPE

//#define			SHOW_ADJUST_DEBUG_INFO


#define		WINDOW_TITLE		"CSE 486 Project 4 - by JFS"
#define		IMAGE_SIZE		256*256

#define		DEFAULT_WIDTH		500
#define		DEFAULT_HEIGHT		500

#define		NOT_OBJ				-123465.0f

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// for the program states...
enum{
	PARTA_STATE=0,
	PARTB_STATE=1
};

// Enums for the menu options
enum {
	MENU_SEPERATER=0,

	MENU_EXIT
};


// control callback enums
enum {
	CHANGE_STATE = 1,
	POST_REDISPLAY,
	CHANGE_LIGHT,
	
	QUIT
};

/////////////////////////////////////////////////////////
// Several short helper functions
/////////////////////////////////////////////////////////

#define		Abs(A)		( (A)<0.0 ? -(A) : (A) )
#define		Max(A,B)	( (A)>(B) ? (A) : (B) )
#define		Min(A,B)	( (A)>(B) ? (B) : (A) )
#define		SafeRelease(A)		{ if( (A) ) delete (A);  (A)=NULL; }

#ifndef TRUE
#	define	TRUE	1
#	define	FALSE	0
#endif


//////////////////////////////////////////////////////////////////////////////
//	Global/live variables (the live variables are for GLUI)
//////////////////////////////////////////////////////////////////////////////
int 		pState=PARTA_STATE;			// part A or part B?
float 		p0=0.2f;
float		q0=0.4f;
float		r0=1.0f;


char 		outputFile[ sizeof(GLUI_String) ] = "";
float 		sphereRadius=3.0f;
int 		mainWinID=0;
int			bgColor=64;

int			r=128, g=0, b=0;

int 		screenWidth;
int 		screenHeight;

int			viewX, viewY;
int			startX, startY;
int			moving;

float		pLimit=3.0f;
float		qLimit=3.0f;

float			data[256][256];				// the actual resulting data
unsigned char 	texData[ 256*256*3 ];
GLuint 		texID;

char *zoomText[] = { "1x", "2x", "4x", "8x" };
float zoomVal[] = { 0.5f, 2.0f, 4.0f, 8.0f };

// debug variables
int		allowNeg = FALSE;
int		showCon	= FALSE;
int		conStep = 20;
int		showAxis = FALSE;
int		darkSide = TRUE;
int		isDirection =TRUE;

// GLUI global stuff
GLUI			*glui;
GLUI_Panel		*partPanel;
GLUI_Rollout		*lightPanel;
GLUI_RadioGroup 	*partRadio;
GLUI_RadioButton	*partA;
GLUI_RadioButton	*partB;
GLUI_Listbox		*zoom_list;
GLUI_Rollout		*partA_Rollout;
GLUI_Rollout		*partB_Rollout;
GLUI_EditText		*p0_edit;
GLUI_EditText		*q0_edit;
GLUI_EditText		*r0_edit;
GLUI_RadioGroup		*light_group;
GLUI_RadioButton	*l_direction;
GLUI_RadioButton	*l_position;
GLUI_EditText		*p_limit;
GLUI_EditText		*q_limit;
GLUI_EditText		*r_edit;
GLUI_EditText		*g_edit;
GLUI_EditText		*b_edit;

GLUI_Rollout		*debug_Rollout;
GLUI_Checkbox		*neg_check;
GLUI_Checkbox		*con_check;
GLUI_EditText		*con_int;
GLUI_Checkbox		*axis_check;
GLUI_Checkbox		*dark_check;

GLUI_Button		*quitButton;



//////////////////////////////////////////////////////////////////////////////
//	Function Declarations
//////////////////////////////////////////////////////////////////////////////
void Init( void );
void InitMenus( void );
void Keyboard( unsigned char c, int x, int y );
void Reshape( int w, int h );
void Display( void );
void MouseClick( int button, int state, int x, int y );
void MouseMotion( int x, int y );
void CleanUp( void );
void Control_CB( int control );		// for buttons...

// Print text to the screen
void GLPrint( int x, int y, char* text, ... );
void GLPrint( char* text, ... );

void Run_Part_A( void );
void Run_Part_B( void );
void AdjustResult( void );
void DisplayResult( void );
void CreateOrigTex( void );


/*******  Start of function implementations *****************************************/

//////////////////////////////////////////////////////////////////////////////
//	Callback function for using buttons and for all GLUI stuff.
//	Also serves as a central point to perform global tasks such
//	as posting a redisplay or quitting.
//////////////////////////////////////////////////////////////////////////////
void Control_CB( int control )
{
	switch( control )
	{
		case CHANGE_STATE:
			if( pState==PARTA_STATE )
			{
				//partA_Rollout->enable();
				//partB_Rollout->disable();
			} 
			else 
			{
				//partA_Rollout->disable();
				//partB_Rollout->enable();
			}
			// continue on to post a redisplay message...

		case POST_REDISPLAY:
			glutSetWindow( mainWinID );
			glutPostRedisplay();
			break;

		case CHANGE_LIGHT:
			//isDirection = !(light_group->get_int_value());
			isDirection = !isDirection;
			Control_CB( POST_REDISPLAY );
			break;

		case QUIT:
			CleanUp();
			exit(0);

		default:
			break;
	};// switch
}


/////////////////////////////////////////////////////////////////////
//  Print text to the window at (x,y).  Note, can be used like
//  printf() by adding variables ( "%3.2f", myFloat, etc ) right
//  into the string.
/////////////////////////////////////////////////////////////////////
void GLPrint( int x, int y, char* text, ... )
{
	char text2[256];
	va_list ap;

	va_start( ap, text );
		vsprintf( text2, text, ap );
	va_end(ap);

	glRasterPos2i( x, y );
	int len=strlen( text2 );
	for( int i=0; i<len; i++ )
		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, text2[i] );
}

/////////////////////////////////////////////////////////////////////
//  Same as GLPrint above, only this one does not move the cursor
//  position.
/////////////////////////////////////////////////////////////////////
void GLPrint( char* text, ... )
{
	char text2[256];
	va_list ap;

	va_start( ap, text );
		vsprintf( text2, text, ap );
	va_end(ap);

	int len=strlen( text2 );
	for( int i=0; i<len; i++ )
		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, text2[i] );
}


/////////////////////////////////////////////////////////////////////
//  GLUT callback function - this handles resizes to the window.
/////////////////////////////////////////////////////////////////////
void Reshape( int w, int h )
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	screenWidth = w;
	screenHeight = h;

	glViewport( 0, 0, screenWidth, screenHeight );
	gluOrtho2D( 0, screenWidth-1, screenHeight-1, 0 );
}


//////////////////////////////////////////////////////////////////////////////
//	Run the algorithm for part A
//////////////////////////////////////////////////////////////////////////////
void Run_Part_A( void )
{
	float p, q;
	float ps, qs;
	int i, j;

	if( r0==0.0f )
		r0=1.0f;

	// change the light position to 2D homogeneous coordinates...
	ps = p0/r0;
	qs = q0/r0;

	for( i=-128; i<128; i++ )
	{
		for( j=-128; j<128; j++ )
		{
			// compute p and q
			p = ((float)j) * (pLimit/128.0f);
			q = (-(float)i) * (qLimit/128.0f);

			// store the result
			data[i+128][j+128] = ( 1.0f + ps*p + qs*q ) /
						( sqrt(1+p*p+q*q) * sqrt(1+ps*ps+qs*qs) );

			if( data[i+128][j+128]<0.0f && allowNeg==FALSE ){
				if( showCon ){
					data[i+128][j+128] = 1.0f;
				} else {
					data[i+128][j+128] = 0.0f;
				}
			}

		}// for j
	}// for i
}


//////////////////////////////////////////////////////////////////////////////
//	Run the algorithm for part B
//////////////////////////////////////////////////////////////////////////////
void Run_Part_B( void )
{
	float x1, y1;
	float x, y, z;
	float a, b, c;
	float det;

	float pn, qn, rn;	// normal to the surface
	float ps, qs, rs;	// line from point on sphere to the light source

	int i, j;

	for( i=-128; i<128; i++ )
	{
		for( j=-128; j<128; j++ )
		{
			// calculate (x1, y1)
			x1 = ((float)j +0.5f)/127.5f;
			y1 = (0.5f - (float)i)/127.5f;

			a = (x1*x1+y1*y1)/16.0f + 1.0f; //( x1*x1/16.0f + y1*y1/16.0f + 1.0f );
			b = -40.0f;
			c = 391.0f;

			det = ( 1600.0f - 4.0f*a*c);


			if( det<0.0f )
			{
				// not in the sphere
				data[ i+128 ][ j+128 ] = NOT_OBJ;
			}
			else
			{
				// on the shere, compute R(p,q)...

				if( a==0.0f )	// this should never happen, but just in case...
					a = 0.001f;

				z = ( -b - sqrt( det ) ) / (2*a);
				x = z*x1/4;
				y = z*y1/4;

				// now we have the point in 3D space on the sphere that we are looking
				//  at.  Now get the normal at that point!!

				pn = x;
				qn = y;
				rn = z-20.0f;	// this normal is NOT normalized

				if( isDirection ){
					ps=p0;  qs=q0;  rs=r0;
				} else {
					ps = p0 - x;
					qs = q0 - y;
					rs = r0 - z;	// vector from surface point to the light source
				}

				data[ i+128 ][ j+128 ] = ( pn*ps + qn*qs + rn*rs ) /
							( sqrt( rn*rn+pn*pn+qn*qn ) + sqrt( rs*rs+ps*ps+qs*qs ) );
			}

		} // for j
	} // for i
}

//////////////////////////////////////////////////////////////////////////////
//	Scale the pixel values from part A
//////////////////////////////////////////////////////////////////////////////
void AdjustResult_A( void )
{
	float max, min;
	long i, j;
	float shift, factor;

	// calculate the max and min of the data
	for( i=0; i<256; i++ )
	{
		for( j=0; j<256; j++ )
		{
			if( j==0 && i==0 ){
				min=max=data[0][0];
			} else {
				min = Min( min, data[i][j] );
				max = Max( max, data[i][j] );
			}
		} // for j
	} // for i

	shift = -min;
	factor = 255.0f/(max+shift);

#if defined( SHOW_ADJUST_DEBUG_INFO )

	cout <<"Adjust:\n"
		<<"  min=    "<<min<<endl
		<<"  max=    "<<max<<endl
		<<"  shift=  "<<shift<<endl
		<<"  factor= "<<factor<<endl;

#endif

	// shift each value, and then scale by factor to fit in 0-255 range

	for( i=0; i<256; i++ )
	{
		for( j=0; j<256; j++ )
		{
			data[i][j] = (data[i][j]+shift)*factor;
		} // for j
	} // for i
}


//////////////////////////////////////////////////////////////////////////////
//	Adjust the values for part B to fit within the 0-255 range.
//	Pixels that are not part of the sphere, are set to a negative number.
//////////////////////////////////////////////////////////////////////////////
void AdjustResult_B( void )
{
	float max, min;
	long i, j;
	float shift, factor;

	// calculate the max and min of the data
	for( i=0; i<256; i++ )
	{
		for( j=0; j<256; j++ )
		{
			if( j==0 && i==0 ){
				max=data[0][0];
			} else {
				max = Max( max, data[i][j] );
			}
		} // for j
	} // for i

	min = max;
	for( i=0; i<256; i++ )
	{
		for( j=0; j<256; j++ )
		{
			if( data[i][j]<0.0f )
			{
				if( data[i][j]!=NOT_OBJ && darkSide )
				{
					if( allowNeg ){
						min = Min( min, data[i][j] );
					} else {
						min = data[i][j] = 0.0f;
					}
				}
			} else {
				min = Min( min, data[i][j] );
			}
		} // for j
	} // for i

	shift = -min;
	factor = 255.0f/(max+shift);

#if defined( SHOW_ADJUST_DEBUG_INFO )

	cout <<"Adjust:\n"
		<<"  min=    "<<min<<endl
		<<"  max=    "<<max<<endl
		<<"  shift=  "<<shift<<endl
		<<"  factor= "<<factor<<endl;

#endif

	// shift each value, and then scale by factor to fit in 0-255 range

	for( i=0; i<256; i++ )
	{
		for( j=0; j<256; j++ )
		{
			if( data[i][j] == NOT_OBJ )
				data[i][j] = -1.0f;
			else
				data[i][j] = (data[i][j]+shift)*factor;
		} // for j
	} // for i
}


/////////////////////////////////////////////////////////////////////
//  Creates a texture from the original image.  This allows it to
//   be displayed in the window.
/////////////////////////////////////////////////////////////////////
void CreateOrigTex( void )
{
	unsigned char temp[ 256*256*3 ];

	if( texID ){
		glDeleteTextures( 1, &texID );
		texID = 0;
	}

	for( long i=0; i<IMAGE_SIZE; i++ ){
		if( ( ((float*)data)[i]<0.0f) ){
			temp[ i*3   ] = r;
			temp[ i*3+1 ] = g;
			temp[ i*3+2 ] = b;
		} else {
			temp[i*3] = temp[i*3+1] = temp[i*3+2] = 
							((unsigned char)((float*)data)[i]);
		}


		if( showCon )
		{
			if( temp[i*3]%conStep==0 ){
				temp[i*3] = temp[i*3+1] = temp[ i*3+2 ] = 255;
			} else {
				temp[i*3] = temp[i*3+1] = temp[ i*3+2 ] = 0;
			}
		}

		if( showAxis && (i%128)==0 && i%256!=0 )
		{
			temp[i*3] = 255;
			temp[i*3+1] = temp[ i*3+2 ] = 0;
		} 
		else if( showAxis && i/256==128 )
		{
			temp[i*3] = 255;
			temp[i*3+1] = temp[ i*3+2 ] = 0;
		}
	}

	glGenTextures( 1, &texID );
	glBindTexture( GL_TEXTURE_2D, texID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 
		256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, temp );

}


//////////////////////////////////////////////////////////////////////////////
//	Draw a textured quad using the computed image to display
//  the result.
//////////////////////////////////////////////////////////////////////////////
void DisplayResult( void )
{
	float zoom = zoomVal[ zoom_list->get_int_val() ];
	float texStartX = ((float)viewX)/255.0f;
	float texStartY = ((float)viewY)/255.0f;
	
	if( zoom<1.0f )
		zoom = 1.0f;

	float texEndX = texStartX + 1.0f/zoom;
	float texEndY = texStartY + 1.0f/zoom;
	
	if( texEndX>1.0f )
	{
		texStartX -= texEndX-1.0f;
		texEndX = 1.0f;
	}

	if( texEndY>1.0f )
	{
		texStartY -= texEndY-1.0f;
		texEndY = 1.0f;
	}


	glPushMatrix();

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, texID );

	glBegin( GL_TRIANGLE_STRIP );
		glTexCoord2f( texStartX, texStartY );
		glVertex2i( 0, 0 );

		glTexCoord2f( texStartX, texEndY );
		glVertex2i( 0, screenHeight );

		glTexCoord2f( texEndX, texStartY );
		glVertex2i( screenWidth, 0 );

		glTexCoord2f( texEndX, texEndY );
		glVertex2i( screenWidth, screenHeight );
	glEnd();

	glPopMatrix();
}

/////////////////////////////////////////////////////////////////////
//  GLUT callback function - called upon a refresh request.  It draws
//  the three textures and various state information.
/////////////////////////////////////////////////////////////////////
void Display( void )
{
	glClear( GL_COLOR_BUFFER_BIT );
	glEnable( GL_TEXTURE_2D );
	glColor3f( 1.0f, 1.0f, 1.0f );
	
	if( pState==PARTA_STATE ){
		Run_Part_A();
		AdjustResult_A();
	} else {
		Run_Part_B();
		AdjustResult_B();
	}

	CreateOrigTex();
	DisplayResult();

	glutSwapBuffers();
}


/////////////////////////////////////////////////////////////////////
//  GLUT callback - handles mouse clicks
/////////////////////////////////////////////////////////////////////
void MouseClick( int button, int state, int x, int y )
{
	glutSetWindow( mainWinID );

	if( button==GLUT_LEFT_BUTTON ){
		if( state == GLUT_DOWN ){
			moving = TRUE;
			startX = x;
			startY = y;
		} else {
			moving = FALSE;
		}
	}

	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////
//  GLUT callback - handles mouse movements while a mouse button is
//   held down.
/////////////////////////////////////////////////////////////////////
void MouseMotion( int x, int y )
{
	glutSetWindow( mainWinID );

	if( moving )
	{
		viewX -= x-startX;
		viewY -= y-startY;

		if( viewX<0 )		viewX=0;
		if( viewX>255 )		viewX=255;
		if( viewY<0 )		viewY=0;
		if( viewY>255 )		viewY=255;

		startX = x;
		startY = y;
	}

	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////
//  GLUT callback - handles keyboard input
/////////////////////////////////////////////////////////////////////
void Keyboard( unsigned char c, int x, int y )
{
	switch( c )
	{
		case 27:
			CleanUp();
			exit(0);
			break;	// never reached

		default:
			break;
	}
	glutSetWindow( mainWinID );
	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////
//   Init() - Initialize the window, menus, etc.
/////////////////////////////////////////////////////////////////////
void Init( void )
{
	// Setup the window
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowSize( DEFAULT_WIDTH, DEFAULT_HEIGHT );

	mainWinID = glutCreateWindow( WINDOW_TITLE );

	// Add in our event handlers
	glutReshapeFunc( Reshape );
	glutDisplayFunc( Display );
	glutMouseFunc( MouseClick );
	glutMotionFunc( MouseMotion );
	glutKeyboardFunc( Keyboard );

	glClearColor( 0.0f, 0.0f, 1.0f, 0.0f );
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );

#if defined( USE_GLUI )
	
	/*------------------ GLUI STUFF HERE -----------------*/
	
	glui = GLUI_Master.create_glui( "P4 Controls" );
	glui->set_main_gfx_window( mainWinID );

	// add controls and stuff
	glui->add_statictext( "CSE 486 Project 4" );
	glui->add_statictext( " by Jason F Smith" );
	
	glui->add_separator();

	// create a panel to choose which part is displayed (partA/partB)
	partPanel = glui->add_panel( "Display" );
	partRadio = glui->add_radiogroup_to_panel( partPanel, 
								&pState, CHANGE_STATE, Control_CB );

	partA = glui->add_radiobutton_to_group( partRadio, "Reflectance Map (Part A)" );
	partB = glui->add_radiobutton_to_group( partRadio, "Sphere (Part B)" );

	glui->add_separator_to_panel( partPanel );

	zoom_list = glui->add_listbox_to_panel( partPanel, "Zoom", NULL, 
								POST_REDISPLAY, Control_CB );

	for( int w=0; w<4; w++ )
		zoom_list->add_item( w, zoomText[w] );
	zoom_list->set_int_val( 0 );


	lightPanel = glui->add_rollout( "Light Position", true );
	p0_edit = glui->add_edittext_to_panel( lightPanel, "P0",
						GLUI_EDITTEXT_FLOAT,
						&p0,
						POST_REDISPLAY,
						Control_CB );
	q0_edit = glui->add_edittext_to_panel( lightPanel, "Q0",
						GLUI_EDITTEXT_FLOAT,
						&q0, POST_REDISPLAY,
						Control_CB );
	r0_edit = glui->add_edittext_to_panel( lightPanel, "R0",
						GLUI_EDITTEXT_FLOAT,
						&r0, POST_REDISPLAY,
					 	Control_CB );


	glui->add_statictext_to_panel( lightPanel, "Coordinates represent" );
	light_group = glui->add_radiogroup_to_panel( lightPanel, 
							&isDirection, CHANGE_LIGHT, Control_CB );
	l_direction = glui->add_radiobutton_to_group( light_group, "Light direction" );
	l_position = glui->add_radiobutton_to_group( light_group, "Light position" );

	// add a rollout with controls for part A
	partA_Rollout = glui->add_rollout( "Part A Options", false );

	p_limit = glui->add_edittext_to_panel( partA_Rollout, "P Limit",
						GLUI_EDITTEXT_FLOAT,
						&pLimit, POST_REDISPLAY,
						Control_CB );
	q_limit = glui->add_edittext_to_panel( partA_Rollout, "Q Limit",
						GLUI_EDITTEXT_FLOAT,
						&qLimit, POST_REDISPLAY,
						Control_CB );


	partB_Rollout = glui->add_rollout( "Part B Options", false );
	r_edit = glui->add_edittext_to_panel( partB_Rollout, "Bg Red", 
							GLUI_EDITTEXT_INT, &r, POST_REDISPLAY, Control_CB );
	g_edit = glui->add_edittext_to_panel( partB_Rollout, "Bg Green", 
							GLUI_EDITTEXT_INT, &g, POST_REDISPLAY, Control_CB );
	b_edit = glui->add_edittext_to_panel( partB_Rollout, "Bg Blue", 
							GLUI_EDITTEXT_INT, &b, POST_REDISPLAY, Control_CB );

	debug_Rollout = glui->add_rollout( "Debug", true );
	
	neg_check = glui->add_checkbox_to_panel( debug_Rollout,
						 "Allow Negative R(p,q)",
						 &allowNeg, POST_REDISPLAY, Control_CB );

	con_check = glui->add_checkbox_to_panel( debug_Rollout,
						"Show image as contours",
						&showCon, POST_REDISPLAY, Control_CB );

	con_int = glui->add_edittext_to_panel( debug_Rollout,
						"Contour Step", GLUI_EDITTEXT_INT,
						&conStep, POST_REDISPLAY, Control_CB );

	axis_check = glui->add_checkbox_to_panel( debug_Rollout,
						"Show axis", &showAxis, POST_REDISPLAY, Control_CB );

	dark_check = glui->add_checkbox_to_panel( debug_Rollout,
						"Show DarkSide (Part B)", &darkSide,
						POST_REDISPLAY, Control_CB );

	quitButton = glui->add_button( "Exit", QUIT, Control_CB );


	// update any display stuff that changes when the state does 
	Control_CB( CHANGE_STATE );
#endif

}


/////////////////////////////////////////////////////////////////////
//  Cleanup the textures (or anything else)
/////////////////////////////////////////////////////////////////////
void CleanUp( void )
{
	if( texID )
		glDeleteTextures( 1, &texID );

#if defined( USE_GLUI )
	GLUI_Master.close_all();
#endif

}


/////////////////////////////////////////////////////////////////////
//   main() - where everything starts
////////////////////////////////////////////////////////////////////
int main( int argc, char **argv )
{
	glutInit( &argc, argv );
	
	// Do our initialization stuff, window, menus, etc.
	Init();

	// Enter the glut main loop, never returns from this!
	glutMainLoop();

	return 0; // never reached!!
}


/*******/
/* EOF */
/*******/
