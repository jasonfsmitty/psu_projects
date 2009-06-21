/*------------------------------------------------------+
	Jason F Smith	4588
	CSE/EE 486

	Just declare the function.
 +------------------------------------------------------*/

#ifndef MEDIAL_AXIS_H
#define MEDIAL_AXIS_H

#include <iostream.h>

void CalculateMedial( 	unsigned char* in,
						unsigned char* out,
						int w,
						int h );


// Set backgroundPixel before calling CalculateMedial
//  This allows for different values to be used as the background pixel
//  All pixels which are not equal to the backgroundPixel will be
//  considered as part of the object.
extern int backgroundPixel;		// The value of the background pixels

#endif
