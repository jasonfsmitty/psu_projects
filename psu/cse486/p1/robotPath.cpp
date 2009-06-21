/*----------------------------------------------------------+
	Jason F Smith
	CSE 486		Project 1

	Compute the robot's path from the medial axis data.
	Not very efficent.

	All code here by JFS.
 +----------------------------------------------------------*/

#include "cse486p1.h"


Position* FindClosest( Position s );
void CalculateConnected( void );
void ChangeOver( int x, int y, unsigned char from, unsigned char to );
bool CheckValues( Position start, Position end );


/*--------------------------------------------------------------------------+
	Calculate the robot's path.  Should be replaced by a variation of the
	CleanRobotPath algorithm.
 +--------------------------------------------------------------------------*/
bool CalcRobotPath( void )
{
	Position start;
	Position end;
	Position *temp;
	unsigned char c;
	int i;
	bool status;

	// get the start/end (x,y)
	start.x = control.startX;
	start.y = control.startY;
	end.x = control.endX;
	end.y = control.endY;


	if( control.pathImage ){
		delete control.pathImage;
		control.pathImage=NULL;
	}

	if( !CheckValues( start, end ) )
		return false;


	control.pathImage = new unsigned char[ control.imageWidth*control.imageHeight ];
	if( control.pathImage == NULL ){
		MessageBox( NULL, "Could not allocate enough memory for the pathImage.", "CalcRobotPath() Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	for( i=0; i<256*256; i++ ){
		control.pathImage[i] = 0;
	}
	// Check that the values are valid?
	//  - now now, to be added later

	// find the closest medial axis to that position
	temp = FindClosest( start);
	if( temp==NULL ){
		MessageBox( NULL, "Could not associate the starting\nposition with a medial axis.", "Calculate Robot Path Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}
	start.x = temp->x;
	start.y = temp->y;

	temp = FindClosest( end );
	if( temp==NULL ){
		MessageBox( NULL, "Could not associate the ending\nposition with a medial axis.", "Calculate Robot Path Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}
	end.y = temp->y;
	end.x = temp->x;


	//Calculate the connected objects
	CalculateConnected();


	// check if the medial axis at start/end have
	//  the same object value
	unsigned char val1 = control.pathImage[ start.y*control.imageWidth + start.x];
	unsigned char val2 = control.pathImage[ end.y*control.imageWidth + end.x];

	//if equal, then there are the same object
	if( val1==val2 && val1!=0 ){
		c=128;
		status=true;
	} else {
		c=0;
		status=false;
	}

/*
	if( val1 == 0 ){
		MessageBox( NULL, "Found val1 to be zero", "Debug", 0 );
	}
	if( val2==0 ){
		MessageBox( NULL, "Found val2 to be zero.", "Debug", 0 );
	}
*/

	// set all the values of the object to 128, the default value
	for( i=0; i<control.imageHeight*control.imageWidth; i++ ){
		control.pathImage[i] = ( control.pathImage[i]==val1 ? c : 0 );
	}


	CleanRobotPath( start, end );

	return status;
}

/*--------------------------------------------------------------------------+
	Given a position s, this function tries to locate the closest medial
	axis pixel.  Note that it doesn't do a perfect job.  It returns on
	the first pixel found, but that pixel could possibly not be connected
	through the medial axis to the opposing start/end pixel.
 +--------------------------------------------------------------------------*/
Position* FindClosest( Position s )
{
	static Position p;

	int d;

	d=0;
	p.x=0;
	p.y=0;

	for( d=0; d<control.imageHeight; d++ ){
		for( p.y =(s.y-d); p.y<=(s.y+d); p.y++ ){
			for( p.x=(s.x-d); p.x<=(s.x+d); p.x++ ){
				
				if( p.x>0 && p.x<control.imageWidth &&
					p.y>0 && p.y<control.imageHeight ){
					// if (p.x, p.y) is on the medial axis, then return (p.x, p.y)
					if( control.medialImage[ p.y*control.imageWidth+p.x ] != 0 &&
						//control.medialImage[ p.y*control.imageWidth+p.x ] >= control.robotSize &&
						d < control.robotSize ){
						// medial axis found
						return &p;
					}
				} // if inside image

			}// for p.x
		}// for p.y
	}// for d

	return NULL;
}


/*--------------------------------------------------------------------------+
	Calculates all of the connected objects using a modified
	sequential algorithm.  The algorithm does not store a table
	of identical object values.  Instead, when two different values
	are found to be of the same object, the program immediately makes
	the replacements by calling ChangeOver().
 +--------------------------------------------------------------------------*/
void CalculateConnected( void )
{
// some defines to make things easier to type
#define		s		control.medialImage
#define		d		control.pathImage
#define		w		control.imageWidth
#define		h		control.imageHeight

	unsigned char index=0, u, l, ul, ur, c;
	int i, j;


	for( i=0; i<control.imageHeight; i++ ){

		// scan each row and look for a pixel
		for( j=0; j<control.imageWidth; j++ ){

			if( s[i*w+j] != 0 ){ // then pixel found

				u = ( i>0 				?	d[(i-1)*w+j]	: 0 );
				l = ( j>0				?	d[i*w+j-1]		: 0 );
				ul = ( i>0	&& j>0		?	d[(i-1)*w+j-1]	: 0 );
				ur = ( i>0	&& j+1<w	?	d[(i-1)*w+j+1]	: 0 );

				if( u==0 && l==0 && ul==0 && ur==0 ){
					// this gets a new index value
					d[i*w+j] = ++index;
				} else {
					// it is connectd to something else
					// get that something else
					if( l!=0 )
						c=l;
					else if( ul!=0 )
						c=ul;
					else if( u!=0 )
						c=u;
					else if( ur!=0 )
						c=ur;
					else 
						MessageBox( NULL, "Error in CalculateConnected.","DEBUG", 0 );


					d[i*w+j]=c;
					
					// Set all of the pixels to the same value
					if( l!=0 && l!=c ){
						// should be an error, but just process it for now
						ChangeOver( j, i, l, c );
					}

					if( ul!=0 && ul!=c )
						ChangeOver( j, i, ul, c );
					if( u!=0  && u!=c )
						ChangeOver( j, i, u, c );
					if( ur!=0 && ur!=c )
						ChangeOver( j, i, ur, c );
				}// if-else

			}// if(pixelfound)
		}// for j
	} // for i

}


/*--------------------------------------------------------------------------+
	Goes through the pathImage and replaces all 'from' values to the
	'to' ones.  Used by the CalculateConnected function.
 +--------------------------------------------------------------------------*/
void ChangeOver( int x, int y, unsigned char from, unsigned char to )
{
	if( from==0 || to==0 ){
		MessageBox( NULL, "Found a zero within ChangeOver.", "Debug Info", MB_OK|MB_ICONEXCLAMATION );
	}


	for( int i=0; i<y*control.imageWidth+x; i++ ){
		if( control.pathImage[i]==from ){
			control.pathImage[i] = to;
		} // if
	}// for
}



/*--------------------------------------------------------------------------+
	Just a function to check all of the values for the start/end
	coordinates and the robot size.
 +--------------------------------------------------------------------------*/
bool CheckValues( Position start, Position end )
{

		//Check the start/end coordinates
	if( (start.x < 0) || (start.x >= control.imageWidth) ){
		MessageBox( NULL, "Starting X coordinate is outside of current image.", "Calculate Robot Path Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}
	if( (start.y < 0) || (start.y >= control.imageHeight) ){
		MessageBox( NULL, "Starting Y coordinate is outside of the current image.", "Calculate Robot Path Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if( (end.x < 0) || (end.x >= control.imageWidth) ){
		MessageBox( NULL, "Ending X coordinate is outside of current image.", "Calculate Robot Path Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}
	if( (end.y < 0) || (end.y >= control.imageHeight) ){
		MessageBox( NULL, "Ending Y coordinate is outside of the current image.", "Calculate Robot Path Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if( control.origImage[ start.y*control.imageWidth+start.x ] == 0 ){
		MessageBox( NULL, "Starting coordinates are not within a hallway.", "Calculate Robot Path Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if( control.origImage[ (end.y)*(control.imageWidth)+(end.x) ] == 0 ){
		MessageBox( NULL, "Ending coordinates are not within a hallway.", "Calculate Robot Path Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	return true;
}




