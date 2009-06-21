/*------------------------------------------------------------------------+
	Calcualate the medial axis of the homework problems.

	Jason F Smith
	CSE 486 - Homework 2
 +------------------------------------------------------------------------*/

#include <iostream.h>
#include <fstream.h>

#include "medialAxis.h"

int main()
{
	ifstream in;
	ofstream out;
	bool done=false;
	char file[256];
	char outFile[256];
	int width;
	int height;
	unsigned char *inBuffer;
	unsigned char *outBuffer;
	char temp;

	backgroundPixel = 0;

	while( !done ){
		cout <<"Enter input file: ";
		cin  >>file;

		cout <<"\nEnter width: ";
		cin  >>width;

		cout <<"\nEnter height: ";
		cin  >>height;

		cout <<"\nEnter output file: ";
		cin  >>outFile;

		inBuffer = new unsigned char [width*height];
		outBuffer = new unsigned char [width*height];

		in.open( file, ios::binary );
		in.read( inBuffer, width*height );
		in.close();

		CalculateMedial( inBuffer, outBuffer, width, height );

		for( int i=0; i<width*height; i++ ){
			outBuffer[i] = ( outBuffer[i]==0 ? 255 : 0 );
		}

		out.open( outFile, ios::binary );
		out.write( outBuffer, width*height );
		out.close();

		cout <<"\nAnother?";
		cin  >>temp;

		if( temp!='y' && temp!='Y' )
			done = true;
	}


	return 0;
}