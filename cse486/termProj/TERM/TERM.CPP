/******************************************************************************

	CSE 486  Term Project

	Jason F Smith

 ******************************************************************************/

// Some global defines which are used throughout the project
//   along with all the default header files and such
#include "defines.h"

// files that are OK to keep in their respective headers..
#include "int_queue.h"
#include "linklist2.h"
#include "histogram.h"
#include "mpeg_play.h"
#include "myString.h"
#include "proj_db.h"


///////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//	global data...
//****************************************************************************
///////////////////////////////////////////////////////////////////////////////

// Movie files.....
char* 	movieFiles[] = { 
	"a.mpg",
	"b.mpg",
	"c.mpg",
	"d.mpg"
};

char* 	truthFiles[] = {
	"a.gtf",
	"b.gtf",
	"c.gtf",
	"d.gtf"
};

const 	int	numFiles=4;


MPEG_Player	mpegWin;
HistWin 	histWin;
GLUI		*glui;

MPEG_Data	mpegData;

union MPEG_FrameBuffer {
	int image[240][320];
	int buffer[ 320*240 ];
};

MPEG_FrameBuffer	cPtr;

MPEG_FrameBuffer	colorPrev;
bool			colorStarted;
MPEG_FrameBuffer	motionPrev;
bool			motionStarted;


int	colorHistory[ 4200 ];
int	colorIndex;

// current options and such......

// first the color stuff -----------------------------------
int		colorThreshold=32;
int		colorStep=0;
int		showColorHist=TRUE;
int		colorAvg;
int		nextColorFrame=0;

// motion stuff --------------------------------------------
int		motionThreshold=30;
int		motionStep=0;
int		showMotionHist=TRUE;
float		motionAvg;
int		nextMotionFrame=0;
int		blockSize=8;
int		blockRange=10;


// global options ------------------------------------------
int		movieIndex=0;		// which movie is selected
int		runStart=0;		// where to start the movie at
int		runEnd=-1;		// where to end the movie
int		runType=2;
int		frameStep=10;		// do we play every frame?  if not, how many do we skip
int		watchMovie=TRUE;	// do we watch the move while it's playing??


// log file stuff
int		useLog=TRUE;
char		logFileName[ sizeof( GLUI_String ) ] = "log.txt";

IntQueue	colorQ;
IntQueue	motionQ;

// a database to collect the information for the cuts...
Cut_DB		db;

// result display stuff ------------------------------------

GLUI			*glui_result;
GLUI_StaticText		*prec_text;
GLUI_StaticText 	*recall_text;
GLUI_StaticText		*truth_text;
GLUI_StaticText		*range_text;
GLUI_StaticText		*log_text;
GLUI_StaticText		*movie_text;

//**************************************************************************//
//**************************************************************************//

///////////////////////////////////////////////////////////////////////////////
//	Callback function for the GLUI live variables and stuff...
///////////////////////////////////////////////////////////////////////////////
void Control_CB( int value )
{
	switch( value )
	{
		case RUN_DETECTION:
			EnterMode( CALC_MODE );
			break;

		case PREVIEW_MOVIE:
			EnterMode( PREVIEW_MODE );
			break;

		case QUIT:
			CleanUp();
			exit(0);
			break;
	};
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SetOptions( int mode )
{
	switch( mode )
	{
		case SPEED_MODE:
			showColorHist = FALSE;
			showMotionHist = FALSE;
			watchMovie = FALSE;

			break;

		case DEBUG_MODE:
			showColorHist = TRUE;
			showMotionHist = TRUE;
			watchMovie = TRUE;

			break;
	};

	GLUI_Master.sync_live_all();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MPEG_Display( void )
{
	if( mpegWin.IsOpen() ){
		mpegWin.Display();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MPEG_Reshape( int w, int h )
{
	if( mpegWin.IsOpen() ){
		mpegWin.Reshape( w, h );
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MPEG_Keyboard( unsigned char c, int x, int y )
{
	if( mpegWin.IsOpen() ){
		mpegWin.Keyboard( c, x, y );
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HIST_Display( void )
{
	if( histWin.IsOpen() ){
		histWin.Display();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HIST_Reshape( int w, int h )
{
	if( histWin.IsOpen() )
		histWin.Reshape( w, h );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HIST_Keyboard( unsigned char c, int x, int y )
{
	if( histWin.IsOpen() )
		histWin.Keyboard( c, x, y );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void EnterMode( int state )
{
	GLUI_Master.set_glutIdleFunc( NULL );

	mpegWin.Close();

	nextColorFrame=nextMotionFrame=0;

	if( state!=RESULT_MODE ){
		histWin.Close();
	}

	if( glui_result ){
		glui_result->hide();
	}

	if( mpegData.frame!=NULL ){
		mpegDestroyFrame( mpegData.frame );
		mpegData.frame = NULL;
	}

	if( mpegData.info!=NULL ){
		mpegClose( mpegData.info );
		mpegData.info = NULL;
		cout <<"Stopped at frame: "<<mpegData.cFrame<<"\n";
	}

	if( runStart<0 )
	{
		runStart=0;
	}

	switch( state )
	{

		case OPTIONS_MODE:
			// show the options window..

			histWin.Close();
			mpegWin.Close();

			GLUI_Master.sync_live_all();
			glui->show();

			break;

		case PREVIEW_MODE:
			// just preview the movie
			// open the mpeg window

			mpegData.info = mpegOpen( movieFiles[movieIndex] );

			if( !mpegData.info ){
				EnterMode( OPTIONS_MODE );
			}

			if( mpegGotoFrame( mpegData.info, runStart )< 0 ){ // index too big
				mpegClose( mpegData.info );
				mpegData.info = NULL;
				EnterMode( OPTIONS_MODE );
			}

			mpegData.cFrame = runStart;

			glui->hide();
			GLUI_Master.set_glutIdleFunc( PreviewIdle );

			mpegWin.SetRefresh( frameStep );
			mpegWin.Open();
			break;


		case CALC_MODE:
			db.Reset();
			db.LoadTruth( truthFiles[ movieIndex ] );
			db.SetCutThreshold( 5 );
			db.SetRange( runStart, runEnd );

			colorQ.Reset();
			motionQ.Reset();

			colorIndex = 0;

			cPtr;
			colorStarted=false;
			motionStarted=false;
			colorStep = Max( 0, colorStep );
			motionStep = Max( 0, motionStep );
			frameStep = Max( 0, frameStep );

			// run the cut sceen detection algorithm...
			mpegData.info = mpegOpen( movieFiles[movieIndex] );

			if( mpegData.info==NULL )
				EnterMode( OPTIONS_MODE );

			if( mpegGotoFrame( mpegData.info, runStart )< 0 ){ // index too big
				mpegClose( mpegData.info );
				mpegData.info = NULL;
				EnterMode( OPTIONS_MODE );
			}

			mpegData.cFrame = runStart;

			glui->hide();

			if( watchMovie ){
				mpegWin.SetRefresh( Max(frameStep,0) );
				mpegWin.Open();
			}

			if( (showColorHist && runType!=MOTION_METHOD) || (showMotionHist && runType!=COLOR_METHOD) )
			{
				if( showColorHist && runType!=MOTION_METHOD ){
					histWin.Setup_1( &colorQ, colorThreshold, colorStep );
				} else {
					histWin.Setup_1( NULL, 0, 0 );
				}

				if( showMotionHist && runType!=COLOR_METHOD ){
					histWin.Setup_2( &motionQ, motionThreshold, motionStep );
				} else {
					histWin.Setup_2( NULL, 0, 0 );
				}

				histWin.Open();
			}

			if( histWin.IsOpen() || mpegWin.IsOpen() )
				GLUI_Master.set_glutIdleFunc( CalcIdle );
			else
				GLUI_Master.set_glutIdleFunc( SpeedIdle );

			break;


		case RESULT_MODE:
			// show the results of running the algorithm
			db.SetRange( Max(0,runStart), mpegData.cFrame );

			RunFilter();

			db.CalcAll();

			if( useLog ){
				db.SetLogFile( logFileName );
				db.WriteLog();
			}

			ShowResults();

			break;
	};
}


///////////////////////////////////////////////////////////////////////////////
//	Run though the algorithm with one of the two windows opened
///////////////////////////////////////////////////////////////////////////////
void CalcIdle( void )
{
	int nextFrame=5000;

	if( (runEnd>0 && mpegData.cFrame>runEnd )
		||  (mpegData.info==NULL)
		||  (mpegData.frame=mpegGetNextFrame( mpegData.info ))==NULL
		||  (!mpegWin.IsOpen() && !histWin.IsOpen())  )
	{
		// exit from preview mode...
		EnterMode( RESULT_MODE );
		return;
	}

	mpegData.isSwapped=FALSE;
	mpegData.cFrame++;

	// Precalculate the gray values ....
	xbgr *ptr;
	ptr = mpegData.frame->xbgrbuffer;

	for( int i=0; i<320*240; i++ )
	{
		cPtr.buffer[i] = ptr[i].r + ptr[i].g + ptr[i].b;
	}

	// Run the color histogram algorithm....
	if( colorStarted && (mpegData.cFrame>=nextColorFrame) )
	{
		RunColor();	// go through one iteration of the color histogram method

		nextColorFrame = mpegData.cFrame + Max(0, colorStep );
		nextFrame = colorStep;

		memcpy( colorPrev.buffer, cPtr.buffer, sizeof( int [320*240] ) );
	} else if( runType!=MOTION_METHOD ){
		nextColorFrame=-1;
		nextFrame=colorStep;
		memcpy( colorPrev.buffer, cPtr.buffer, sizeof( int [320*240] ) );
		colorStarted=true;
	}
		

	if( motionStarted && mpegData.cFrame>=nextMotionFrame )
	{
		RunMotion();

		nextMotionFrame = mpegData.cFrame + Max( 0, motionStep );
		nextFrame = Min( nextFrame, motionStep );

		memcpy( motionPrev.buffer, cPtr.buffer, sizeof( int [320*240] ) );
	} else if( runType!=COLOR_METHOD ){
		memcpy( motionPrev.buffer, cPtr.buffer, sizeof( int [320*240] ) );
		nextMotionFrame=-1;
		nextFrame = Min( nextFrame, motionStep );
		motionStarted=true;
	}

	if( mpegWin.IsOpen() ){
		mpegWin.Display();
		nextFrame = Min( nextFrame, frameStep );
	}

	if( histWin.IsOpen() ){
		// Add new histogram values to the queues and update the display...
		if( showColorHist ){
			colorQ.Push( colorAvg );
		}

		if( showMotionHist ){
			motionQ.Push( motionAvg );
		}

		histWin.Display();
	}


	// destroy the frame
	if( mpegData.frame!=NULL ){
		mpegDestroyFrame( mpegData.frame );
		mpegData.frame=NULL;
	}


	// if need be, search to the next frame
	if( nextFrame!=5000 && nextFrame>1 && mpegData.info!=NULL )
	{
		mpegData.cFrame+=nextFrame-1;
		//cout <<"loading frame: "<<mpegData.cFrame<<"\n";
		if( mpegGotoFrame( mpegData.info, mpegData.cFrame )<0 )
			EnterMode( RESULT_MODE );
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Run through the algorithm as fast as possible
///////////////////////////////////////////////////////////////////////////////
void SpeedIdle( void )
{
	int nextFrame=5000;

	cout <<"Running windowless, please be patient...\n";

	while( 1 ){

	nextFrame=5000;

	if( (runEnd>0 && mpegData.cFrame>runEnd )
		||  (mpegData.info==NULL)
		||  (mpegData.frame=mpegGetNextFrame( mpegData.info ))==NULL  )
	{
		// exit from preview mode...
		GLUI_Master.set_glutIdleFunc( NULL );
		EnterMode( RESULT_MODE );
		return;
	}

	mpegData.cFrame++;

	// Precalculate the gray values ....
	xbgr *ptr;
	ptr = mpegData.frame->xbgrbuffer;

	for( int i=0; i<320*240; i++ ){
		cPtr.buffer[i] = ptr[i].r + ptr[i].g + ptr[i].b;
	}

	// destroy the frame
	if( mpegData.frame!=NULL ){
		mpegDestroyFrame( mpegData.frame );
		mpegData.frame=NULL;
	}

	// Run the color histogram algorithm....
	if( colorStarted && (mpegData.cFrame>=nextColorFrame) )
	{
		RunColor();	// go through one iteration of the color histogram method

		nextColorFrame = mpegData.cFrame + Max(0, colorStep );
		nextFrame = colorStep;

		memcpy( colorPrev.buffer, cPtr.buffer, sizeof( int [320*240] ) );
	} else if( runType!=MOTION_METHOD ){
		nextColorFrame=-1;
		nextFrame=colorStep;
		memcpy( colorPrev.buffer, cPtr.buffer, sizeof( int [320*240] ) );
		colorStarted=true;
	}
		

	if( motionStarted && mpegData.cFrame>=nextMotionFrame )
	{
		RunMotion();

		nextMotionFrame = mpegData.cFrame + Max( 0, motionStep );
		nextFrame = Min( nextFrame, motionStep );

		memcpy( motionPrev.buffer, cPtr.buffer, sizeof( int [320*240] ) );
	} else if( runType!=COLOR_METHOD ){
		memcpy( motionPrev.buffer, cPtr.buffer, sizeof( int [320*240] ) );
		nextMotionFrame=-1;
		nextFrame = Min( nextFrame, motionStep );
		motionStarted=true;
	}


	// if need be, search to the next frame
	if( nextFrame!=5000 && nextFrame>1 && mpegData.info!=NULL )
	{
		mpegData.cFrame+=nextFrame-1;
		if( mpegGotoFrame( mpegData.info, mpegData.cFrame )<0 ){
			GLUI_Master.set_glutIdleFunc( NULL );
			EnterMode( RESULT_MODE );		
			return;
		}
	}

	}// while(1)
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PreviewIdle( void )
{
	if( (runEnd>0 && mpegData.cFrame>runEnd )
		||  (mpegData.info==NULL)
		||  (mpegData.frame=mpegGetNextFrame( mpegData.info ))==NULL 
		||  !mpegWin.IsOpen()  )
	{
		// exit from preview mode...
		EnterMode( OPTIONS_MODE );
		return;
	}
	
	// else keep on going....

	mpegData.cFrame++;
	mpegData.isSwapped = FALSE;

	mpegWin.Display();

	if( mpegData.frame!=NULL )
		mpegDestroyFrame( mpegData.frame );
	mpegData.frame = NULL;

	if( frameStep>1 && mpegData.info!=NULL ){
		if( mpegGotoFrame( mpegData.info, mpegData.cFrame+frameStep ) < 0 ){
#if defined( DEBUG )
			cout <<" DEBUG - Reached the end of the movie\n";
#endif // DEBUG
			EnterMode( OPTIONS_MODE );
			return;
		}
		mpegData.cFrame += frameStep;
	}
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RunFilter( void )
{
	LinkList<CutNode>* cuts;
	int i=0;

	int prevFrame = 0;

	cuts = db.GetCuts();

	cout <<(*cuts).GetSize()<<endl;

	while( i<(*cuts).GetSize() )
	{
		if( Abs( (*cuts)[i].frameNum-prevFrame )  <= colorStep )
		{
			prevFrame = (*cuts)[i].frameNum;
			cout <<"DEBUG - Deleting cut scene for frame "<<prevFrame<<"\n";
			(*cuts).Delete( i );
		} else {
			i++;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShowResults( void )
{
	static bool firstTime=true;

	char prec_string[16];
	char recall_string[16];
	char truth_string[16];
	char range_string[16];
	char movie_string[16];
	char log_string[128];

	// calculate the precision/recall

	sprintf( prec_string, "%3.2f%", 100.0f*(db.GetPrecision()) );
	sprintf( recall_string, "%3.2f%", 100.0f*(db.GetRecall()) );
	sprintf( truth_string, "%d", db.GetNumTruths() );
	sprintf( range_string, "%d thru %d", runStart, mpegData.cFrame );
	
	strcpy( movie_string, movieFiles[ movieIndex ] );

	if( useLog ){
		strcpy( log_string, logFileName );
	} else {
		strcpy( log_string, "N/A" );
	}

	if( firstTime )
	{
		glui_result = GLUI_Master.create_glui( "Results" );

		// Add the static text which goes down the left side...
		glui_result->add_statictext( "Precision:" );
		glui_result->add_statictext( "Recall:" );
		glui_result->add_statictext( "# Truths:" );
		glui_result->add_statictext( "Frame range:" );
		glui_result->add_statictext( "Movie file:" );
		glui_result->add_statictext( "Log file:" );

		glui_result->add_button( "Back to options", OPTIONS_MODE, EnterMode );
		glui_result->add_column( false );

		prec_text = glui_result->add_statictext( prec_string );
		recall_text = glui_result->add_statictext( recall_string );
		truth_text = glui_result->add_statictext( truth_string );
		range_text = glui_result->add_statictext( range_string );
		movie_text = glui_result->add_statictext( movie_string );
		log_text = glui_result->add_statictext( log_string );

	} else {
		//update the window with the new results

		prec_text->set_text( prec_string );
		recall_text->set_text( recall_string );
		truth_text->set_text( truth_string );
		range_text->set_text( range_string );
		movie_text->set_text( movie_string );
		log_text->set_text( log_string );

		glui_result->show();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RunColor( void )
{
	int i;
	int j;
	long total=0;
	CutNode cut;
	float sigma;

	int num=0;

	static int sumX = 0;
	static int sumX2 = 0;
	static int sqrd[ 10 ];

/*
	for( i=0; i<320*240; i++ )
	{
		// calculate the difference between the two frames...
		total +=  Abs( colorPrev.buffer[i] - cPtr.buffer[i] );
	}
*/

//#define		Y_SKIP		100
//#define		X_SKIP		100

	for( i=5; i<105; i++ )
	{
		for( j=5; j<105; j++ )
		{
			total+= Abs( colorPrev.image[i][j] - cPtr.image[i][j] );
			num++;
		}
		for( j=215; j<315; j++ )
		{
			total+= Abs( colorPrev.image[i][j] - cPtr.image[i][j] );
			num++;
		}
	}
	for( i=70; i<170; i++ ){
		for( j=110; j<210; j++ ){
			total+= Abs( colorPrev.image[i][j] - cPtr.image[i][j] );
			num++;
		}
	}
	for( i=135; i<235; i++ )
	{
		for( j=5; j<105; j++ )
		{
			total+= Abs( colorPrev.image[i][j] - cPtr.image[i][j] );
			num++;
		}
		for( j=215; j<315; j++ )
		{
			total+= Abs( colorPrev.image[i][j] - cPtr.image[i][j] );
			num++;
		}
	}


	//colorHistory[ colorIndex ] = 
	colorAvg = (int)( ((float)total) / (float)(3*num) );

/*
	if( colorIndex == 0 ){
		sumX = colorAvg;
		sumX2 = sqrd[ colorIndex ] = colorAvg*colorAvg;
	} else if( colorIndex < 10 ){
		sumX += colorAvg;
		sumX2 += sqrd[ colorIndex ] = colorAvg*colorAvg;
	} else {
		sigma =  Abs(((float)sumX2) - ((float)(sumX*sumX)/10.0f) )/3.0f;
		if(  Abs( ((float)sumX)/10.0f - ((float)colorAvg)) > sigma ){
			cut.frameNum = mpegData.cFrame-1;
			cut.color_diff = colorAvg;
			cut.color_thresh = colorThreshold;
			cut.motion_diff = -1;
			cut.motion_thresh = -1;
			
			db.AddCut( cut );
		}

		// adjust for the new values...

		sumX += colorAvg - colorHistory[ colorIndex-10 ];
		sumX2 += colorAvg*colorAvg - sqrd[ colorIndex%10 ];
		sqrd[ colorIndex%10 ] = colorAvg*colorAvg;
	}

	colorIndex++;
*/
	
	if( (int)(colorAvg+0.5f) > colorThreshold ){
		cut.frameNum = mpegData.cFrame-1;
		cut.color_diff = colorAvg;
		cut.color_thresh = colorThreshold;
		cut.motion_diff = -1;
		cut.motion_thresh = -1;
		db.AddCut( cut );
	}

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RunMotion( void )
{
	int i, j;
	int x, y;
	int mx, my;
	long value;
/*
	for( i=blockRange; i<(240-blockRange)/blockSize; i++)
	{
		for( j=blockRange; j<(320-blockRange)/blockSize; j++ )
		{
			mx = my = 0;
			for( y=i-blockRange; y<i+blockRange; y++ )
			{
				for( x=j-blockRange; x<j+blockRange; x++ )
				{
					long value = MapBlock( j, i, x, y );
					if( value>topValue ){
						
				}
			}
		}
	}
*/

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline int MapBlock( int origX, int origY, int destX, int destY )
{

	return 0;
}


/////////////////////////////////////////////////////////////////////
//   Init() - Initialize the window, menus, etc.
/////////////////////////////////////////////////////////////////////
void Init( void )
{

	// initilize the global data
	SetOptions( DEBUG_MODE );

	mpegData.info = NULL;
	mpegData.frame = NULL;

	mpegWin.SetData( &mpegData );

	Init_GLUI();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void Init_GLUI( void )
{
	int i;

	// options for the color difference part...
	GLUI_Rollout		*color_rollout;
	GLUI_EditText		*color_thresh;
	GLUI_EditText		*color_step;
	GLUI_Checkbox		*color_hist;

	// options for the motion vector part....
	GLUI_Rollout		*motion_rollout;
	GLUI_EditText		*motion_thresh;
	GLUI_EditText		*motion_step;
	GLUI_Checkbox		*motion_hist;

	// Overall options....
	GLUI_Panel		*play_panel;
	GLUI_Listbox		*movie_list;
	GLUI_Button		*preview_button;
	GLUI_EditText		*start_edit;
	GLUI_EditText		*end_edit;
	GLUI_EditText		*skip_edit;
	GLUI_Button		*play_button;

	GLUI_Panel		*run_panel;
	GLUI_RadioGroup		*run_group;
	GLUI_RadioButton 	*run_color;
	GLUI_RadioButton	*run_motion;
	GLUI_RadioButton	*run_both;
	

	// log file stuff
	GLUI_Rollout		*log_rollout;
	GLUI_Checkbox		*log_check;
	GLUI_EditText		*log_edit;

	GLUI_Checkbox		*watch_check;

	GLUI_Button		*quit_button;

	// Set things up.....

	glui = GLUI_Master.create_glui( "MPEG Cut Detection" );
	glui->hide();
	
	glui->add_statictext( "CSE 486  Semeseter Project" );
	glui->add_separator();

	// setup the color rollout ---------------------------------------
	color_rollout = glui->add_rollout( "Color Differenc Options", true );
	color_thresh = glui->add_edittext_to_panel( color_rollout, 
						"Threshold:",
						GLUI_EDITTEXT_INT,
						&colorThreshold );

	color_step = glui->add_edittext_to_panel( color_rollout, 
						"Algorithm Step:", 
						GLUI_EDITTEXT_INT, 
						&colorStep );

	color_hist = glui->add_checkbox_to_panel( color_rollout, 
						"Show frame difference graph", 
						&showColorHist );



	// setup the motion rollout --------------------------------------

	motion_rollout = glui->add_rollout( "Motion Vector Options", true );
	motion_thresh = glui->add_edittext_to_panel( motion_rollout,
						"Threshold:",
						GLUI_EDITTEXT_INT,
						&motionThreshold );

	motion_step = glui->add_edittext_to_panel( motion_rollout,
						"Frame skip:",
						GLUI_EDITTEXT_INT,
						&motionStep );

	motion_hist = glui->add_checkbox_to_panel( motion_rollout,
						"Show frame difference graph",
						&showMotionHist );


	log_rollout = glui->add_rollout( "Log Options", true );

	log_check = glui->add_checkbox_to_panel( log_rollout,
						"Write results to file",
						&useLog );

	log_edit = glui->add_edittext_to_panel( log_rollout,
						"Filename:",
						GLUI_EDITTEXT_TEXT,
						logFileName );

	glui->add_column( false );

	// setup the play panel -------------------------------------------
	play_panel = glui->add_panel( "Global Options" );

	movie_list = glui->add_listbox_to_panel( play_panel,
						"Movie:",
						&movieIndex );

	for( i=0; i<numFiles; i++ ){
		movie_list->add_item( i, movieFiles[i] );
	}

	preview_button = glui->add_button_to_panel( play_panel, 
							"Preview",
							PREVIEW_MOVIE, 
							Control_CB );

	start_edit = glui->add_edittext_to_panel( play_panel,
							"Start Frame:",
							GLUI_EDITTEXT_INT,
							&runStart );

	end_edit = glui->add_edittext_to_panel( play_panel,
							"End Frame:",
							GLUI_EDITTEXT_INT,
							&runEnd );

	skip_edit = glui->add_edittext_to_panel( play_panel,
							"Skip n frames; n:",
							GLUI_EDITTEXT_INT,
							&frameStep );

	run_panel = glui->add_panel_to_panel( play_panel, "Run..." );
	run_group = glui->add_radiogroup_to_panel( run_panel, &runType );
	
	run_color = glui->add_radiobutton_to_group( run_group, "Color difference only" );
	run_motion = glui->add_radiobutton_to_group( run_group, "Motion method only" );
	run_both = glui->add_radiobutton_to_group( run_group, "Both methods" );

	watch_check = glui->add_checkbox_to_panel( play_panel, "Show movie while working", &watchMovie );

	play_button = glui->add_button_to_panel( play_panel, "Run", CALC_MODE, EnterMode );	

	// Lets not forget a way to quit this lovely program (but why would you want to....)
	quit_button = glui->add_button( "Quit", QUIT, Control_CB );

	glui->show();
}


/////////////////////////////////////////////////////////////////////
//  Cleanup the textures (or anything else)
/////////////////////////////////////////////////////////////////////
void CleanUp( void )
{
	GLUI_Master.close_all();
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
