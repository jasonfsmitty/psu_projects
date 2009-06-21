/*-----------------------------------------------------------------------------+
	histogram.h

	A class to automatically display one or two histograms

 +-----------------------------------------------------------------------------*/

#ifndef JFS_HISTOGRAM_WINDOW_H
#define JFS_HISTOGRAM_WINDOW_H

#include <iostream.h>
#include <GL/glut.h>

#include "int_queue.h"

//////////////////////////////////////////////////////////

#if defined( DEBUG )
#	define HIST_DEBUG
#	pragma message ("INFO - Histogram difference window class debug: ON")
#else
#	pragma message ("INFO - Histogram difference window class debug: OFF")
#endif

#ifndef		Max
#	define	Max(A,B)		((A)<(B) ? (B) : (A) )
#	define	Min(A,B)		((A)<(B) ? (A) : (B) )
#	define	Abs(A)			((A)<0.0 ? -(A) : (A) )
#endif

#define		HIST_WIDTH		160
#define		HIST_HEIGHT		75

///////////////////////////////////////////////////////////
//  Several callback functions which need to be defined
void HIST_Display( void );
void HIST_Reshape( int, int );
void HIST_Keyboard( unsigned char, int, int );

///////////////////////////////////////////////////////////
class HistWin{
	protected:
		int winID;

		IntQueue* q1;
		int	thresh1;
		int step1;

		IntQueue* q2;
		int thresh2;
		int step2;

		int width, height;

	public:
		///////////////////////////////////////////////////////
		HistWin( void ){
			winID=0;
			q1=q2=NULL;
			step1=step2=1;
			thresh1=thresh2=0;
		};

		///////////////////////////////////////////////////////
		~HistWin( void ){
			if( winID ){
#if defined( HIST_DEBUG )
				cout <<"DEBUG - Leaving scope of HistWin, closing\n";
#endif // defined( HIST_DEBUG )
				Close();
			}
		};


		///////////////////////////////////////////////////////
		void PostRedisplay( void )
		{
			if( winID ){
				glutSetWindow( winID );
				glutPostRedisplay();
			}
		};

		///////////////////////////////////////////////////////
		void Setup_1( IntQueue* q, int thresh, int step ){
			if( q ){
				q1 = q;
				thresh1 = thresh;
				step1 = Max( step, 1 );
			} else {
				q1 = NULL;
				thresh1 = 0;
			}
		};

		///////////////////////////////////////////////////////
		void Setup_2( IntQueue* q, int thresh, int step ){
			if( q ){
				q2 = q;
				thresh2 = thresh;
				step2 = Max( step, 1 );
			} else {
				q2 = NULL;
				thresh2 = 0;
			}
		};

		///////////////////////////////////////////////////////
		bool Open( void )
		{
			if( winID || (q1==NULL && q2==NULL) ){

#if defined( HIST_DEBUG )
				cout <<"DEBUG - could not open histogram window ";
				if( winID )
					cout <<"(already open)\n";
				else
					cout <<"(must set at least one queue)\n";
#endif // defined( HIST_DEBUG )

				return false;
			}

			width = HIST_WIDTH;
			height = HIST_HEIGHT;

			glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB );
			glutInitWindowSize( width, height );
			glutInitWindowPosition( 0, 300 );
			winID = glutCreateWindow( "Histogram Window" );

			glutReshapeFunc( HIST_Reshape );
			glutDisplayFunc( HIST_Display );
			glutKeyboardFunc( HIST_Keyboard );

			glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

			return true;
		};

		///////////////////////////////////////////////////////
		bool Close( void )
		{
			if( winID )
				glutDestroyWindow( winID );
			winID = 0;
			return true;
		};

		///////////////////////////////////////////////////////
		bool IsOpen( void )
		{
			return (winID!=0);
		};

		///////////////////////////////////////////////////////
		void Display( void ){
			int i;
			float winStep = 1.0f;

			if( q1 )
				winStep = ((float)q1->NumItems());
			else if( q2 )
				winStep = Max( winStep, ((float)q1->NumItems()) );

			winStep = ((float)width)/winStep;

			glutSetWindow( winID );
			glClearColor( 0.2f, 0.2f, 1.0f, 0.0f );

			glClear( GL_COLOR_BUFFER_BIT );
			if( q1 ){
				glColor3f( 1.0f, 0.3f, 0.0f );
				glBegin( GL_LINE_STRIP );
					for( i=0; i<(q1->NumItems()); i++ )
						glVertex3f( i*winStep, (*q1)[i], 0 );
				glEnd();
			}
			if( q2 ){
				glColor3f( 0.0f, 1.0f, 0.0f );
				glBegin( GL_LINE_STRIP );
					for( i=0; i<(q2->NumItems()); i++ )
						glVertex3f( i*winStep, (*q2)[i], 0 );
				glEnd();
			}

			glBegin( GL_LINES );
				if( q1 ){
					glColor3f( 1.0f, 0.3f, 1.0f );
					glVertex3i( 0, thresh1, 0 );
					glVertex3i( width, thresh1, 0 );
				}

				if( q2 ){
					glColor3f( 1.0f, 1.0f, 0.0f );
					glVertex3i( 0, thresh2, 0 );
					glVertex3i( width, thresh2, 0 );
				}
			glEnd();

			glFlush();
		};

		///////////////////////////////////////////////////////
		void Reshape( int w, int h )
		{
			int top;

			glutSetWindow( winID );

			if( h<=0 )
				h=1;
			if( w<=0 )
				w=1;

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			width = w;
			height = h;
			top = Max( thresh1*2, thresh2*2 );

			if( top<=0 )
				top = 128;

			glViewport( 0, 0, w, h );
			gluOrtho2D( 0, w-1, 0, top );

			glutPostRedisplay();
		};

		///////////////////////////////////////////////////////
		void Keyboard( unsigned char c, int x, int y ){
			Close();
		};
};


///////////////////////////////////////////////////////
// Make sure only here for this file...
#ifdef HIST_DEBUG
#undef HIST_DEBUG
#endif
////////////////////////////////////////////////////////
#endif // !JFS_HISTOGRAM_WINDOW_H
