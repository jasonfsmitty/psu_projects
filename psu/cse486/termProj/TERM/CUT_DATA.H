/*---------------------------------------------------------------------------+
	cut_list.h

	Jason F Smith

	A class to handle tracking of the cut frames' information.
 +---------------------------------------------------------------------------*/

#ifndef CUT_DATA_H
#define CUT_DATA_H

#include "defines.h"

//extern struct MPEG_Data;

// Different methods for the Cut_Data detection methods
enum {
	COLOR_METHOD=1,
	MOTION_METHOD=2,

	BOTH_METHODS= (COLOR_METHOD | MOTION_MENTHOD )
};


struct Cut_Data{
	int frame;
	int color_diff;		// interframe color histogram difference
	int motion_diff;	// interframe motion vector difference.

	int method;

	Cut_Data* n;
	Cut_Data* p;
};



#endif // CUT_DATA_H
