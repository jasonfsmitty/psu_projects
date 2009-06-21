/*---------------------------------------------------------------------------+
	Jason F Smith
	CSE 486  Project 1

	Functions to calculate a 'clean' path between a start pixel and
	end pixel.  Could be used/modified to actually check if there is
	a path between the two points, but since I already have that code
	in place and it's within 24 hours of the deadline, I'll leave things
	as is.

	All code in this file by Jason F Smith.
 +---------------------------------------------------------------------------*/


#include "cse486p1.h"

bool TestBranch( int x, int y, Position d, long index );
void BranchFail( long del );
void BranchSuccess( void );

long* buffer;
int w;
int h;

/*--------------------------------------------------------------------------+
	Clean up the robot's path.

	If it wasn't the night before this project was due, I'd use this to 
	test if there was a path between the two points.  But for now, I'll
	leave everything as is.
 +--------------------------------------------------------------------------*/
void CleanRobotPath( Position s, Position d )
{

	w = control.imageWidth;
	h = control.imageHeight;

	buffer = new long[w*h];

	if( buffer==NULL ){
		MessageBox( NULL, "Could not allocate enough memory\nfor the CleanPath buffer.", "CleanRobotPath Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	for( int i=0; i<w*h; i++ ){
		buffer[i] = ( control.pathImage[i]!=0 ? 1 : 0 );
	}

	TestBranch( s.x, s.y, d, 2);

	delete buffer;
}

/*

	original pseudo-code for the cleaning algorithm
	( assumes that there is at least one path )

	recursive algorithm

	count surrounding pixels
	if( #==0 )
		branch failed
	else if( #==1 )
		continue branch
	else // #>0
		split branch, result depends on OR of each branch's result

	
	storage - make copy of current path image
	delete pixels (instead of marking them)
	if a branch fails, copy current image into pathImage
	if a branch succeeds, XOR the two images for the final path

*/

bool TestBranch( int x, int y, Position d, long index )
{

	// Delete the pixel
	buffer[y*w+x] = index;

	if( x==d.x && y==d.y ){
		BranchSuccess();
		return true;			// branch succeeded
	}

	// get number of surrounding pixels
	for( int i=(y-1); i<(y+2); i++ ){
		for( int j=(x-1); j<(x+2); j++ ){
			if( i>0 && i<h && j>0 && j<w ){


				if( buffer[i*w+j]==1 ){
					if( TestBranch( j, i, d, index+1) ){
						return true;
					}
				}// if( pixel )


			} // if valid
		}// for j
	}// for i

	BranchFail( index );

	return false;
}

/*------------------------------------------------------------+
	On failure, forget about the branch and update the
	image by getting rid of these pixels.
 +------------------------------------------------------------*/
void BranchFail( long del )
{
	for( int i=0; i<w*h; i++ ){
		if( buffer[i]==del ){
			buffer[i] = 0;
		}
	}
}

/*------------------------------------------------------------+
	On success, we set all of the pixels in the current
	branch to 255.  Also, all pixels not in the path
	are set to 128.  This is used when displaying the image
	so that the unused path is a different color.
 +------------------------------------------------------------*/
void BranchSuccess( void )
{
	for( int i=0; i<w*h; i++ ){
		if( buffer[i]>1 ){
			control.pathImage[i] = 255;
		} else if( control.pathImage[i]!=0 ){
			control.pathImage[i] = 128;
		}
	}	
}
