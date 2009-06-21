/*----------------------------------------------------------------------------------+
	Jason F Smith
	CSE 486   Project 2 - Edge detection.

	Main implementation file for the project.  Requires GLUT to compile (use
	provided makefile or the MS Visual C++ workspace under Windows)
 +----------------------------------------------------------------------------------*/
#if defined( _WIN32 )	// glut in current directory
#	include "glut.h"
#else // we're in 304 Hammond, which has GLUT installed to the include directory
#include <GL/glut.h>
#endif

#include <fstream.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>

#include "myString.h"

// Some defines to control certain compiling options
#define AUTO_ADJUST_MASK
//#define ALLOW_LOG_GRADIENT_METHOD
//#define _POND
//#define SHOW_LOG_MASK_TOTALS

// Several short helper functions

#define		Abs(A)		( (A)<0.0 ? -(A) : (A) )
#define		Max(A,B)	( (A)>(B) ? (A) : (B) )
#define		Min(A,B)	( (A)>(B) ? (B) : (A) )
#define		SafeRelease(A)		{ if( (A) ) delete (A);  (A)=NULL; }

const double ZERO_TOLERANCE = 0.00001;	// how close to zero should be considered zero?
const double ZERO_MARGIN = 1.0;


#define 	DEFAULT_SIGMA		1.5
#define		DEFAULT_ZEROMAX		20
#define		DEFAULT_DISPLAY		LOG_DISPLAY_NORMAL
#define		DEFAULT_THRESH		15.0

///////////////////////////////////////////////////////////////////////
//  Various enums used throughout

// Enums for the menu options
enum {
	MENU_SEPERATER=0,
	MENU_LOAD,
	MENU_SAVE,
	MENU_ZOOM_1X,
	MENU_ZOOM_2X,
	MENU_ZOOM_4X,
	MENU_ZOOM_8X,
	MENU_SET_SIGMA,
	MENU_SET_THRESHOLD,
	MENU_SET_ZERO_MAX,
	MENU_RECALCULATE,
	MENU_LOG_NORMAL,
	MENU_LOG_SIGN,
	MENU_LOG_GRAD,
	MENU_LOG_EDGE_GRAD,
	MENU_LOG_EDGE_SIGN,
	MENU_2X2,
	MENU_ROBERTS,
	MENU_SOBEL,
	MENU_PREWITT,

	MENU_EXIT
};

// Various states that the program can be in
enum {
	NORMAL_STATE=0,
	INPUT_STATE=1
};

// Various states for the LoG detection display
enum {
	LOG_DISPLAY_NORMAL=1,	// regular edge detection with b/w display of edges
	LOG_DISPLAY_SIGN,	// shows the sign of the results of the LoG mask
	LOG_DISPLAY_GRAD
};

// different gradient operators for the LoG edge detection
enum {
	GRADIENT_2X2_OP=1,
	GRADIENT_ROBERTS_OP,
	GRADIENT_SOBEL_OP,
	GRADIENT_PREWITT_OP
};

// States for how the edges are detected
enum {
	LOG_GRADIENT_EDGES=1,
	LOG_SIGN_EDGES
};
	
/////////////////////////////////////////////////////////////////////
// Global constants ------------------------------------------
const int imageWidth = 256;
const int imageHeight = 256;
const int MAX_INPUT	= 256;
const int IMAGE_SIZE = 256*256;
#if defined( _WIN32 )
const double e = 2.71828182846;
#else
const double e = M_E;
#endif

//  Define the gradient operator masks

#define GradMaskType		double
// 2x2 (5.9)
const GradMaskType grad2x2_X[3][3] = { { -1, 1, 0 }, { -1, 1, 0 }, { 0, 0, 0 } };
const GradMaskType grad2x2_Y[3][3] = { { 1, 1, 0 }, { -1, -1, 0 }, { 0, 0, 0} };

// Roberts (5.12)
const GradMaskType gradRoberts_X[3][3] = { { 1, 0, 0 }, { 0, -1, 0 }, { 0, 0, 0 } };
const GradMaskType gradRoberts_Y[3][3] = { { 0, -1, 0 }, { 1, 0, 0 }, { 0, 0, 0 } };

//Sobel (5.16)
const GradMaskType gradSobel_X[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };
const GradMaskType gradSobel_Y[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };

// Prewitt (5.17)
const GradMaskType gradPrewitt_X[3][3] = { { -1, 0, 1 }, { -1, 0, 1 }, {-1, 0, 1 } };
const GradMaskType gradPrewitt_Y[3][3] = { { 1, 1, 1 }, { 0, 0, 0 }, { -1, -1, -1 } };

/////////////////////////////////////////////////////////////////////
// Global Data -----------------------------------------------

int pState;			// current state of the program
int logState;			// state of display for the log results
int logEdgeDetection;
int gradientOp;

GLuint	origTexID=0;		// the texture object ID used to draw the original image
GLuint	logTexID=0;		// the texture object ID used to draw the LoG image
GLuint  facetTexID=0;		// the texture object ID used to draw the facet image

// Unaltered data
char origFile[256];
unsigned char origData[ IMAGE_SIZE ];	// Also used to draw the original image
float logData[ IMAGE_SIZE ];				// The results of the LoG edge detection function
float facetData[ IMAGE_SIZE ];			// The results of the facet edge detection function

// Data used to draw the images using textures
//  This data is altered to show edges better
unsigned char logEdges[ IMAGE_SIZE ];	// The LoG edge data
unsigned char facetEdges[ IMAGE_SIZE ];	// The facet edge data

int screenWidth;
int screenHeight;

double zeroMax = 1.0 ;

float zoom;
int viewX, viewY;
int startX, startY;
int moving;

float sigma;		// for LoG
int gradThreshold;
float threshold;	// for Facet

bool logModified=false;
bool facetModified=false;

// Data for the command line input
char inputPrompt[32];
char inputLine[256];
int  inputIndex;
void (*inputCallback)( char* );

/////////////////////////////////////////////////////////////////////
// Function definitions -------------------------------------

void Init( void ); 		// initialize the application
void InitMenus( void );		// Can get fairly complex, so we'll give it it's own function
void Cleanup( void );		// Make sure we deallocate all of the data
void DisplayStatus( void );

void GetInput( char* prompt, void (*callbackFunc)(char*) );

// Image handling / edge detection functions
void LoadImage( char* file );
void SaveImage( char* file, unsigned char* rawData );
void Calc_LoG( void );
void CalcLoGEdges_Grad( void );
void CalcLoGEdges_Sign( void );

void Calc_Facet( void );
double LoG( int x, int y ); // calculate the value at (x,y) of the mask
double FacetApprox( int x, int y ); // approximate the Facet planer model at the pixel at (x,y)

void ProcessImage( void );
bool CreateOrigTex( void );
bool CreateLoGTex( void );
bool CreateFacetTex( void );

void SetSigma( char* in );
void SetThreshold( char* in );
void SetGradThreshold( char* in );
void SetZeroMax( char* in );

/////////////////////////////////////////////////////////////////////
// Callback functions for GLUT -------------------------------
void Reshape( int w, int h );		// handle window reshapes
void Display( void );			// draw the image
void MouseClick( int button, int state, int x, int y );
void MouseMotion( int x, int y );
void Keyboard( unsigned char c, int x, int y );

// menuing functions
void MainMenu( int value );
void ZoomMenu( int value );
void LoGMenu( int value );
void FacetMenu( int value );

void GLPrint( int x, int y, char* text, ... );
void GLPrint( char* text, ... );

/*******  Start of function implementations *****************************************/

inline int Sign( double x )
{
	if( Abs(x)<= zeroMax )
		return 0;
	if( x<0.0 )
		return -1;
	return 1;
}

inline bool AboutZero( double x )
{
	return ( Abs(x)<=ZERO_MARGIN );
}



/////////////////////////////////////////////////////////////////////
//  Initializes the input state, used to read keyboard input from
//    inside of the OpenGL window, and display it.
/////////////////////////////////////////////////////////////////////
void GetInput( char* prompt, void (*callbackFunc)(char*) )
{	
	if( !callbackFunc )
		return;
	
	pState = INPUT_STATE;

	if( prompt )
		strcpy( inputPrompt, prompt );
	else
		strcpy( inputPrompt, "Unknown Prompt" );

	inputCallback = callbackFunc;
	inputIndex = 0;
	inputLine[0]=NULL;
}


/////////////////////////////////////////////////////////////////////
//  A callback function used with GetInput to set the sigma
//    after the users enters it by the keyboard.
/////////////////////////////////////////////////////////////////////
void SetSigma( char* in )
{
	sigma = StringToFloat( in );
	logModified = true;
	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////
//  Another callback function used inconjunction with GetInput.
//  This one sets the Facet threshold.
/////////////////////////////////////////////////////////////////////
void SetThreshold( char* in )
{
	threshold = StringToFloat( in );
	facetModified = true;
	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////
//  Yet another callback function.  This one sets the threshold
//  which is used in the combination LoG edge detection method.
/////////////////////////////////////////////////////////////////////
void SetGradThreshold( char* in )
{
	gradThreshold = StringToInt( in );
	logModified = true;
	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////
void SetZeroMax( char* in )
{
	zeroMax = StringToFloat( in );
	logModified = true;
	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////
//  Print text to the window at (x,y).  Note, can be used like
//  printf() by adding variables ( "%3.2f", myFloat, etc ) right
//  into the string.
/////////////////////////////////////////////////////////////////////
void GLPrint( int x, int y, char* text, ... )
{
	char text2[256];
	va_list ap;

	va_start( ap, text );
		vsprintf( text2, text, ap );
	va_end(ap);

	glRasterPos2i( x, y );
	int len=strlen( text2 );
	for( int i=0; i<len; i++ )
		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, text2[i] );
}

/////////////////////////////////////////////////////////////////////
//  Same as GLPrint above, only this one does not move the cursor
//  position.
/////////////////////////////////////////////////////////////////////
void GLPrint( char* text, ... )
{
	char text2[256];
	va_list ap;

	va_start( ap, text );
		vsprintf( text2, text, ap );
	va_end(ap);

	int len=strlen( text2 );
	for( int i=0; i<len; i++ )
		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, text2[i] );
}


/////////////////////////////////////////////////////////////////////
//  Load a raw image from a file.  After the image is loaded, it
//  calls ProcessImage() to calculate edges.
/////////////////////////////////////////////////////////////////////
void LoadImage( char* file )
{
	ifstream in;

	if( !file || strlen(file)==0 )
		return;
#if defined( _WIN32 ) // this method works just fine under windows...
	in.open( file, ios::binary );
#else // we are in Hammond,... funny things go on with the images in Hammond...
	in.open( file );
#endif

	if( in.fail() ){
		cout <<"\n** Error ** Could not open file: "<<file<<endl;
		in.close();
		return;
	}

	for( long i=0; i<IMAGE_SIZE; i++ ){
		in.read( &origData[i], 1 );
	}
	in.close();

	strcpy( origFile, file );

	// do edge detection, create textures, etc
	ProcessImage();

	return;
}

/////////////////////////////////////////////////////////////////////
//  Save the image/data into the file.  Assumes that the data is
//  256*256 number of bytes only.  Currently not used, but left in
//  just the same.
/////////////////////////////////////////////////////////////////////
void SaveImage( char* file, unsigned char* rawData )
{
	if( !file || !rawData )
		return;

#if defined( _WIN32 )
	ofstream out( file, ios::binary );
#else
	ofstream out( file );
#endif

	if( out.fail() ){
		cout <<"Error opening save file: "<<file<<endl;
		return;
	}

	out.write( rawData, IMAGE_SIZE );
	out.close();

	return;
}

/////////////////////////////////////////////////////////////////////
//  Calculate all of the edges and create the textures to go
//  along with them for display purposes.
/////////////////////////////////////////////////////////////////////
void ProcessImage( void )
{
	Calc_LoG();
	Calc_Facet();

	CreateOrigTex();
	CreateLoGTex();
	CreateFacetTex();
}


/////////////////////////////////////////////////////////////////////
//  Calculate the LoG function at position (x,y)
/////////////////////////////////////////////////////////////////////
double LoG( int x, int y )
{
	double x2 = x*x;
	double y2 = y*y;
	double s2 = sigma*sigma;

	// g(x,y) = ( (x^2+y^2-2sigma^2) / (sigma^4) ) * e^-( (x^2+y^2)/(2sigma^2) )
	// pow(x,y) = x^y

	double exp = -( (x2+y2)*0.5 / s2 );

//	if( Abs(x)+Abs(y) >= 6 )
//		return 0.0;

	return ( (x2+y2-2*s2)/(s2*s2) ) * pow( e, exp );
}

/////////////////////////////////////////////////////////////////////
//  Calculate the edges via the LoG method.
/////////////////////////////////////////////////////////////////////
void Calc_LoG( void )
{
	double mask[7][7];	// the LoG mask
	double total=0.0; 	// for testing purposes
	long i, j;
	long index;
	int x, y;
	long k;

	memset( logData, 0, sizeof( float[IMAGE_SIZE] ) );
	memset( logEdges, 0, sizeof( unsigned char[IMAGE_SIZE] ) );

	// first, create the mask
	for( i=0; i<7; i++ ){	// rows
		for( j=0; j<7; j++ ){ // columns
			mask[i][j] = LoG( i-3, j-3 );
			total += mask[i][j];
		}
	}

#if defined( SHOW_LOG_MASK_TOTALS )
	cout <<"Sum of LoG mask is: "<<total<<endl;
#endif // defined( SHOW_LOG_MASK_TOTALS )

#if defined( AUTO_ADJUST_MASK )

	// Adjust the total of the mask so that it sums to 1

/*  my old very simple method,... you would think that it works, but really it causes
    more problems than it solves
*/
/*
	for( i=0; i<7; i++ ){
		for( j=0; j<7; j++ ){
			mask[i][j] -= total/49.0;
		}
	}

*/
/*	Here's my new and improved method!!

	Problem:  The sum of the mask is not close enough to zero.

	Old solution:  Take total/49 and subtract from each pixel of mask.
	Why it didn't work:  It caused more noise than was there originally because
			     it gave more weight to the outer pixels than what it
			     should have.

	New Solution:

	sum = summation of Abs( each pixel )

	new pixel value = old - ( Abs(old) / sum ) * total

	Why I think this works better :
		It effects pixels with smaller values much less than those with large absolute
		values.
*/
/*  ok, that didn't work so well,...
*/
	double sum = 0.0;

	// compute the summation
	for( i=0; i<7; i++ ){
		for( j=0; j<7; j++ ){
			sum += Abs( mask[i][j] );
		}
	}

	for( i=0; i<7; i++ ){
		for( j=0; j<7; j++ ){
			mask[i][j] -= Abs( mask[i][j] ) * total / sum;
		}
	}


/*
	this method will apply a second mask to the mask when adjusting the
	values
*/
/*
	double aMask[7][7] =
	{ { 0.0, 0.0, 1.0, 2.0, 1.0, 0.0, 0.0 },
	  { 0.0, 2.0, 4.0, 5.0, 4.0, 2.0, 0.0 },
	  { 1.0, 4.0, 3.0, 2.0, 3.0, 4.0, 1.0 },
	  { 2.0, 5.0, 2.0, 0.0, 2.0, 5.0, 2.0 },
	  { 1.0, 4.0, 3.0, 2.0, 3.0, 4.0, 1.0 },
	  { 0.0, 2.0, 4.0, 5.0, 4.0, 2.0, 0.0 },
	  { 0.0, 0.0, 1.0, 2.0, 1.0, 0.0, 0.0 }  };
	double aTotal;


	aTotal = 0.0;
	for( i=0; i<7; i++ ){
		for( j=0; j<7; j++ ){
			aTotal += aMask[i][j];
		}
	}

	for( i=0; i<7; i++ ){
		for( j=0; j<7; j++ ){
			mask[i][j] -= aMask[i][j] * total / (double)aTotal;
		}
	}

*/

#if defined( SHOW_LOG_MASK_TOTALS )
	// compute the new mean of the mask
	total = 0;
	for( i=0; i<7; i++ ){
		for( j=0; j<7; j++ ){
			total += mask[i][j];
		}
	}

	cout <<"The adjusted LoG mask sum is: "<<total<<endl;
#endif // defined( SHOW_LOG_MASK_TOTALS )

#endif // defined( AUTO_ADJUST_MASK )

	// Apply the mask to all of the pixels
	for( y=3; y<253; y++ ){
		for( x=3; x<253; x++ ){
			index = y*256 + x;
			total = 0.0;

			for( i=-3; i<4; i++ ){
				for( j=-3; j<4; j++ ){
					k = index + 256*i + j;
					total += origData[k] * mask[i+3][j+3];
				} // for j
			} // for i

			// now that we have the value, plug it in

			logData[index] = (float)total;
		}// for x
	}  // for y

#if defined( ALLOW_LOG_GRADIENT_METHOD )

	// Choose how we interpret the results
	if( logEdgeDetection==LOG_GRADIENT_EDGES )
		CalcLoGEdges_Grad();
	else
		CalcLoGEdges_Sign();

#else
	CalcLoGEdges_Sign();
#endif

	logModified=false;
}

/////////////////////////////////////////////////////////////////////
//  Once the mask has been applied this is one method that may be
//  used to detect edges.  I can't seem to get it to work, I think
//  I just misunderstood Kasturi.  But I left it in because I don't 
//  like to delete code.  Use the define at the top of the file
//  to turn on/off the ability to use this method in the program.
//
//  Method:  For every pixel, if logData[x]==0, and gradient at that
//		pixel is above a threshold, then edge pixel.
/////////////////////////////////////////////////////////////////////
void CalcLoGEdges_Grad( void )
{
	GradMaskType grad_X[3][3];
	GradMaskType grad_Y[3][3];
	GradMaskType gradXTotal=0.0;
	GradMaskType gradYTotal=0.0;

	int x, y, j, i;
	long index;

	// get our gradient op mask
	switch( gradientOp ){
		case GRADIENT_2X2_OP:
			memcpy( grad_X, grad2x2_X, sizeof( GradMaskType [3][3] ) );
			memcpy( grad_Y, grad2x2_Y, sizeof( GradMaskType [3][3] ) );
			break;

		case GRADIENT_ROBERTS_OP:
			memcpy( grad_X, gradRoberts_X, sizeof( GradMaskType [3][3] ) );
			memcpy( grad_Y, gradRoberts_Y, sizeof( GradMaskType [3][3] ) );
			break;

		case GRADIENT_SOBEL_OP:
			memcpy( grad_X, gradSobel_X, sizeof( GradMaskType [3][3] ) );
			memcpy( grad_Y, gradSobel_Y, sizeof( GradMaskType [3][3] ) );
			break;

		case GRADIENT_PREWITT_OP:
		default:
			memcpy( grad_X, gradPrewitt_X, sizeof( GradMaskType [3][3] ) );
			memcpy( grad_Y, gradPrewitt_Y, sizeof( GradMaskType [3][3] ) );
			break;
	};

	for( y=3; y<253; y++ ){
		for( x=3; x<253; x++ ){
			index = 256*y+x;

			// determine if this pixel is an edge pixel
			if( logState==LOG_DISPLAY_GRAD ){
					// test the gradient to see if it's really an edge
					for( i=0; i<3; i++ )
					{
						for( j=0; j<3; j++ )
						{
							gradXTotal += grad_X[i][j] * logData[ 256*(i-1+y)+j+x-1 ];
							gradYTotal += grad_Y[i][j] * logData[ 256*(i-1+y)+j+x-1 ];
						}// for j
					} // for i

					if(  ( Abs(gradXTotal) + Abs(gradYTotal) ) > (double)gradThreshold )
					{
						logEdges[ index ] = 255;
					} else 
					{
						logEdges[ index ] = 0;
					}

			} else {
				if( AboutZero(logData[index])==0 ){
					// test the gradient to see if it's really an edge
					for( i=0; i<3; i++ )
					{
						for( j=0; j<3; j++ )
						{
							gradXTotal += grad_X[i][j] * logData[ 256*(i-1+y)+j+x-1 ];
							gradYTotal += grad_Y[i][j] * logData[ 256*(i-1+y)+j+x-1 ];
						}// for j
					} // for i

					if(  ( Abs(gradXTotal) + Abs(gradYTotal) ) > (double)gradThreshold )
					{
						logEdges[ index ] = 255;
					} else 
					{
						logEdges[ index ] = 0;
					}
				} else 
				{
					logEdges[ index ] = 0;
				}
			}
		
		}// for x
	}  // for y
}

/////////////////////////////////////////////////////////////////////
//  This is the primary LoG edge detection decision routine.  Takes
//  the raw data from applying the LoG mask, and computes the
//  edges.
//
//  Method:  For each pixel, if there is a sign change across that
//   pixel in any direction, set as an edge pixel.
/////////////////////////////////////////////////////////////////////
void CalcLoGEdges_Sign( void )
{
	int x, y;
	long index;

	double c, u, d, l, r, ul, ur, dl, dr;

	for( y=3; y<253; y++ ){
		for( x=3; x<253; x++ ){
			index = 256*y+x;

			u=d=l=r=0.0;
			ul=ur=dl=dr=0.0;
			
			// get the 8-neighbors
			u = logData[ index-256 ];
			d = logData[ index+256 ];
			l = logData[ index-1 ];
			r = logData[ index+1 ];
			ul = logData[ index-257 ];
			ur = logData[ index-255 ];
			dl = logData[ index+255 ];
			dr = logData[ index+257 ];

			c = logData[ index ];

			if( Sign(c)==0 ){
				// check if there was a sign change on either side
				if( ( Sign(u)==-Sign(d) && Sign(u)!=0 ) ||
					( Sign(l)==-Sign(r) && Sign(l)!=0 ) ||
					( Sign(ul)==-Sign(dr) && Sign(ul)!=0 ) ||
					( Sign(dl)==-Sign(ur) && Sign(dl)!=0 )		)
				{
					logEdges[index] = 255;
				} else {
					logEdges[index] = 0;
				}

			} else // Sign(c)!=0
			if( ( Sign(c) ==-Sign(ur) && Abs(c)<=Abs(ur) ) ||
				( Sign(c)==-Sign(r ) &&	Abs(c)<=Abs(r ) ) ||
				( Sign(c)==-Sign(dr) &&	Abs(c)<=Abs(dr) ) ||
				( Sign(c)==-Sign(d ) &&	Abs(c)<=Abs(d ) ) ||
				( Sign(c)==-Sign(dl) &&	Abs(c)<=Abs(dl) ) ||
				( Sign(c)==-Sign(l ) &&	Abs(c)<=Abs(l ) ) ||
				( Sign(c)==-Sign(ul) && Abs(c)<=Abs(ul) ) ||
				( Sign(c)==-Sign(u ) &&	Abs(c)<=Abs(u ) )  	)
			{
				logEdges[index] = 255;
			} else {
				logEdges[index] = 0;
			}
		}// for x
	}  // for y
}


/////////////////////////////////////////////////////////////////////
//  Given a position in the image, this function approximates the
//  gradient at that point using the planer facet method.
//
//  Method: Calculate k1, k2, and k3.
//   Plug into equation to determine gradient. ( g = sqrt( k2^2 + k3^3 ) )
/////////////////////////////////////////////////////////////////////
double FacetApprox( int x, int y)
{
	if( x<1 || x>254 || y<1 || y>254 )
		return 0.0;

	double k1=0.0;
	double k2=0.0;
	double k3=0.0;
	double data[3][3];
	int i, j;
//	long index;

	// grap the data so it's easier to work with
	for( i=-1; i<2; i++ ){
		for( j=-1; j<2; j++ ){
			//index = y*256 + x + i*256 + j;
			data[i+1][j+1] = origData[ 256*(y+i) + x + j ];
		}
	}

	// solve for k1
	//  k1 = (sum data)/9
	// solve for k2
	//   k2 = (sum of columns j=-1 and j=1)/6
	// solve for k3
	//   k3 = (sum of rows i=-1 and i=1)/6
	k1=k2=k3=0.0;
	for( i=0; i<3; i++ ){
		for( j=0; j<3; j++ ){
			k1 += data[i][j];
			if( j!=1 )
				k2 += data[i][j] * (float)(j-1);
			if( i!=1 )
				k3 += data[i][j] * (float)(i-1);
		}// for j
	}// for i

	// adjust values
	k1 /= 9.0;
	k2 /= 6.0;
	k3 /= 6.0;

	// this is the equation of the plane!!
	// f(x,y) = k1 + k2*x +k3*y

	// f'/dx = k2
	// f'/dy = k3
	
	return sqrt( (k2*k2 + k3*k3) );
}

/////////////////////////////////////////////////////////////////////
//  This function calculates the edges using the planer facet
//   method (part B)
/////////////////////////////////////////////////////////////////////
void Calc_Facet( void )
{
	int i, j;

	// Compute all of the gradient values
	for( i=0; i<256; i++ ){
		for( j=0; j<256; j++ ){

			// while we're at it, check if it's an edge!!
			if( (facetData[i*256+j]=FacetApprox( j, i )) > threshold ){
				facetEdges[ i*256+j ] = 255;
			} else {
				facetEdges[ i*256+j ] = 0;
			}

		}// for j
	} // for i

	facetModified=false;

}


/////////////////////////////////////////////////////////////////////
//  Creates a texture from the original image.  This allows it to
//   be displayed in the window.
/////////////////////////////////////////////////////////////////////
bool CreateOrigTex( void )
{
	unsigned char temp[ IMAGE_SIZE*3 ];

	if( origTexID ){
		glDeleteTextures( 1, &origTexID );
		origTexID = 0;
	}

	for( long i=0; i<IMAGE_SIZE; i++ ){
		temp[i*3] = temp[i*3+1] = temp[i*3+2] = origData[i];
	}

	glGenTextures( 1, &origTexID );
	glBindTexture( GL_TEXTURE_2D, origTexID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, temp );

	return true;

}


/////////////////////////////////////////////////////////////////////
//  Creates a texture for displaying the results of the LoG edge
//    calculations.
/////////////////////////////////////////////////////////////////////
bool CreateLoGTex( void )
{
	unsigned char temp[ IMAGE_SIZE*3 ];

	if( logTexID ){
		glDeleteTextures( 1, &logTexID );
		logTexID=0;
	}

	if( logState==LOG_DISPLAY_SIGN )
	{
		for( long i=0; i<IMAGE_SIZE; i++ ){
			if( Sign(logData[i]) == 0 ){
				temp[i*3]=0; temp[i*3+1]=0; temp[i*3+2]=0;
			} else if( Sign( logData[i] ) < 0 ){
				temp[i*3]=0; temp[i*3+1]=255; temp[i*3+2]=0;
			} else {
				temp[i*3]=255; temp[i*3+1]=0; temp[i*3+2]=0;
			}
		}
	} else { // logState==LOG_DISPLAY_NORMAL

		for( long i=0; i<IMAGE_SIZE; i++ ){
			temp[i*3] = temp[i*3+1] = temp[i*3+2] = logEdges[i];
		}
	}

	glGenTextures( 1, &logTexID );
	glBindTexture( GL_TEXTURE_2D, logTexID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, temp );

	return true;
}


/////////////////////////////////////////////////////////////////////
//  Another texture producer, this one is for the facet method.
/////////////////////////////////////////////////////////////////////
bool CreateFacetTex( void )
{
	unsigned char temp[ IMAGE_SIZE*3 ];

	if( facetTexID ){
		glDeleteTextures( 1, &facetTexID );
		facetTexID = 0;
	}

	for( long i=0; i<IMAGE_SIZE; i++ ){
		temp[i*3] = temp[i*3+1] = temp[i*3+2] = facetEdges[i];
	}

	glGenTextures( 1, &facetTexID );
	glBindTexture( GL_TEXTURE_2D, facetTexID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, temp );

	return true;
}


/////////////////////////////////////////////////////////////////////
//  GLUT callback function - this handles resizes to the window.
/////////////////////////////////////////////////////////////////////
void Reshape( int w, int h )
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	screenWidth = w;
	screenHeight = h;

	glViewport( 0, 0, screenWidth-1, screenHeight-1 );
	gluOrtho2D( 0, screenWidth-1, screenHeight-1, 0 );
}

/////////////////////////////////////////////////////////////////////
//  GLUT callback function - called upon a refresh request.  It draws
//  the three textures and various state information.
/////////////////////////////////////////////////////////////////////
void Display( void )
{
	float texStartX = ((float)viewX)/255.0f;
	float texStartY = ((float)viewY)/255.0f;

	if( zoom < 1.0f )
		zoom = 1.0f;

	float texEndX = texStartX + 1.0f/zoom;
	float texEndY = texStartY + 1.0f/zoom;
	
	// Make sure not off the edge of the picture
	if( texEndX>1.0f ){
		texStartX -= texEndX-1.0f;
		texEndX = 1.0f;
	}
	if( texEndY>1.0f ){
		texStartY -= texEndY-1.0f;
		texEndY = 1.0f;
	}


	glClear( GL_COLOR_BUFFER_BIT );
	glEnable( GL_TEXTURE_2D );
	glColor3f( 1.0f, 1.0f, 1.0f );
	
	if( strlen( origFile)>0 ){
		if( !origTexID )
			CreateOrigTex();

		glBindTexture( GL_TEXTURE_2D, origTexID );

		glBegin( GL_TRIANGLE_STRIP );
			glTexCoord2f( texStartX, texStartY );
			glVertex2i( 0, 0 );
			
			glTexCoord2f( texStartX, texEndY );
			glVertex2i( 0, screenHeight/2 );

			glTexCoord2f( texEndX, texStartY );
			glVertex2i( screenWidth/2, 0 );

			glTexCoord2f( texEndX, texEndY );
			glVertex2i( screenWidth/2, screenHeight/2 );
		glEnd();
	}

	if( logModified && origTexID ){
		Calc_LoG();
		CreateLoGTex();
	}

	if( logTexID ){
		// Draw the LoG picture
		glBindTexture( GL_TEXTURE_2D, logTexID );

		glPushMatrix();

		glTranslatef( screenWidth/2, 0, 0 );
		glBegin( GL_TRIANGLE_STRIP );
			glTexCoord2f( texStartX, texStartY );
			glVertex2i( 0, 0 );
			
			glTexCoord2f( texStartX, texEndY );
			glVertex2i( 0, screenHeight/2 );

			glTexCoord2f( texEndX, texStartY );
			glVertex2i( screenWidth/2, 0 );

			glTexCoord2f( texEndX, texEndY );
			glVertex2i( screenWidth/2, screenHeight/2 );
		glEnd();
		glPopMatrix();
	}


	if( facetModified && origTexID ){
		Calc_Facet();
		CreateFacetTex();
	}

	if( facetTexID ){
		// Draw the facet picture
		glBindTexture( GL_TEXTURE_2D, facetTexID );

		glPushMatrix();
		glTranslatef( 0, screenHeight/2, 0 );
		glBegin( GL_TRIANGLE_STRIP );
			glTexCoord2f( texStartX, texStartY );
			glVertex2i( 0, 0 );
			
			glTexCoord2f( texStartX, texEndY );
			glVertex2i( 0, screenHeight/2 );

			glTexCoord2f( texEndX, texStartY );
			glVertex2i( screenWidth/2, 0 );

			glTexCoord2f( texEndX, texEndY );
			glVertex2i( screenWidth/2, screenHeight/2 );
		glEnd();
		glPopMatrix();
	}


	// Display some current state information
	DisplayStatus();

	glutSwapBuffers();
}

/////////////////////////////////////////////////////////////////////
//  Displays the various state information in the window.
/////////////////////////////////////////////////////////////////////
void DisplayStatus( void )
{
	glPushMatrix();

		int line=0;
		int step = 15;

		glDisable( GL_TEXTURE_2D );

		// just to make the alignment and stuff easier
		glTranslatef( screenWidth/2+20, screenHeight/2+20, 0 );

		GLPrint( 0, step*(line++), "    --- Info ---" );

//		line++;

		GLPrint( 0, step*(line++), "Image File: " );
		if( origFile[0]!=NULL )
			GLPrint( origFile );
		else 
			GLPrint( "(no image loaded)" );
		
		line++;

		GLPrint( 0, step*(line++), "LoG Sigma: %3.2f", sigma );
		GLPrint( 0, step*(line++), "LoG Zero Bound: %3.2f", zeroMax );
		GLPrint( 0, step*(line++), "LoG Display Type: " );

		if( logState == LOG_DISPLAY_SIGN )
			GLPrint( "Color Sign" );
		else if( logState == LOG_DISPLAY_NORMAL )
			GLPrint( "Normal" );
		else
			GLPrint( "Gradient" );

#if defined( ALLOW_LOG_GRADIENT_METHOD )

		GLPrint( 0, step*(line++), "LoG Edge Detection: " );
		if( logEdgeDetection == LOG_GRADIENT_EDGES )
			GLPrint( "Gradient" );
		else
			GLPrint( "Sign" );

		GLPrint( 0, step*(line++ ), "Gradient Op: " );
		switch( gradientOp ){
			case GRADIENT_2X2_OP:
				GLPrint( "2x2 (5.9)" );
				break;
			case GRADIENT_ROBERTS_OP:
				GLPrint( "Roberts (5.12)" );
				break;
			case GRADIENT_SOBEL_OP:
				GLPrint( "Sobel (5.16)" );
				break;
			case GRADIENT_PREWITT_OP:
				GLPrint( "Prewitt (5.17)" );
				break;
			default:
				GLPrint( "Unknown, please set" );
				break;
		};
		GLPrint( 0, step*(line++), "Gradient Threshold: %d", gradThreshold );

#endif // defined( ALLOW_LOG_GRADIENT_METHOD )

		GLPrint( 0, step*(line++), "Facet Threshold: %5.2f", threshold );

		line++;

		GLPrint( 0, step*(line++), "Zoom: %dx", (int)zoom );

		if( pState==INPUT_STATE ){
			inputLine[inputIndex]=NULL;

			line++;

			GLPrint( 0, step*(line++), inputPrompt );
			GLPrint( 0, step*(line++), ">" );
			GLPrint( inputLine );
			GLPrint( "_" );
		}

	glPopMatrix();
}


/////////////////////////////////////////////////////////////////////
//  GLUT callback - handles mouse clicks
/////////////////////////////////////////////////////////////////////
void MouseClick( int button, int state, int x, int y )
{
	if( pState==INPUT_STATE )
		pState=NORMAL_STATE;

	if( button==GLUT_LEFT_BUTTON ){
		if( state==GLUT_DOWN ){
			moving = 1;
			startX = x;
			startY = y;
		} else {
			moving = 0;
		}
	}

	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////
//  GLUT callback - handles mouse movements while a mouse button is
//   held down.
/////////////////////////////////////////////////////////////////////
void MouseMotion( int x, int y )
{
	if( moving ){
		viewX -= x-startX;
		viewY -= y-startY;

		if( viewX < 0 )		viewX = 0;
		if( viewX >255 )	viewX = 255;
		if( viewY < 0 )		viewY = 0;
		if( viewY > 255 )	viewY = 255;

		startX = x;
		startY = y;
	}

	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////
//  GLUT callback - handles keyboard input
/////////////////////////////////////////////////////////////////////
void Keyboard( unsigned char c, int x, int y )
{

	if( pState==INPUT_STATE ){
		// capture the input

		// process special buttons
		if( c == 13 ){ 
			// Enter was pressed!  time to go to work!!
			pState=NORMAL_STATE;
			inputLine[inputIndex]=NULL;
			inputCallback( inputLine );
		} else if( c == 27 ){// ESCAPE
			pState=NORMAL_STATE;
		} else if( c == 8 ){ // backspace key
			if( inputIndex>0 )
				inputIndex--;
		} else { // default processing, enter it into the string
			if( inputIndex+1<MAX_INPUT ){
				inputLine[inputIndex++] = (char)c;
			}
		}
	} else { // normal state
		switch( c ){
			case 27: // escape
				Cleanup();
				exit(0);
			
			case 'Z':
			case 'z':
				LoGMenu( MENU_SET_ZERO_MAX );
				break;

			case 'c':
			case 'C':
				LoGMenu( MENU_LOG_SIGN );
				break;

			case 'n':
			case 'N':
				LoGMenu( MENU_LOG_NORMAL );
				break;

			case 'l': // go into load file mode
			case 'L':
				MainMenu( MENU_LOAD );
				break;

			case 's':
			case 'S':	// set sigma
				LoGMenu( MENU_SET_SIGMA );
				break;

			case 't':	// set the facet threshold
			case 'T':
				FacetMenu( MENU_SET_THRESHOLD );
				break;

			case '+':	// zoom in
				if( zoom<8.0 )
					zoom *= 2.0;
				break;

			case '-':	// zoom out
				if( zoom > 1.0 )
					zoom *= 0.5;
				break;

			default:
				break;
		};
	}

	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////
//  GLUT callback - handles choices in the main menu
/////////////////////////////////////////////////////////////////////
void MainMenu( int value )
{
	//char input[256];
	char* str=NULL;

	if( pState==INPUT_STATE )
		pState=NORMAL_STATE;

	switch( value )
	{
		case MENU_LOAD:
			GetInput( "Enter image filename:", LoadImage );
#if !defined( _WIN32 ) && !defined( _POND ) // then in 304 Hammond...
			inputLine[0]=NULL;
			strcpy( inputLine, "../../../cg486/images/" );
			inputIndex = strlen( inputLine );
#endif
			break;

		case MENU_SAVE:
			break;

		case MENU_RECALCULATE:
			if( strlen( origFile )>0 )
				LoadImage( origFile );
			break;

		case MENU_EXIT:
			Cleanup();
			exit(0);
	};

	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////
//   ZoomMenu - set the different zoom values
/////////////////////////////////////////////////////////////////////
void ZoomMenu( int value )
{
	if( pState==INPUT_STATE )
		pState=NORMAL_STATE;

	switch( value ){
		case MENU_ZOOM_1X:
			zoom = 1.0f;
			break;
		case MENU_ZOOM_2X:
			zoom = 2.0f;
			break;
		case MENU_ZOOM_4X:
			zoom = 4.0f;
			break;
		case MENU_ZOOM_8X:
			zoom = 8.0f;
			break;
		default:
			zoom = 1.0f;
			break;
	};

	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////
// GLUT callback - handles choices in the facet options submenu
/////////////////////////////////////////////////////////////////////
void FacetMenu( int value )
{
	switch( value )
	{
		case MENU_SET_THRESHOLD:
			GetInput( "Enter facet threshold:", SetThreshold );
			break;
	};

	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////
//  GLUT callback - handles choices in the LoG options submenu
/////////////////////////////////////////////////////////////////////
void LoGMenu( int value )
{
	switch( value )
	{
		case MENU_SET_SIGMA:
			GetInput( "Enter sigma:", SetSigma );
			break;

		case MENU_SET_THRESHOLD:
			GetInput( "Enter grad threshold:", SetGradThreshold );
			break;

		case MENU_SET_ZERO_MAX:
			GetInput( "Enter zero upper bound:", SetZeroMax );
			break;

		case MENU_LOG_EDGE_GRAD:
			logEdgeDetection = LOG_GRADIENT_EDGES;
			logModified = true;
			break;
	
		case MENU_LOG_EDGE_SIGN:
			logEdgeDetection = LOG_SIGN_EDGES;
			logModified = true;
			break;

		case MENU_LOG_NORMAL:
			logState = LOG_DISPLAY_NORMAL;
			logModified=true;
			break;

		case MENU_LOG_SIGN:
			logState = LOG_DISPLAY_SIGN;
			logModified=true;
			break;

		case MENU_LOG_GRAD:
			logState = LOG_DISPLAY_GRAD;
			logModified=true;
			break;

		case MENU_2X2:
			gradientOp = GRADIENT_2X2_OP;
			logModified=true;
			break;

		case MENU_ROBERTS:
			gradientOp = GRADIENT_ROBERTS_OP;
			logModified=true;
			break;

		case MENU_SOBEL:
			gradientOp = GRADIENT_SOBEL_OP;
			logModified=true;
			break;

		case MENU_PREWITT:
			gradientOp = GRADIENT_PREWITT_OP;
			logModified = true;
			break;
	};

	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////
//   Init() - Initialize the window, menus, etc.
/////////////////////////////////////////////////////////////////////
void Init( void )
{
	// Setup the window
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowSize( 500, 500 );
	// glutInitWindowPosition( 200, 200 );
	glutCreateWindow( "CSE 486 P2 - Edge Detection by JFS" );

	// Add in our event handlers
	glutReshapeFunc( Reshape );
	glutDisplayFunc( Display );
	glutMouseFunc( MouseClick );
	glutMotionFunc( MouseMotion );
	glutKeyboardFunc( Keyboard );

	glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );

	// Initialize all of the menus and attach to the right
	//   mouse button
	InitMenus();

	origTexID = logTexID = facetTexID = 0;
	pState = NORMAL_STATE;
	sigma = DEFAULT_SIGMA;
	threshold = DEFAULT_THRESH;
	gradientOp = GRADIENT_PREWITT_OP;
	gradThreshold = 64;
	logEdgeDetection = LOG_SIGN_EDGES;
	zeroMax = DEFAULT_ZEROMAX;
	logState = LOG_DISPLAY_NORMAL;

	Reshape( 500, 500 );

	glEnable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
}


/////////////////////////////////////////////////////////////////////
//   InitMenus - Initialize and setup all of the menu stuff
/////////////////////////////////////////////////////////////////////
void InitMenus( void )
{
	// Create the main menu

	int z_menu = glutCreateMenu( ZoomMenu );
	glutAddMenuEntry( "1x", MENU_ZOOM_1X );
	glutAddMenuEntry( "2x", MENU_ZOOM_2X );
	glutAddMenuEntry( "4x", MENU_ZOOM_4X );
	glutAddMenuEntry( "8x", MENU_ZOOM_8X );


	int log_display_menu = glutCreateMenu( LoGMenu );
	glutAddMenuEntry( "Normal B/W Edge Image", MENU_LOG_NORMAL );
	glutAddMenuEntry( "Sign Image", MENU_LOG_SIGN );

#if defined( ALLOW_LOG_GRADIENT_METHOD )
	glutAddMenuEntry( "Gradient Image", MENU_LOG_GRAD );

	int log_grad_menu = glutCreateMenu( LoGMenu );
	glutAddMenuEntry( "2x2 (5.9)", MENU_2X2 );
	glutAddMenuEntry( "Roberts (5.12)", MENU_ROBERTS );
	glutAddMenuEntry( "Sobel (5.16)", MENU_SOBEL );
	glutAddMenuEntry( "Prewitt (5.17)", MENU_PREWITT );

	int edge_menu = glutCreateMenu( LoGMenu );
	glutAddMenuEntry( "Gradient", MENU_LOG_EDGE_GRAD );
	glutAddMenuEntry( "Sign Changes", MENU_LOG_EDGE_SIGN );

#endif

	int log_menu = glutCreateMenu( LoGMenu );

#if defined( ALLOW_LOG_GRADIENT_METHOD )
	glutAddSubMenu( "Detection Method", edge_menu );
	glutAddSubMenu( "Gradient Op", log_grad_menu );
#endif

	glutAddSubMenu( "Display Mode", log_display_menu );
	glutAddMenuEntry( "Set Sigma...", MENU_SET_SIGMA );

#if defined( ALLOW_LOG_GRADIENT_METHOD )
	glutAddMenuEntry( "Set Threshold...", MENU_SET_THRESHOLD );
#endif
	
	glutAddMenuEntry( "Set Zero Max...", MENU_SET_ZERO_MAX );



	int facet_menu = glutCreateMenu( FacetMenu );
	glutAddMenuEntry( "Set Threshold...", MENU_SET_THRESHOLD );



	glutCreateMenu( MainMenu );
	glutAddMenuEntry( "CSE 486 - P2", MENU_SEPERATER );
	glutAddMenuEntry( "  ", MENU_SEPERATER );
	glutAddMenuEntry( "Load Image...", MENU_LOAD );
	glutAddMenuEntry( "Reload Image", MENU_RECALCULATE );

	glutAddSubMenu( "LoG Options", log_menu );
	glutAddSubMenu( "Facet Options", facet_menu );

	glutAddSubMenu( "Zoom", z_menu );

	glutAddMenuEntry( "   ", MENU_SEPERATER );
	glutAddMenuEntry( "Exit", MENU_EXIT );

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}


/////////////////////////////////////////////////////////////////////
//  Cleanup the textures (or anything else)
/////////////////////////////////////////////////////////////////////
void Cleanup( void )
{
	if( origTexID )
		glDeleteTextures( 1, &origTexID );
	if( logTexID )
		glDeleteTextures( 1, &logTexID );
	if( facetTexID )
		glDeleteTextures( 1, &facetTexID );
}


/////////////////////////////////////////////////////////////////////
//   main() - where everything starts
////////////////////////////////////////////////////////////////////
int main( int argc, char **argv )
{
	glutInit( &argc, argv );
	
	// Do our initialization stuff, window, menus, etc.
	Init();

	// Enter the glut main loop, never returns from this!
	glutMainLoop();

	return 0; // never reached!!
}


/*******/
/* EOF */
/*******/
