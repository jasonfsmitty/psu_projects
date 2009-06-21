/*--------------------------------------------------------------------+

	Jason F Smith

	CSE 486 - Project 1

	cse486p1.h - main project header file.

 +--------------------------------------------------------------------*/

/*--- Include files ---*/
//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>

#include <GL\gl.h>
#include <GL\glu.h>
#include <GL\glaux.h>


/*--- Project header files ---*/
#include "resource.h"
#include "medialAxis.h"		// computes the medial axis
#include "robotPath.h"
#include "cleanPath.h"

/* global variable prototypes */
extern HDC			hDC;			// Private GDI Device Context
extern HGLRC		hRC;			// Permanent Rendering Context
extern HWND			hWnd;			// Holds Our Window Handle
extern HWND			dialogHWND;
extern HINSTANCE	hInstance;		// Holds The Instance Of The Application
extern HINSTANCE	dInstance;

extern int			screenWidth;
extern int			screenHeight;
extern bool			fullscreen;
extern bool			keys[256];
extern bool			active;		// Window Active Flag Set To TRUE By Default
extern bool			glWindowCreated;
extern GLuint		texName;


// structure to hold image information along with the actual image
struct ImagingControls {
	int imageWidth;
	int imageHeight;
	int imageForeground;

	int robotSize;
	int startX;
	int startY;
	int endX;
	int endY;

	unsigned char* origImage;
	unsigned char* medialImage;
	unsigned char* overlayImage;
	unsigned char* pathImage;

	unsigned char* currentImage;
	int currentType;

	char imagePath[MAX_PATH];
};

extern ImagingControls control;


#define		Min(A,B)		( (A)<(B) ? (A) : (B) )
#define		Max(A,B)		( (A)<(B) ? (B) : (A) )


struct Position{
	int x;
	int y;
};


// various values for type in DisplayWindow(type)
enum
{
	ORIGINAL_IMAGE=1,
	MEDIAL_IMAGE=2,
	OVERLAY_IMAGE=3,
	PATH_IMAGE=4
};


/********************** Function Prototypes **************************/

bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag);
GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
GLvoid KillGLWindow(GLvoid);
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam);			// Additional Message Information
int CALLBACK DialogProc(HWND h,UINT m,WPARAM w,LPARAM l);
bool InitGL( void );
void KillGL( void );
void DrawGLScene( void );
void CleanUp( void );

int StringToInt( char* str );
void DisplayWindow( int type );
bool SaveImage( char* file, unsigned char *image, int length );
bool LoadImage( void );

// functions to initialize the data for each type of image
unsigned char* InitOriginal( void );
unsigned char* InitMedial( void );
unsigned char* InitOverlay( void );
unsigned char* InitPath( void );
void ProcessData( void );
void ProcessRobot( void );
