/*----------------------------------------------------------------------------+
	BasicShell.cpp

	The bare bones needed to get started on a Win32 OpenGL program.

	This file is to be distributed along with BasicGL.h and BasicGL.cpp

	All code originally from the tutorials on http://nehe.gamedev.net

	Files compiled by Jason F Smith (jfs155@psu.edu).

 +----------------------------------------------------------------------------*/

#include <windows.h>		// Header File For Windows
#include <stdio.h>			// Header File For Standard Input/Output
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include "BasicGL.h"

/*--- Other includes here  ---*/



/*------- Several pre-processor defines  ---*/
#define		ASK_FOR_FULLSCREEN
//#define		DEBUG_MESSAGE_BOXES

/*------- Values for several defines/constants  ------*/
#include "BasicDefaults.h"

/*------------  Global Data  -------------*/
bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default

// Light0 Data


// Mat properties


/*---  Other global data here  ---*/


/*------------  A list of functions implemented in this file (by default)  -----------*/
// WndProc(...)		// Prototyped in BasicGL.h
// WinMain(...)		// No need for prototype
void	CleanUp( void );		// Called when exiting
bool	Init( void );			// Called when initializing program
bool	DrawGLScene( void );



/***************** Functions are implemented from here down   ***********************/

/*----  Initialization function  -------*/
bool Init( void )
{
	// Several default GL states...
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup

	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	// For OpenGL blending
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);					// Set The Blending Function For Translucency
	glEnable(GL_BLEND);


	// Initialize lighting

	// Initialize material properties

	// Initialize textures

	// Initialize program data (classes, structs, global variables, etc)

	return true;	// if successful
}


void CleanUp( void )
{

	// Restore memory, save config, scores, etc.

}

bool DrawGLScene( void )		// Here's Where We Do All The Drawing
{

	return true;
}


/*----  Default Window processing function.  ----*/
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch( uMsg )									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if( !HIWORD(wParam) ){					// Check Minimization State
				active=TRUE;						// Program Is Active
			} else {
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
#if defined( DEBUG_MESSAGE_BOXES )
			MessageBox( NULL, "Recieved WM_QUIT message in WndProc().", "Quitting...", 0 );
#endif /* defined( DEBUG_MESSAGE_BOXES ) */
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);

} // WinProc





int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

#if defined( ASK_FOR_FULLSCREEN )
	// Ask The User Which Screen Mode They Prefer
	if( MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO )
	{
		fullscreen=FALSE;							// Windowed Mode
	}
#else /* set default fullscreen value */
	fullscreen=DEFAULT_FULLSCREEN;
#endif /* defined(ASK_FOR_FULLSCREEN */

	// Create Our OpenGL Window
	if (!CreateGLWindow(WINDOW_TITLE, DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_BPP, fullscreen))
	{
#if defined( DEBUG_MESSAGE_BOXES )
		MessageBox( NULL, "Could not create the initial window.", "CreateGLWindow Error", 0 );
#endif /* defined( DEBUG_MESSAGE_BOXES ) */
		return 0;									// Quit If Window Was Not Created
	} // if(!CreateGLWindow

	if( !Init() ){
#if defined( DEBUG_MESSAGE_BOXES )
		MessageBox( NULL, "Could not initialize program with Init().", "Error Initializing", 0 ):
#endif /* defined( DEBUG_MESSAGE_BOXES ) */
		return 0;
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			} 
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		} else {										// If There Are No Messages

			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if( (active && !DrawGLScene()) || keys[VK_ESCAPE] ) {  // Active?  Was There A Quit Received?
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			} else {
				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
				
				// Can set specific key functionality here!!
				// Example, F1 key switches between fullscreen/windowed
				if( keys[VK_F1] ){						// Is F1 Being Pressed?
					keys[VK_F1]=FALSE;					// If So Make Key FALSE
					KillGLWindow();						// Kill Our Current Window
					fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
					// Recreate Our OpenGL Window
					if( !CreateGLWindow(WINDOW_TITLE, screenWidth, screenHeight, fullscreen) ){
#if defined( DEBUG_MESSAGE_BOXES )
						MessageBox( NULL, "Could not switch to/from fullscreen/windowed mode.", "CreateGLWindow Error", 0 );
#endif /* defined( DEBUG_MESSAGE_BOXES ) */
						return 0;						// Quit If Window Was Not Created
					} // if(!CreateWindow...)
				}// if( F1 pressed )
			}// if( time to quit ) else
		} // if(peekmessage)-else
	} // while(!done)

	// Shutdown
	KillGLWindow();									// Kill The Window

	/*--- Cleanup occurs here ---*/
	CleanUp();

	return (msg.wParam);							// Exit The Program
} // WinMain