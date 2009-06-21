/*--------------------------------------------------------------------------+
	p3.cpp

	Jason F Smith

	CSE 486   Spring 2001
	Project 3

 +--------------------------------------------------------------------------*/

#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <math.h>

#if defined( _WIN32 )
#	define		M_PI		3.14159//26535897932384626433832795
#endif

#define Abs(A) 		( (A)<0.0 ? -(A) : (A) )

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

struct Corner{
	int x;
	int y;
};

//////////////////////////////////////////////////////////////////

const unsigned char THRESHOLD = 128;

//////////////////////////////////////////////////////////////////
unsigned char data[300][300];
bool isSlant; // or is it tilted

int dT_major;
int dT_minor;
double yT;

int dC_major;
int dC_minor;
double yC;

int dB_major;
int dB_minor;
double yB;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
double CalcAngle( double b, double a, double y1 )
{
	return atan( ( 1.0-b/a ) / (y1) );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
double Calc_A( double d_minor_0_0, double d_major_0_0 )
{
	return ( d_minor_0_0 / d_major_0_0 );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
double Calc_B( double d_minor_0_y, double d_major_0_y )
{
	return ( d_minor_0_y / d_major_0_y );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
double Calc_y1( int y1_min, int y1_max )
{
	return ( (150.0-( (double)y1_max+(double)y1_min)/2.0 ) / 300.0 );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
double Calc_x1( int x1_min, int x1_max )
{
	return Calc_y1( x1_min, x1_max );
	//return ((((double)x1_max+(double)x1_min-1.0)/2.0)-150.0 )/300.0;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int Calc_Major( int leftX, int rightX )
{
	return (rightX-leftX);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int Calc_Minor( int topY, int botY )
{
	return (botY-topY);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ApplyFilter( void )
{
	for( int i=0; i<300*300; i++ ){
		((unsigned char*)data)[i] = ( ((unsigned char*)data)[i]<THRESHOLD ? 0 : 255 );
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CalcTopBrick( void )
{
	int x, y;
	int i, j;
	int good;
	int y1, y2;
	int x1, x2;

	if( isSlant )
	{
		x=150; y=1;

		while( data[x][y]!=255 ) y++;

		good = false;
		while( y<150 && !good )
		{
			good = true;
			for( i=-1; i<2 && good; i++ )
			{
				for( j=-1; j<2 && good; j++ )
				{
					if( data[y+i][x+j]==255 )
						good=false;
				}// for j
			} // for i
			y++;
		} // while

		// supposedly now we have coordinates inside of a brick that can be used
		//  to calculate d_major and d_minor
		
		y1=y2=y;		
		while( data[y1-1][x]!=255 ) y1--;
		while( data[y2+1][x]!=255 ) y2++;

		yT = Calc_y1( y1, y2 );
		y = (int)(yT+0.5);

		x1=x2=x;
		while( data[y][x1-1]!=255 ) x1--;
		while( data[y][x2+1]!=255 ) x2++;

		dT_minor = Calc_Minor( y1, y2 );
		dT_major = Calc_Major( x1, x2 );
	}
	else // is tilted
	{
		x=2; 
		y=151;  // ??

		while( data[x][y]!=255 ) x++;

		good = false;
		while( x<150 && !good )
		{
			good = true;
			for( i=-1; i<2 && good; i++ )
			{
				for( j=-1; j<2 && good; j++ )
				{
					if( data[y+i][x+j]==255 )
						good=false;
				}// for j
			} // for i
			x++;
		} // while

		// supposedly now we have coordinates inside of a brick that can be used
		//  to calculate d_major and d_minor

		x1=x2=x;
		while( data[y][x1-1]!=255 ) x1--;
		while( data[y][x2+1]!=255 ) x2++;

		yT = Calc_x1( x1, x2 );
		x = (int)(yT+0.5);

		y1=y2=y;

		while( data[y1-1][x]!=255 ) y1--;
		while( data[y2+1][x]!=255 ) y2++;
		
		// now calculate the minor/major axis for the brick
		dT_minor = Calc_Minor( y1, y2 );
		dT_major = Calc_Major( x1, x2 );
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CalcCenterBrick( void )
{
	int x, y;
	int i, j;
	int good;
	int y1, y2;
	int x1, x2;

	if( isSlant )
	{
		x=150; y=150;

		good = false;
		while( y<258 && !good )
		{
			good = true;
			for( i=-1; i<2 && good; i++ )
			{
				for( j=-1; j<2 && good; j++ )
				{
					if( data[y+i][x+j]==255 )
						good=false;

				}// for j
			} // for i
			y++;
		} // while

		// supposedly now we have coordinates inside of a brick that can be used
		//  to calculate d_major and d_minor
		
		y1=y2=y;
		while( data[y1-1][x]!=255 ) y1--;
		while( data[y2+1][x]!=255 ) y2++;

		yC = Calc_y1( y1, y2 );
		y = (int)(yC+0.5);

		x1=x2=x;
		while( data[y][x1-1]!=255 ) x1--;
		while( data[y][x2+1]!=255 ) x2++;

		dC_minor = Calc_Minor( y1, y2 );
		dC_major = Calc_Major( x1, x2 );
	}
	else // is tilted
	{
		x=147; 
		y=151;  // ??

		while( data[x][y]!=255 ) x++;

		good = false;
		while( x<258 && !good )
		{
			good = true;
			for( i=-1; i<2 && good; i++ )
			{
				for( j=-1; j<2 && good; j++ )
				{
					if( data[y+i][x+j]==255 )
						good=false;
				}// for j
			} // for i
			x++;
		} // while

		// supposedly now we have coordinates inside of a brick that can be used
		//  to calculate d_major and d_minor

		// tilted means that the horizontal lines are parallel....
		x1=x2=x;
		while( data[y][x1-1]!=255 ) x1--;
		while( data[y][x2+1]!=255 ) x2++;

		yC = Calc_x1( x1, x2 );

		x = (int)(yC+0.5);

		y1=y2=y;

		while( data[y1-1][x]!=255 ) y1--;
		while( data[y2+1][x]!=255 ) y2++;
		
		// now calculate the minor/major axis for the brick
		dC_minor = Calc_Minor( y1, y2 );
		dC_major = Calc_Major( x1, x2 );
	}

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CalcBottomBrick( void )
{
	int x, y;
	int i, j;
	int good;
	int y1, y2;
	int x1, x2;

	if( isSlant )
	{
		x=150; y=258;

		while( data[y][x]!=255 ) y--;

		good = false;
		while( y>150 && !good )
		{
			good = true;
			for( i=-1; i<2 && good; i++ )
			{
				for( j=-1; j<2 && good; j++ )
				{
					if( data[y+i][x+j]==255 ) 
						good=false;
				}// for j
			} // for i
			y--;
		} // while

		// supposedly now we have coordinates inside of a brick that can be used
		//  to calculate d_major and d_minor
		
		// this is slanted, so the horizontial lines are parallel
		y1=y2=y;
		
		while( data[y1-1][x]!=255 ) y1--;
		while( data[y2+1][x]!=255 ) y2++;

		yB = Calc_y1( y1, y2 );
		y = (int)(yB+0.5);

		x1=x2=x;
		while( data[y][x1-1]!=255 ) x1--;
		while( data[y][x2+1]!=255 ) x2++;

		dB_major = Calc_Major( x1, x2 );
		dB_minor = Calc_Minor( y1, y2 );
	}
	else // is tilted
	{
		x=258; 
		y=151;  // ??

		while( data[x][y]!=255 ) x--;

		good = false;
		while( x>150 && !good )
		{
			good = true;
			for( i=-1; i<2 && good; i++ )
			{
				for( j=-1; j<2 && good; j++ )
				{
					if( data[y+i][x+j]==255 )
						good=false;
				}// for j
			} // for i
			x--;
		} // while

		// supposedly now we have coordinates inside of a brick that can be used
		//  to calculate d_major and d_minor

		x1=x2=x;
		while( data[y][x1-1]!=255 ) x1--;
		while( data[y][x2+1]!=255 ) x2++;

		yB = Calc_x1( x1, x2 );
		x = (int)(yB+0.5);

		y1=y2=y;

		while( data[y1-1][x]!=255 ) y1--;
		while( data[y2+1][x]!=255 ) y2++;
		
		// now calculate the minor/major axis for the brick
		dB_minor = Calc_Minor( y1, y2 );
		dB_major = Calc_Major( x1, x2 );
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Reset( void )
{
	dT_major = dT_minor = 0;
	dC_major = dC_minor = 0;
	dB_major = dB_minor = 0;

	yT = yC = yB = 0.0;

	//memset( (void*)data, 0, sizeof( data ) );
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void FindAngle( char* file, double &result )
{
	ifstream input;
	int i, j;

	Reset();

#if defined( _WIN32 )
	input.open( file, ios::binary );
#else
	input.open( file );
#endif

	if( input.fail() ){
		cout <<"Could not open the file.\n";
		result = 0.0;
		return;
	}

	for( i=0; i<300*300; i++ )
		input >> ((unsigned char*)data)[i];

	input.close();

	//check filename to see if it's a slant or tilt...
	if( strstr( file, "slant" ) ){
		isSlant = true;
	} else {
		isSlant = false;
	}

	// check if it's the brick texture, if so we need to threshold it
	if( strstr( file, "brick" ) ){
		ApplyFilter();
	}

	// make sure the data is set correctly (if the filter wasn't used)
	for( i=0; i<300; i++ ){
		for( j=0; j<300; j++ ){
			data[i][j] = ( data[i][j]==0 ? 0 : 255 );
		}
	}

	// Calculate the three bricks needed to get the angle..
	CalcTopBrick();
	CalcCenterBrick();
	CalcBottomBrick();

	double topAngle=0.0;
	double botAngle=0.0;

	topAngle = Abs( CalcAngle(   Calc_B( dT_minor, dT_major ),
				Calc_A( dC_minor, dC_major ),
				yT  )/2.0 );
	botAngle = Abs( CalcAngle(   Calc_B( dB_minor, dB_major ),
				Calc_A( dC_minor, dC_major ),
				yB  ) );

	cout <<"\nTop angle is: "<<(180.0/M_PI)*topAngle<<endl;
	cout <<"Bot angle is: "<<(180.0/M_PI)*botAngle<<endl;

	result = (topAngle + botAngle) / 2.0;
	result *= ( 180.0 / M_PI );
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
int main()
{
	char* file[4]= { "slant.raw",
			 "slantbrick.raw",
			 "tilt.raw",
			 "tiltbrick.raw"
			};
	double angle;

	cout <<"\n\n----------- CSE 486  Project 3  by Jason Smith (Group 19) --------\n";
	
	//cout <<" Enter file (*.raw): ";
	//cin.getline( file, 256, '\n' );

	for( int i=0; i<4; i++ ){
		// do all of that stuff to get the angle
		FindAngle( file[i], angle );
		cout <<"Angle for "<<file[i]<<" was calculated as: "<<angle<<endl;
	}

	return 0;
}
