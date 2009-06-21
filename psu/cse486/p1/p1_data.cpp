/*-------------------------------------------------------------------------+
	Jason F Smith

	CSE 486 Project 1

	Actual declarations of global data.  No implementation code
	in this file.
	
 +------------------------------------------------------------------------*/
#include <windows.h>
#include <GL\gl.h>
#include "cse486p1.h"


// Windowing stuff

HDC			hDC;			// Private GDI Device Context
HGLRC		hRC;			// Permanent Rendering Context
HWND		hWnd;			// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application
HINSTANCE	dInstance;
HWND		dialogHWND;

int			screenWidth=0;
int			screenHeight=0;
bool		fullscreen=false;
bool		glWindowCreated=false;

bool		keys[256];			// not used
bool		active=true;		// Window Active Flag Set To TRUE By Default


ImagingControls control;

GLuint		texName;