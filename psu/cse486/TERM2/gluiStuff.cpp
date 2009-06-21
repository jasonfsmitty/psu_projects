/*----------------------------------------------------------------+
	gluiStuff.cpp

	Jason F Smith

	GLUI initialization, and other stuff.

 +----------------------------------------------------------------*/

#include "glui.h"
#include <GL/glut.h>
#include <stdlib.h>

#include "cse486_mpeglib-cpp.h"
#include "glui.h"
#include "mpeg.h"
#include "defines.h"


////////////////////////////////////////////////////////////////////
//	Declaration of all of the data for the GLUI stuff
//	Some of these will need to be global, just need to 
//	  add an extern where it's needed.
////////////////////////////////////////////////////////////////////
GLUI		*glui=NULL;

GLUI_Rollout	*movie_panel;
GLUI_Listbox	*movie_list;
GLUI_EditText	*movie_Xedit;
GLUI_EditText	*movie_Yedit;
GLUI_Checkbox	*play_check;
GLUI_EditText	*movieSkip_edit;
GLUI_EditText	*start_edit;
GLUI_EditText	*end_edit;
GLUI_Button	*preview_button;

GLUI_Rollout	*color_panel;
GLUI_Listbox	*color_list;
GLUI_Listbox	*colorDiff_list;
GLUI_EditText	*color_edit;

GLUI_Rollout	*motion_panel;
GLUI_EditText	*block_edit;
GLUI_EditText	*range_edit;
GLUI_Listbox	*motionDiff_list;
GLUI_EditText	*same_edit;
GLUI_EditText	*motion_edit;

GLUI_Panel	*run_panel;
GLUI_Listbox	*run_list;
GLUI_EditText	*run_edit;
GLUI_EditText	*cut_edit;
GLUI_Checkbox	*hist_check;
GLUI_Checkbox	*log_check;
GLUI_EditText	*log_edit;
GLUI_Button	*run_button;

GLUI_Button	*quit_button;


// Stuff for the results window....
GLUI		*result_glui=NULL;

GLUI_StaticText *movie_result;
GLUI_StaticText *range_result;
GLUI_StaticText *prec_result;
GLUI_StaticText *recall_result;
GLUI_StaticText *log_result;
GLUI_StaticText *truth_result;
GLUI_StaticText *report_result;
GLUI_StaticText *fa_result;
GLUI_StaticText *missed_result;


////////////////////////////////////////////////////////////////////
//	Initialize the GLUI interface.
////////////////////////////////////////////////////////////////////
void Init_GLUI( void )
{
	// Get the main GLUI object
	glui = GLUI_Master.create_glui( "MEPG Cut Detection" );

	///////////////////////////////////////////////////////////////////////
	//  Set up the movie options
	movie_panel = glui->add_rollout( "Movie Options", false );

	movie_list = glui->add_listbox_to_panel( movie_panel, 
						"Movie file:",
						&movieIndex );

	for( int i=0; i<4; i++ ){
		movie_list->add_item( i, movieFiles[i] );
	}

	movie_Xedit = glui->add_edittext_to_panel( movie_panel,
						"Width:",
						GLUI_EDITTEXT_INT,
						&movieWidth );

	movie_Yedit = glui->add_edittext_to_panel( movie_panel,
						"Height:",
						GLUI_EDITTEXT_INT,
						&movieHeight );

	play_check = glui->add_checkbox_to_panel( movie_panel, 
						"Show movie during detection",
						&watchMovie );

	movieSkip_edit = glui->add_edittext_to_panel( movie_panel,
							"Frame step:",
							GLUI_EDITTEXT_INT,
							&frameSkip );

	start_edit = glui->add_edittext_to_panel( movie_panel,
						"Start frame:",
						GLUI_EDITTEXT_INT,
						&startFrame );

	end_edit = glui->add_edittext_to_panel( movie_panel,
						"End frame:",
						GLUI_EDITTEXT_INT,
						&endFrame );

	preview_button = glui->add_button_to_panel( movie_panel,
						"Preview", PREVIEW_MODE, EnterMode );


	//////////////////////////////////////////////////////////////////////
	//  Setup the color histogram interface panel

	color_panel = glui->add_rollout( "Color Options", false );

	color_list = glui->add_listbox_to_panel( color_panel, "Color space:", &colorSpace );
	color_list->add_item( RGB_COLOR_SPACE, "RGB" );
	color_list->add_item( LUV_COLOR_SPACE, "L*u*v*" );

	colorDiff_list = glui->add_listbox_to_panel( color_panel, "Diff calc:", &colorDiff );
	colorDiff_list->add_item( ABS_DIFF, "Abs(a-a)" );
	//colorDiff_list->add_item( SQUARED_DIFF, "(a-a)^2" );

	color_edit = glui->add_edittext_to_panel( color_panel, "Threshold:",
							GLUI_EDITTEXT_INT, &colorThreshold );


	///////////////////////////////////////////////////////////////////////
	//	Setup the motion vectro interface.

	motion_panel = glui->add_rollout( "Motion Vector Options", false );
	block_edit = glui->add_edittext_to_panel( motion_panel,
						"Block size:",
						GLUI_EDITTEXT_INT,
						&blockSize );

	range_edit = glui->add_edittext_to_panel( motion_panel,
						"Block range:",
						GLUI_EDITTEXT_INT,
						&blockRange );

	motionDiff_list = glui->add_listbox_to_panel( motion_panel, "Diff calc:", &motionDiff );
	motionDiff_list->add_item( ABS_DIFF, "Abs(a-a)" );
	//motionDiff_list->add_item( SQUARED_DIFF, "(r-r)^2" );

	same_edit = glui->add_edittext_to_panel( motion_panel, "Same block thresh:",
						GLUI_EDITTEXT_INT,
						&sameThreshold );

	motion_edit = glui->add_edittext_to_panel( motion_panel, "Cut thresh:",
							GLUI_EDITTEXT_INT,
							&motionThreshold );

	///////////////////////////////////////////////////////////////////////
	//	Add a panel to control the actual running of the program

	run_panel = glui->add_panel( "Run Options" );

	run_list = glui->add_listbox_to_panel( run_panel, "Detection 
method:",
							&runType );

	run_list->add_item( COLOR_METHOD, "Color only" );
	run_list->add_item( MOTION_METHOD, "Motion only" );
	run_list->add_item( BOTH_METHODS, "Both methods" );

	run_edit = glui->add_edittext_to_panel( run_panel,
						"Algorithm step:",
						GLUI_EDITTEXT_INT,
						&runStep );

	cut_edit = glui->add_edittext_to_panel( run_panel,
						"Cut window:",
						GLUI_EDITTEXT_INT,
						&cutWindow );

	hist_check = glui->add_checkbox_to_panel( run_panel,
						"Show frame difference plot",
						&showPlot );

	log_check = glui->add_checkbox_to_panel( run_panel,
						"Write results to log",
						&useLog );

	log_edit = glui->add_edittext_to_panel( run_panel,
						"Log filename:",
						GLUI_EDITTEXT_TEXT,
						logFile );

	run_button = glui->add_button_to_panel( run_panel, "Run",
						CALC_MODE, EnterMode );


	///////////////////////////////////////////////////////////////////
	quit_button = glui->add_button( "Quit", QUIT, Control_CB );

}

////////////////////////////////////////////////////////////////////
//	Destroy the GLUI interface.
////////////////////////////////////////////////////////////////////
void Clean_GLUI( void )
{
	GLUI_Master.close_all();
}


////////////////////////////////////////////////////////////////////
//	Callback function for the main GLUI interface.
////////////////////////////////////////////////////////////////////
void Control_CB( int value )
{
	switch( value )
	{

		case QUIT:
			CleanUp();
			exit(0);
			break;	// not needed


	};
}



////////////////////////////////////////////////////////////////////
//	Opens a window to display the results of the cut
//	  detection algorithms.
////////////////////////////////////////////////////////////////////
void OpenResults( void )
{
	char movieString[ 20 ];
	char rangeString[ 20 ];
	char precisionString[ 20 ];
	char recallString[ 20 ];
	char logString[ 20 ];
	char truthString[ 20 ];
	char reportString[ 20 ];
	char faString[ 20 ];
	char missedString[ 20 ];

	// get strings ready with the results

	strcpy( movieString, movieFiles[ movieIndex ] );
	sprintf( rangeString, "%d - %d", startFrame, endFrame );
	sprintf( precisionString, "%3.2f%", (precision*100.0f) );
	sprintf( recallString, "%3.2f%", (recall*100.0f) );
	strcpy( logString, logFile );
	sprintf( truthString, "%d", numTruths );
	sprintf( reportString, "%d", numDetected );
	sprintf( faString, "%d", numFalse );
	sprintf( missedString, "%d", numMissed );


	// actually use set the static text with the results...

	if( result_glui==NULL )
	{
		// open the window for the first time....
		result_glui = GLUI_Master.create_glui( "Results" );

		result_glui->add_statictext( "Movie file:" );
		result_glui->add_statictext( "Frame range:" );
		result_glui->add_statictext( "Precision:" );
		result_glui->add_statictext( "Recall:" );
		result_glui->add_statictext( "Log file:" );
		
		result_glui->add_separator();

		result_glui->add_statictext( "Num truths:" );
		result_glui->add_statictext( "Num detected:" );
		result_glui->add_statictext( "Num false alarms:" );
		result_glui->add_statictext( "Num missed:" );

		result_glui->add_button( "Done", OPTIONS_MODE, EnterMode );
		result_glui->add_column( false );

		movie_result = result_glui->add_statictext( movieString );
		range_result = result_glui->add_statictext( rangeString );
		prec_result = result_glui->add_statictext( precisionString );
		recall_result = result_glui->add_statictext( recallString );
		log_result = result_glui->add_statictext( logString );

		result_glui->add_separator();

		truth_result = result_glui->add_statictext( truthString );
		report_result = result_glui->add_statictext( reportString );
		fa_result = result_glui->add_statictext( faString );
		missed_result = result_glui->add_statictext( missedString );

	} else {

		movie_result->set_text( movieString );
		range_result->set_text( rangeString );
		prec_result->set_text( precisionString );
		recall_result->set_text( recallString );
		log_result->set_text( logString );

		truth_result->set_text( truthString );
		report_result->set_text( reportString );
		fa_result->set_text( faString );
		missed_result->set_text( missedString );

	}

	result_glui->show();
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void CloseResults( void )
{
	if( result_glui!=NULL )
		result_glui->hide();
}

