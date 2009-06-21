/*------------------------------------------------------------------------+

	BasicGL.h

	Library for several key OpenGL processes in Win32.

	Code was taken from the tutorials at
		NeHe Productions (http://nehe.gamedev.net)
		HeHe is run by Jeff Molofee.
	
	Compiled by Jason F Smith.

	Jason -
		I don't really care who uses this or for what purpose.  The 
		credit really goes to NeHe, after all, it's his code.  I just
		stuck a bunch of it into a couple of files so that I could
		develop programs faster and focus on the OpenGL code.

 +------------------------------------------------------------------------*/
#include <windows.h>		// Header File For Windows
#include <stdio.h>			// Header File For Standard Input/Output
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library


/*---  Global data to be defined in BasicGL.cpp file  ----*/
extern	HDC			hDC;			// Private GDI Device Context
extern	HGLRC		hRC;			// Permanent Rendering Context
extern	HWND		hWnd;			// Holds Our Window Handle
extern	HINSTANCE	hInstance;		// Holds The Instance Of The Application

/*---  Global data to inform user of various states. Do not modify directly!!  ---*/
extern	int			screenWidth;
extern	int			screenHeight;
extern  bool		fullscreen;


/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
extern	bool		CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag);

/*---  Properly kill the window  ---*/
extern	void		KillGLWindow(GLvoid);

/*---  Resize and initialize the GL window  ---*/
extern	void		ReSizeGLScene(GLsizei width, GLsizei height);

/*---  Load a BMP from a file.  ---*/
extern AUX_RGBImageRec *LoadBMP(char *Filename);

/*	Completely loads and initializes a texture from a BMP file.
	Returns: true->success,  false->failure
	textureID is the ID for the Texture Object that the
	  texture was loaded into.*/
extern bool			LoadTexture( char* textureFile, GLuint &textureID );

/*---  Sets the texture object refered to by textureID to the current texture ---*/
#define				SetTexture(T)			glBindTexture(GL_TEXTURE_2D,T)

/*---  Delete the texture object  ---*/
#define				DeleteTexture(T)		glDeleteTextures(1,&T)


extern	bool		InitGL( void );

/*---  Just your basic windows callback function.  Must be implemented by the user!!  ---*/
extern	LRESULT		CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
