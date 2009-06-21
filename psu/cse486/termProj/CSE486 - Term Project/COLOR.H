/*-----------------------------------------------------------------------+
	color.h

	Jason F Smith

	CSE 486 Semester Project

	Declarations of methods and data types for performing the
	color histogram based shot detection.
 +-----------------------------------------------------------------------*/

#ifndef COLOR_SHOT_CHANGE_H
#define COLOR_SHOT_CHANGE_H

#if defined( _WIN32 )
#	include "glut.h"
#else
#	include <GL/glut.h>
#endif


struct RGB_Pixel{
	unsigned char red, green, blue;
};

#define LUVDataType 	float

struct LUV_Pixel{
	LUVDataType l, u, v;
};


void ConvertToLUV( RGB_Pixel &rgb, LUV_Pixel &luv );

bool CompareFrames_Color( ?? );

#endif
