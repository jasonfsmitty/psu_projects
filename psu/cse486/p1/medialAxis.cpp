/*---------------------------------------------------------------------------+
	Jason F Smith   4588
	CSE/EE 486

	My implementation of calculating the medial axis.  All code
	by JFS.
 +---------------------------------------------------------------------------*/
#include "medialAxis.h"


// local functions used to calculate the medial axis
void CalcAxis( unsigned char* in, unsigned char* out, int w, int h );
void RecursiveDistance( unsigned char* data, int w, int h );
void ThinData( unsigned char* in, int w, int h );
int backgroundPixel=0;


/*------------------------------------------------+
	CalculateMedial
	Calculates the medial axis for the image.
 +------------------------------------------------*/
void CalculateMedial( unsigned char* in, unsigned char* out, int w, int h )
{
	int i;
	unsigned char *temp;

	temp = new unsigned char[w*h];

	// initialize the output and temp
	for( i=0; i<w*h; i++ ){
		temp[i] = ( in[i]==backgroundPixel ? 0 : 1 );
		out[i] = 0;
	}

	// iterate through the image for the distance values
	//CalcDistance( temp, w, h );
	RecursiveDistance( temp, w, h );

	// Clean out all of junk and save the medial axis
	CalcAxis( temp, out, w, h );

	// all done
	delete temp;
	return;
}


/*-----------------------------------------------------------------+
	Recursively calculates the distance for every object pixel.
 +-----------------------------------------------------------------*/
void RecursiveDistance( unsigned char* data, int w, int h )
{
	bool noChanges=true;
	int i,j;
	unsigned char u, b, l, r, c;
	unsigned char min;

	for( i=0; i<h; i++ ){
		for( j=0; j<w; j++ ){

			// get the current value
			c = data[i*w+j];

			if( c != 0 ){
				// Always check the 4-neighbors
				u = ( i>0		?	data[(i-1)*w+j] : 0 );
				b = ( i+1<h		?	data[(i+1)*w+j] : 0 );
				l = ( j>0		?	data[i*w+j-1]   : 0 );
				r = ( j+1<w		?	data[i*w+j+1]   : 0 );

				min = u;

				if( min>b )
					min=b;
				if( min>l )
					min=l;
				if( min>r )
					min=r;

				// Check the remaining 8 neighbors depending on the row and column
				u = ( i>0	&& j>0		?	data[(i-1)*w+j-1] : 0 );
				b = ( i+1<h && j+1<w	?	data[(i+1)*w+j+1] : 0 );
				l = ( i+1<h && j>0		?	data[(i+1)*w+j-1] : 0 );
				r = ( i>0	&& j+1<w	?	data[(i-1)*w+j+1] : 0 );

				if( min>u )
					min=u;
				if( min>b )
					min=b;
				if( min>l )
					min=l;
				if( min>r )
					min=r;

				// the distance on this iteration:
				//   d = Min(of 8 neighbors) + 1;
				if( min+1 != c ){
					// then make the update
					noChanges=false;
					data[i*w+j] = min+1;
				}

			}// if else

		}// for j
	}// for i


	// that's right, it's a double negative...
	if( !noChanges ){
		RecursiveDistance( data, w, h );
	}
}


/*------------------------------------------------------------------+
	Take the calculated distances and eliminate those distances
	which are not local maximums, and hence not part of the 
	medial axis.
 +------------------------------------------------------------------*/
void CalcAxis( unsigned char* in, unsigned char* out, int w, int h )
{
	int i, j;
	unsigned char u, b, l, r, c;


	for( i=0; i<h; i++ ){
		for( j=0; j<w; j++ ){
			// get the four corners
			c = in[i*w+j];

			if( c!=0 ){
				u = ( i>0 	? in[(i-1)*w+j] : 0 );
				b = ( i+1<h	? in[(i+1)*w+j] : 0 );
				l = ( j>0	? in[i*w+j-1]   : 0 );
				r = ( j+1<w	? in[i*w+j+1]   : 0 );

				out[i*w+j] = ( ( c<u || c<b || c<l || c<r ) ? 0 : c );

			}// if
		} // for j
	} // for i

/*
	NOTE: as of right now, this does not account for objects of even pixels
	widths.  Hence, the medial axis could be two pixels thick!!
*/
	ThinData( out, w, h );
}


/*----------------------------------------------------------------------+
	Get rid of the double width axis.  This really doesn't do that
	great of a job, but it does it well enough for this project.
 +----------------------------------------------------------------------*/
void ThinData( unsigned char* in, int w, int h )
{
	unsigned char* temp;
	temp = new unsigned char[w*h];
	unsigned char u, b, l, r, ul, br, bl, ur;
	bool needed;

	// copy the data over into temp
	for( int k=0; k<w*h; k++ ){
		temp[k] = in[k];
	}

	for( int i=0; i<h; i++ ){
		for( int j=0; j<w; j++ ){
			if( temp[i*w+j]!=0 ){
				needed = true;

				// check if it is to be deleted
				u = ( i>0 	? temp[(i-1)*w+j] : 0 );
				b = ( i+1<h	? temp[(i+1)*w+j] : 0 );
				l = ( j>0	? temp[i*w+j-1]   : 0 );
				r = ( j+1<w	? temp[i*w+j+1]   : 0 );

				// Check the remaining 8 neighbors depending on the row and column
				ul = ( i>0	&& j>0		?	temp[(i-1)*w+j-1] : 0 );
				br = ( i+1<h && j+1<w	?	temp[(i+1)*w+j+1] : 0 );
				bl = ( i+1<h && j>0		?	temp[(i+1)*w+j-1] : 0 );
				ur = ( i>0	&& j+1<w	?	temp[(i-1)*w+j+1] : 0 );

				// check and say if not needed
				if( ul && !(ur || br) && l && bl && ( b || u ) )
					needed = false;

				if( !(ul || ur) && bl && b && br && ( l || r ) )
					needed = false;

				if( !needed ) { // meets requirements
					in[i*w+j] = 0; // delete it
				} else { // leave it alone
					in[i*w+j] = temp[i*w+j];
				} // if-else

			} // if pixel
		}// for j
	}// for i

	delete temp;
}