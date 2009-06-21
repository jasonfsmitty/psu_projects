/*------------------------------------------------------------+
	Jason F Smith
	CSE 418

	Image data for texturing.

 +------------------------------------------------------------*/

#ifndef _P3_IMAGE_H
#define _P3_IMAGE_H


#include <fstream>
#include <iostream>
using namespace std;
#include <time.h>

//#include "hackedRGB.h"

//#define		LoadTexture(A)		_tdReadRGBTeximage(A)
// Returns a TDteximage* to the loaded values.

#define		MAX_TEX_DATA		65536
#define		TEX_IMAGE_WIDTH		64
#define		TEX_IMAGE_HEIGHT	128


#define		DEFAULT_TEXTURE_FILE		"texture1.dds"
static GLuint textureName;

#if defined( USE_DBZ_TEXTURE )

//TDteximage *texImage;
GLubyte textureData[128 * 64 * 4];
GLubyte fileData[ 128*64*3+1 ];

#define		DBZ_TEXTURE_FILE	"dbzTexture.raw"


bool MakeTextureData( char *filename )
{
	ifstream in;
	in.open( DBZ_TEXTURE_FILE );

	istream* in2;
	in2 = &in.ignore();

	GLubyte b1, b2, b3;
	long i;
	int j, k, w;

	i=0;
	while(  i<24576 ){
		in >> fileData[i++];
	}

	in.close();

	cout <<"read in "<<i<<" bytes of data\n";

	GLubyte *ptr;
	GLubyte *ptr2;
	ptr = fileData;
	ptr2 = textureData;

	for( k=0; k<128; k++ ){
		for( j=0; j<64; j++ ){
			*(ptr2++) = *(ptr++);
			*(ptr2++) = *(ptr++);
			*(ptr2++) = *(ptr++);
			*(ptr2++) = (GLubyte)255;
		}
		for( w=j; w<64; w++){
			
			*(ptr2++) = (GLubyte)255;
			*(ptr2++) = (GLubyte)255;
			*(ptr2++) = (GLubyte)255;
			*(ptr2++) = (GLubyte)255;

			//ptr++;
		}

	}

	return true;
}

#else

unsigned char textureData[TEX_IMAGE_HEIGHT][TEX_IMAGE_WIDTH][4];


#if defined(USE_RANDOM_TEXTURE)

bool MakeTextureData( char *filename )
{
	srand( 0 );
	unsigned char c = rand();

	for( int i=0; i<128; i++ ){
		for( int j=0; j<64; j++ ){
			//c = ((((i&( rand() ) ))^(( rand() ))==0))*(0xaa)+0x55;
			textureData[i][j][0] = (GLubyte) c | rand();
			textureData[i][j][1] = (GLubyte) c & rand();
			textureData[i][j][2] = (GLubyte) c & rand();
			textureData[i][j][3] = (GLubyte) 0xff;
		} // for j
	}// i
	return true;
}

#else // non random texture

bool MakeTextureData( char *filename )
{
	for( int i=0; i<128; i++ ){
		for( int j=0; j<64; j++ ){
			int c = ((((i&0x8)==0)^((j&0x8))==0))*(0xaa)+0x55;
			textureData[i][j][0] = (GLubyte) c;
			textureData[i][j][1] = (GLubyte) c;
			textureData[i][j][2] = (GLubyte) c;
			textureData[i][j][3] = (GLubyte) 255;
		} // for j
	}// i
	return true;
}
#endif // random texture


#endif  // use_dbz_texture



#endif // _p3_image_h
