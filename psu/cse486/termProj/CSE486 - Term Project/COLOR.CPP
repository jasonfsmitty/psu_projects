/*----------------------------------------------------------------------------+
	color.cpp

	Jason F Smith

	CSE 486 Semester project - This file contains functions to perform
	the color histogram based shot change detection.

 +----------------------------------------------------------------------------*/

#if defined( _WIN32 )
#	include "glut.h"
#else
#	include <GL/glut.h>
#endif

#include <math.h>
#include "color.h"

const LUVDataType x0 = 250.155
const LUVDataType y0 = 255.0;
const LUVDataType z0 = 301.41;
const LUVDataType u0 = ( 4.0*x0 ) / ( x0 + 15.0*y0 + 3.0*z0 ); // ~= 0.0010448
const LUVDataType v0 = ( 9.0*y0 ) / ( x0 + 15.0*y0 + 3.0*z0 );// ~= 0.0023962 


//////////////////////////////////////////////////////////////////////////////
//	Convert from RGB color space to LUV.
//	This will probably need some optimization for things to run as
//	quick as possible.
//
//	TODO: Profile the program with VC++ to see if this really is a
//		bottleneck before optimizing.
//////////////////////////////////////////////////////////////////////////////
void ConvertToLUV( RGB_Pixel &rgb, LUV_Pixel &luv )
{
	LUVDataType x, y, z;
	LUVDataType u1, v1;

	x = 0.607*(float)rgb.r + 0.174*(float)rgb.g + 0.200*(float)rgb.b;
	y = 0.299*(float)rgb.r + 0.587*(float)rgb.g + 0.114*(float)rgb.b;
	z = /* 0.000*(float)rgb.r + */ 0.066*(float)rgb.g + 1.116*(float)rgb.b;

	float fraction = ((float)y) / ((float)y0);
	if( fraction >= 0.008856 ){
		luv.l = 25.0 * pow( (100.0*fraction), (1.0/3.0) ) - 16.0;
	} else {
		luv.l = 903.3 * fraction;
	}

	u1 = ( 4.0*x ) / ( x + 15.0*y + 3.0*z );
	v1 = ( 9.0*y ) / ( x + 15.0*y + 3.0*z );

	luv.u = 13.0 * luv.l * ( u1 - u0 );
	luv.v = 13.0 * luv.l * ( v1 - v0 );
}


//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
