/*
		BasicDefaults.h

		by Jason F Smith

	Contains the default values used within BasicShell.cpp	

	By declaring the pre-processor define: INCLUDE_GL_DEFAULTS, 
	all of the global GL defaults will be included.

*/

#ifndef BASIC_DEFAULTS_H
#define	BASIC_DEFAULTS_H


#include <GL\gl.h>		// OpenGL

// Window properties
#define		WINDOW_TITLE		"BasicShell Example"
#define		DEFAULT_WIDTH		640
#define		DEFAULT_HEIGHT		480
#define		DEFAULT_BPP			16
#define		DEFAULT_FULLSCREEN	true


/*---------------------------------------------------+
	All of the light and material properties are
	set to the default as detailed in the 
	OpenGL Programming Guide, Chapter 5
 +---------------------------------------------------*/
#if defined( INCLUDE_GL_DEFAULTS )

/*--- Defines which sets of variables are included  ---*/
#define		INCLUDE_DEFAULT_GL_LIGHT_VARS
#define		INCLUDE_DEFAULT_GL_MAT_VARS


#if defined( INCLUDE_DEFAULT_GL_LIGHT_VARS )

// Default light properties
GLfloat defaultGLLightPosition[] = {0.0, 0.0, 1.0, 1.0};
GLfloat defaultGLLightColor[] = {1.0, 1.0, 1.0, 1.0};
GLfloat defaultGLLightAmbient[] = {0.0, 0.0, 0.0, 1.0};
GLfloat defaultGLLightDiffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat defaultGLLightSpecular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat defaultGLLightSpotDirection[] = { 0.0, 0.0, -1.0 };
GLfloat defaultGLLightSpotExponent = 0.0;
GLfloat defaultGLLightSpotCutoff = 180.0;
GLfloat defaultGLLightConstantAttenuation = 1.0;
GLfloat defaultGLLightLiearAttenuation = 0.0;
GLfloat defaultGLLightQuadAttenuation = 0.0;

#endif /* defined( INCLUDE_DEFAULT_GL_LIGHT_VARS ) */
#if defined( INCLUDE_DEFAULT_GL_MAT_VARS )

// Default material properties
GLfloat defaultGLMatAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat defaultGLMatDiffuse[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat defaultGLMatSpecular[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat defaultGLMatShininess[] = { 0.0 };
GLfloat defaultGLMatEmmission[] = { 0.0, 0.0, 0.0, 1.0 };
GLint   defaultGLMatColorIndexes[] = { 0, 1, 1 };

#endif /* defined( INCLUDE_DEFAULT_GL_MAT_VARS ) */

#endif /* defined( INCLUDE_GL_DEFAULTS ) */

#endif /* defined( BASIC_DEFAULTS_H ) */