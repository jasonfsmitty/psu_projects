/*---------------------------------------------------------------------------+

	mpeg_play.h

	Jason F Smith

	Class for opening and playing an MPEG movie for the project.

 +---------------------------------------------------------------------------*/

#ifndef MPEG_PLAY_CLASS_H
#define MPEG_PLAY_CLASS_H

#include <iostream.h>
#include "cse486_mpeglib-cpp.h"

#ifndef MAX_FILENAME
#define	MAX_FILENAME	128
#endif

#if defined( DEBUG )
#	define MPEG_DEBUG
#	pragma message ("INFO - MPEG player class debug: ON")
#else
#	pragma message ("INFO - MPEG player class debug: OFF")
#endif

#define		QUICK_MPEG_PLAY

//////////////////////////////////////////////////////////
// functions which need to be defined...
void MPEG_Display( void );
void MPEG_Reshape( int, int );
void MPEG_Keyboard( unsigned char, int, int );
//////////////////////////////////////////////////////////

struct MPEG_Data{
	mpeginfo*	info;	// returned from opening the mpeg
	mpegframe*	frame;	// current frame info, pixel data

	int cFrame;			// the frame number that is currently loaded
						//  set to (-1) when nothing is loaded or when
						//  the current frame is indeterminent

	int isSwapped;		// Flag for if the MPEG data has been swapped to account
						//  the X channel (alpha?  doesn't matter, we don't use it!
};


//////////////////////////////////////////////////////////////////
class MPEG_Player{
	protected:
		int winID;			// GLUT window ID
		int	width;
		int height;
		MPEG_Data* data;
		int refresh;		// how often should the player refresh
		int last;			// frame number of the last refresh

	public:
		///////////////////////////////////////////////////////
		MPEG_Player( void ){
			winID = 0;
			width = 320;
			height = 240;
			data = NULL;
			refresh = 10;
			last = 0;
		};


		///////////////////////////////////////////////////////
		~MPEG_Player( void ){
			if( winID ){
#if defined( MPEG_DEBUG )
				cout <<"DEBUG - leaving MPEG player scope, closing window\n";
#endif // MPEG_DEBUG
				Close();
			}
		};

		///////////////////////////////////////////////////////////////////////
		void SetData( MPEG_Data* ptr )
		{
			data = ptr;
		};

		///////////////////////////////////////////////////////////////////////
		void SetRefresh( int display_every_nth_frame ){
			refresh = display_every_nth_frame;
		};

		///////////////////////////////////////////////////////////////////////
		void PostRedisplay( void ){
			if( winID ){
				glutSetWindow( winID );
				glutPostRedisplay();
			}
		};

		///////////////////////////////////////////////////////////////////////
		bool IsOpen( void ){ return (winID!=0); };


		///////////////////////////////////////////////////////////////////////
		bool Open( void ){
			if( winID ){
#if defined( MPEG_DEBUG )
				cout <<"DEBUG - could not open MPEG window (already opened)\n";
#endif // MPEG_DEBUG 
				return false;
			}

			glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
			glutInitWindowSize( width, height );
			glutInitWindowPosition( 0, 0 );

			winID = glutCreateWindow( "JFS MPEG Player" );

			glutReshapeFunc( MPEG_Reshape );
			glutDisplayFunc( MPEG_Display );
			glutKeyboardFunc( MPEG_Keyboard );

			last = 0;

			return true;
		};


		///////////////////////////////////////////////////////////////////////
		bool Close( void ){
			if( winID!=0 ){
				glutDestroyWindow( winID );
				winID = 0;
			}
			return true;
		};


		///////////////////////////////////////////////////////////////////////
		void Display( void ){
			mpegframe *p;
			unsigned char t;
			int i;

			if( winID!=0 && data!=NULL && data->frame!=NULL && (data->cFrame!=last) && (data->cFrame-last)>=refresh )
			{
				p = data->frame;

#if !defined( QUICK_MPEG_PLAY )

				if( !data->isSwapped )
				{
					for( i=0; i<(p->width*p->height); i++ )
					{
						p->xbgrbuffer[i].x = p->xbgrbuffer[i].r;
						t = p->xbgrbuffer[i].g;
						p->xbgrbuffer[i].g = p->xbgrbuffer[i].b;
						p->xbgrbuffer[i].b = t;
					}
					data->isSwapped = TRUE;
				}

#endif // QUICK_MPEG_PLAY

				glutSetWindow( winID );
				glDrawPixels( p->width, p->height-5, GL_RGBA, GL_UNSIGNED_BYTE, p->xbgrbuffer );
				glFlush();

				last = data->cFrame;
			}
		};


		///////////////////////////////////////////////////////////////////////
		void Reshape( int w, int h ){
			static bool recursed=false;		// avoid an infinite loop (just incase...)

			glutSetWindow( winID );
			if( (w!=320 || h!=240) && !recursed ){
				recursed=true;
				glutReshapeWindow( 320, 240 );
			}
			recursed=false;
		};


		///////////////////////////////////////////////////////////////////////
		void Keyboard( unsigned char c, int x, int y ){
#if defined( MPEG_DEBUG )
			cout <<"DEBUG - closing MPEG player ("<<(int)c<<"key press)\n";
#endif // defined( MPEG_DEBUG )
			Close();
		};

};

///////////////////////////////////////////////////////////////////////
// All done with the MPEG_DEBUG define, don't want to interfere anywhere else
#ifdef MPEG_DEBUG
#undef MPEG_DEBUG
#endif 
///////////////////////////////////////////////////////////////////////

#endif // MPEG_PLAY_CLASS_H
