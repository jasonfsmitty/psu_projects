/*------------------------------------------------------------------------+

		defines.h

		Jason F Smith

		Defines for the project.

 +------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////
//	Turn debuging mode on/off
///////////////////////////////////////////////////////////////////////////////
#if !defined( DEBUG )

// Comment out the following line to turn debug on/off

#	define		DEBUG

#	if defined( DEBUG )
#		pragma message ("INFO - Global debugging: ON")
#	else
#		pragam message ("INFO - Global debugging: OFF")
#	endif
#endif	// DEBUG not already defined

// Run through all of the includes and static library linking...

// Default header files...
#	include <iostream.h>
#	include <fstream.h>

// Toolkit to make control box stuff
#	include "glui.h"
#if defined( _WIN32 )
#	include "glut.h"
#else
#	include <GL/glut.h>
#endif

/////////////////////////////////////////////////////////////////////////
// Link to GLUI library and include GLUT
#	if defined( _WIN32 )	// at home under MSVC++ 6.0

#		pragma comment( lib, "glui32.lib" )

#	else	// in 304 Hammond...

//#		pragma comment( lib, "libmpeg.a" )	// not available for Win32!!
//#		pragma comment( lib, "libglui.a" )	// linked for Win32 above

#	endif


///////////////////////////////////////////////////////////////////////////////
//	Several global macros...
///////////////////////////////////////////////////////////////////////////////

#ifndef		Max
#	define			Max(A,B)		((A)>=(B)?(A):(B))
#	define			Min(A,B)		((A)>=(B)?(B):(A))
#	define			Sign(A)			((A)>=0.0?(1):(-1))
#	define			Abs(A)			((A)>=0.0?(A):-(A))
#endif // ifndef  Max

#ifndef		SafeRelease
#	define		SafeRelease(A)	if(A){ delete (A); (A)=NULL; }
#endif

#ifndef TRUE
#	define	TRUE	1
#	define	FALSE	0
#endif

#define		MAX_FILENAME		128
#define		MAX_LINE		128 



enum {
	RUN_DETECTION=1,
	PREVIEW_MOVIE,
	SET_PRESET,

	QUIT
};


// Various presets....
enum {
	SPEED_MODE=1,
	DEBUG_MODE
};

// various modes that can be set with EnterMode()
enum {
	OPTIONS_MODE=1,
	PREVIEW_MODE,
	CALC_MODE,
	RESULT_MODE
};


enum {
	COLOR_METHOD=0,
	MOTION_METHOD=1,
	BOTH_METHODS=2
};


///////////////////////////////////////////////////////////////////////////////
//	Function declarations....
///////////////////////////////////////////////////////////////////////////////
void MPEG_Display( void );
void MPEG_Reshape( int, int );
void MPEG_Keyboard( unsigned char, int, int );
void HIST_Display( void );
void HIST_Reshape( int, int );
void HIST_Keyboard( unsigned char, int, int );

void Control_CB( int value );
void Init( void );
void Init_GLUI( void );
void CleanUp( void );

void SetOptions( int );
void EnterMode( int );
void CalcIdle( void );
void SpeedIdle( void );
void PreviewIdle( void );
void ShowResults( void );
void RunMotion( void );
void RunColor( void );

void RunFilter( void );
