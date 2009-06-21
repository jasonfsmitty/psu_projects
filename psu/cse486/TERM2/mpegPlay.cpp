/*---------------------------------------------------------------------------+
	mpegPlay.cpp

	Jason F Smith
	CSE 486 - Semester Project

	Functions to handle watching of the MPEG movie.

 +---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include "defines.h"
#include "cse486_mpeglib-cpp.h"
#include "mpeg.h"


int mpegWinID=0;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void MPEG_Keyboard( unsigned char c, int x, int y )
{
	switch( c ){
		case 27:
		default:
			CloseMyMPEG();
			break;
	};
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void MPEG_Display( void )
{
	unsigned char mpegBuffer[320][3];
	int i, j;

	glutSetWindow( mpegWinID );

	// right this later...
	if( (mpegData.cFrame-startFrame)%frameSkip==0 && mpegWinID!=0 ){
		// update the frame...
		for( i=skipY; i<(240-skipY); i++ )
		{
			for( j=skipX; j<(320-skipX); j++ )
			{
				mpegBuffer[j][0] = mpegData.frame->xbgrbuffer[320*i+j].r;
				mpegBuffer[j][1] = mpegData.frame->xbgrbuffer[320*i+j].g;
				mpegBuffer[j][2] = mpegData.frame->xbgrbuffer[320*i+j].b;
			}

			glRasterPos2i( 0, i-skipY );
			glDrawPixels( movieWidth, 1, GL_RGB, GL_UNSIGNED_BYTE, mpegBuffer );
		}
		// NOT double buffered, so no swapping..
	}
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void MPEG_Reshape( int w, int h )
{
	if( w!=movieWidth || h!=movieHeight ){
		glutSetWindow( mpegWinID );
		glutReshapeWindow( movieWidth, movieHeight );
	}

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	glViewport( 0, 0, movieWidth, movieHeight );
	gluOrtho2D( 0, movieWidth-1, movieHeight-1, 0 );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void OpenMyMPEG( void )
{
	if( mpegWinID ){
		return;
	}

	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB );
	glutInitWindowSize( movieWidth, movieHeight );
	glutInitWindowPosition( 0, 0 );

	mpegWinID = glutCreateWindow( "MPEG Player" );

	glutReshapeFunc( MPEG_Reshape );
	glutDisplayFunc( MPEG_Display );
	glutKeyboardFunc( MPEG_Keyboard );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void CloseMyMPEG( void )
{
	if( mpegWinID!=0 ){
		glutDestroyWindow( mpegWinID );
		mpegWinID=0;
	}
	return;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
bool IsOpenMPEG( void )
{
	return (mpegWinID!=0);
}


