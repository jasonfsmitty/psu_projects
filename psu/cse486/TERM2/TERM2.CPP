/*-----------------------------------------------------------------+
	term2.cpp


	Jason F Smith

	A re-write of my semeseter project for CSE 486.
	I rewrote it because I was not getting the results
	that I wanted.

 +-----------------------------------------------------------------*/

#include "glui.h"
#include <GL/glut.h>

#include <stdlib.h>
#include <fstream.h>
#include <math.h>

#define	DEBUG

#include "mpeg.h"
#include "cse486_mpeglib-cpp.h"
#include "defines.h"
#include "linklist2.h"
#include "histogram.h"
#include "linklist2.h"


int		movieIndex = 0;
int		movieWidth = 300;
int		movieHeight = 220;
int		watchMovie = TRUE;
int		frameSkip = 10;
int		startFrame = 0;
int		endFrame= -1;
int		colorSpace = RGB_COLOR;
int		colorDiff = ABS_DIFF;
int		colorThreshold = 20;
int		blockSize = 8;
int		blockRange = 8;
int		motionDiff = ABS_DIFF;
int		sameThreshold = 128;
int		motionThreshold = 128;
int		runType = COLOR_METHOD;
int		runStep = 1;
int		showPlot = TRUE;
int		useLog = TRUE;
char		logFile[ sizeof(GLUI_String) ] = "log.txt";

int		skipX = (320-movieWidth)/2;
int		skipY = (240-movieHeight)/2;

MPEG_Data	mpegData;

HistWin		histWin;
IntQueue	colorQ;
IntQueue	motionQ;

int		cutWindow = 1;

float		precision;
float		recall;
int		numTruths;
int		numReported;
int		numDetected;
int		numFalse;
int		numMissed;

// Movie files.....
char* 	movieFiles[4] = { 
	"a.mpg",
	"b.mpg",
	"c.mpg",
	"d.mpg"
};

char* 	truthFiles[4] = {
	"a.gtf",
	"b.gtf",
	"c.gtf",
	"d.gtf"
};

enum {
	CUT_CUT=1,
	GRAD_CUT,
	BLACK_CUT
};

char* cutDesc[3] = { 
	"Normal cut",
	"Gradual transition",
	"Cut to black"
};

struct TruthCut {
	int type;
	int frame;
	int frame2;
	bool found;
};

struct CutData{
	int detected;
	int missed;
};

CutData	gradCut, cutCut, blackCut;

int			lastCutIndex;
LinkList<int>		cutList;
LinkList<TruthCut>	truthList;

ofstream	logOut;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CalcIdle( void )
{
	int nextFrame=7800;

	if( (endFrame>0 && mpegData.cFrame>=endFrame)
			|| (mpegData.info==NULL)
			|| ((mpegData.frame=mpegGetNextFrame( mpegData.info ))==NULL)
			|| (!IsOpenMPEG() && !histWin.IsOpen() )
		)
	{
		EnterMode( RESULT_MODE );
		return;
	}

	mpegData.cFrame++;
	nextFrame = runStep;

	if( runType!=MOTION_METHOD )
	{
		//cout <<"Running color\n";
		RunColor();
		// enter the difference into the colorQ

		//cout <<"Pushing onto the color queue\n";
		colorQ.Push( colorHistory[historyIndex] );
	}

	if( runType!=COLOR_METHOD )
	{
		RunMotion();
		// enter the result into the motionQ
		motionQ.Push( motionHistory[historyIndex] );
	}


	// Check for a cut
	//cout <<"Checking for a cut\n";
	CheckForCut();

	historyIndex++;

	if( IsOpenMPEG() )
	{
		//cout <<"Updating mpeg video display\n";
		MPEG_Display();
	}

	if( histWin.IsOpen() )
	{
		//cout <<"Updating plot window display\n";
		histWin.Display();
	}

	// destroy the old frame
	mpegDestroyFrame( mpegData.frame );
	mpegData.frame = NULL;

	// if need be, search to the next frame
	if( nextFrame!=7800 && nextFrame>1 && mpegData.info!=NULL )
	{
		mpegData.cFrame+=nextFrame-1;
		//cout <<"loading frame: "<<mpegData.cFrame<<"\n";
		if( mpegGotoFrame( mpegData.info, mpegData.cFrame )<0 ){
			EnterMode( RESULT_MODE );
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void EnterMode( int mode )
{
	// do general cleanup stuff here...
	GLUI_Master.set_glutIdleFunc( NULL );

	// make sure the windows are closed...
	histWin.Close();
	CloseMyMPEG();
	CloseResults();

	if( mpegData.frame != NULL )
	{
		mpegDestroyFrame( mpegData.frame );
		mpegData.frame = NULL;
	}

	if( mpegData.info != NULL )
	{
		mpegClose( mpegData.info );
		mpegData.info=NULL;
	}


	// specific stuff here
	switch( mode )
	{
		case OPTIONS_MODE:
			GLUI_Master.sync_live_all();
			glui->show();
			break;


		case RESULTS_MODE:
			endFrame = mpegData.cFrame;
			CalcStats();
			OpenResults();
			break;


		case PREVIEW_MODE:
			cout <<"Preview mode not implemented (yet)\n";
			EnterMode( OPTIONS_MODE );
			break;


		case CALC_MODE:
			glui->hide();

			historyIndex = 0;

			if( frameSkip<=0 )
				frameSkip=1;

			startFrame = mpegData.cFrame = Max( startFrame, 0 );
			colorQ.Reset();
			motionQ.Reset();
			cutList.DeleteAll();
			truthList.DeleteAll();

			skipX = (320-movieWidth)/2;
			skipY = (240-movieHeight)/2;

			//cout <<"SkipX: "<<skipX<<"\nSkipY: "<<skipY<<"\n";

			if( skipX<0 )	skipX = 0;
			if( skipY<0 )	skipY = 0;
			if( skipX>150 ) skipX = 150;
			if( skipY>110 ) skipY = 110;

			lastCutIndex=-100;

			if( cutWindow<=0 )
				cutWindow=1;


			// open the mpeg...
			mpegData.info = mpegOpen( movieFiles[ movieIndex ] );

			// test if opened correctly...
			if( mpegData.info==NULL )
			{
				//cout <<"Could not open movie!\n";
				EnterMode( OPTIONS_MODE );
				return;
			}

			// go to the correct frame...
			if( mpegGotoFrame( mpegData.info, startFrame )<0 )
			{
				//cout <<"Could not find start frame: "<<startFrame<<"\n";
				EnterMode( OPTIONS_MODE );
			}


			if( showPlot )
			{
				//cout <<"Opening plot window\n";

				if( runType!=MOTION_METHOD ){
					histWin.Setup_1( &colorQ, colorThreshold, runStep );
				} else {
					histWin.Setup_1( NULL, 0, 0 );
				}

				if( runType!=COLOR_METHOD ){
					histWin.Setup_2( &motionQ, motionThreshold, runStep );
				} else {
					histWin.Setup_2( NULL, 0, 0 );
				}

				histWin.Open();
			}

			if( watchMovie )
			{
				//cout <<"Opening MPEG player\n";
				OpenMyMPEG();
			}

			GLUI_Master.set_glutIdleFunc( CalcIdle );
			break;

	};
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HIST_Display( void )
{
	if( histWin.IsOpen() )
		histWin.Display();
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
void CheckForCut( void )
{
	if( runType==COLOR_METHOD )
	{
		if( colorHistory[ historyIndex ] > colorThreshold )
		{
			// add the cut
			if( lastCutIndex+1 < historyIndex )
			{
				// there was not a cut the previous
				// frame, so we can add to the list...
				cutList.InsertAtTail( (historyIndex*runStep)+startFrame);
#if defined( DEBUG )
				cout <<"c added cut at "<<(historyIndex*runStep)+startFrame<<"\n";
#endif // DEBUG
			}
#if defined( DEBUG )
			else {
				cout <<"c ignoring cut at "<<(historyIndex*runStep)+startFrame<<"\n";
			}
#endif // DEBUG

			lastCutIndex = historyIndex;
		}
	}
	else if( runType==MOTION_METHOD )
	{
		if( motionHistory[ historyIndex ] > motionThreshold )
		{
			// add the cut
			if( lastCutIndex+1 < historyIndex )
			{
				// there was not a cut the previous
				// frame, so we can add to the list...
				cutList.InsertAtTail( (historyIndex*runStep)+startFrame );
#if defined( DEBUG )
				cout <<"m added cut at "<<(historyIndex*runStep)+startFrame<<"\n";
#endif // DEBUG

			}
#if defined( DEBUG )
			else {
				cout <<"m ignoring cut at "<<(historyIndex*runStep)+startFrame<<"\n";
			}
#endif // DEBUG

			lastCutIndex = historyIndex;
		}
	} 
	else // both methods
	{
		if( colorHistory[ historyIndex ] > colorThreshold 
		    &&  motionHistory[ historyIndex ] > motionThreshold )
		{
			// add the cut
			if( lastCutIndex+1 < historyIndex )
			{
				// there was not a cut the previous
				// frame, so we can add to the list...
				cutList.InsertAtTail( (historyIndex*runStep)+startFrame );
#if defined( DEBUG )
				cout <<"b added cut at "<<(historyIndex*runStep)+startFrame<<"\n";
#endif // DEBUG

			}
#if defined( DEBUG )
			else {
				cout <<"b ignoring cut at "<<(historyIndex*runStep)+startFrame<<"\n";
			}
#endif // DEBUG

			lastCutIndex = historyIndex;
		}
	}

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CalcStats( void )
{
#if defined( DEBUG )
#endif // DEBUG

	int i, j;
	bool cutFound;

	cout <<"Loading truth file...\n";
	LoadTruth();

	if( useLog ){
		cout <<"Starting log: "<<logFile<<"\n";
		StartLog();
	}

	gradCut.missed = gradCut.detected = 0;
	cutCut.detected = cutCut.missed = 0;
	blackCut.detected = blackCut.missed  = 0;

	cout <<"Calculating stats...\n";

	numTruths = truthList.GetSize();

	for( i=0; i<numTruths; i++ )
	{
		truthList[i].found = false;
	}

	numReported = cutList.GetSize();
	numDetected = 0;
	numFalse = 0;
	numMissed = 0;

	for( i=0; i<numReported; i++ )
	{
		cutFound=false;


		for( j=0; j<numTruths && !cutFound; j++ )
		{
			switch( truthList[j].type ){

				case CUT_CUT:
					if(  Abs( cutList[i] - truthList[j].frame ) <= cutWindow ){
						cutFound=true;
						
						if( !truthList[j].found ){
							truthList[j].found=true;
							numDetected++;
							cutCut.detected++;

							if( useLog ){
								logOut <<" +  Truth cut at "
									<<truthList[j].frame
									<<" detected at "
									<<cutList[i]<<"\n";
							}
						} else if( useLog )
						{
							logOut <<" - Cut detected at "<<cutList[i]
								<<" ignored;  Truth at "
								<<truthList[j].frame
								<<" was already detected.\n";
						}
					}
					break;


				case BLACK_CUT:
					if(  Abs( cutList[i] - truthList[j].frame ) <= cutWindow ){
						cutFound=true;
						
						if( !truthList[j].found ){
							truthList[j].found=true;
							numDetected++;
							blackCut.detected++;

							if( useLog ){
								logOut <<" + Truth cut-to-black at "
									<<truthList[j].frame
									<<" detected at "
									<<cutList[i]<<"\n";
							}
						} else if( useLog )
						{
							logOut <<" - Cut detected at "<<cutList[i]
								<<" ignored;  Truth at "
								<<truthList[j].frame
								<<" was already detected.\n";
						}
					}
					break;

				case GRAD_CUT:
					if( (cutList[i]>= truthList[j].frame-cutWindow) &&
						(cutList[i] <= truthList[j].frame2+cutWindow) )
					{
						cutFound=true;

						if( !truthList[j].found ){
							truthList[j].found=true;
							numDetected++;
							gradCut.detected++;

							if( useLog ){
								logOut <<" + Truth gradual transition at "
									<<truthList[j].frame<<"-"
									<<truthList[j].frame2
									<<" detected at "
									<<cutList[i]<<"\n";
							}
						} else if( useLog )
						{
							logOut <<" - Cut reported at "<<cutList[i]
								<<" ignored:  Truth gradual transition at "
								<<truthList[j].frame<<"-"
								<<truthList[j].frame2<<" was already detected.\n";
						}

					}
					break;
			}; // switch
		} // for all truths

		if( !cutFound ){
			numFalse++;
			if( useLog )
			{
				logOut <<" * False alarm detected at "<<cutList[i]<<"\n";
			}
		}
	} // for all reported cuts

	for( j=0; j<numTruths; j++ )
	{
		if( !truthList[j].found ){
			numMissed++;

			switch( truthList[j].type ){
				case CUT_CUT:
					cutCut.missed++;
					break;

				case BLACK_CUT:
					blackCut.missed++;
					break;

				case GRAD_CUT:
					gradCut.missed++;
					break;
			};

			if( useLog )
			{
				if( truthList[j].type == CUT_CUT )
					logOut <<" @ Missed normal cut at "<<truthList[j].frame<<"\n";
				else if( truthList[j].type==BLACK_CUT )
					logOut <<" @ Missed cut-to-black at "<<truthList[j].frame<<"\n";
				else
					logOut <<" @ Missed gradual transition from "<<truthList[j].frame<<"-"<<truthList[j].frame2<<"\n";
			}
		}
	}

	if( numDetected+numFalse == 0 ){
		precision = 1.0f;
	} else {
		precision = ((float)numDetected)/((float)(numDetected+numFalse));
	}

	if( numDetected+numMissed == 0 ){
		recall = 1.0f;
	} else {
		recall = ((float)numDetected)/ ((float)(numDetected+numMissed));
	}

	if( useLog ){
		cout <<"Closing log...\n";
		EndLog();
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void LoadTruth( void )
{
	char line[512];
	ifstream in;
	int i;
	TruthCut add;
	int loop=500;

	truthList.DeleteAll();

	in.open( truthFiles[ movieIndex ] );

	if( in.fail() ){
		cout <<"Error loading the truth file.\n";
		in.close();
		return;
	}

	while( !in.eof() )//&& (--loop>0) )
	{
		in.getline( line, 500, '\n' );

		if( strlen(line)>0 && line[0]!='!' )
		{

		add.frame=0;
		i=0;

		add.frame = GetFrame( line );

		if( (add.frame>=startFrame) && (add.frame<=endFrame) )
		{
			if( strstr( line, "gradtrans" )!=NULL ||
				strstr( line, "GRADTRANS" )!=NULL )
			{
				i=0;

				while( line[i++] != '=' );
				i++;

				add.frame2 = GetFrame( (char*)(line+i) );
				add.type = GRAD_CUT;

				//cout <<"Loaded gradual transition: "<<add.frame<<"-"<<add.frame2<<"\n";
				//cout <<"*"<<(line+i)<<"\n";

				truthList.InsertAtTail( add );
			} 
			else if( strstr( line, "cuttoblack" )!=NULL ||
				strstr( line, "CUTTOBLACK" )!=NULL )
			{
				//cout <<"Loaded cuttoblack: "<<add.frame<<"\n";

				// add in the cut to black
				add.type = BLACK_CUT;
				truthList.InsertAtTail( add );
			}
			else if( strstr( line, "cut" )!=NULL ||
				strstr( line, "CUT" )!=NULL )
			{
				//cout <<"Loaded normal cut: "<<add.frame<<"\n";

				// add in normal cut
				add.type = CUT_CUT;
				truthList.InsertAtTail( add );
			}
		}// end if in range

		}
	}

	in.close();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void StartLog( void )
{
	int i;

	logOut.open( logFile );

	if( logOut.fail() )
	{
		cout <<"Could not open log file, no log will be written.\n";
		logOut.close();
		useLog=FALSE;
		return;
	}

	logOut	<<"Jason F Smith  Group #19\n"
		<<"CSE 486 Semester Project\n"
		<<"\n"
		<<"Cut change algrothm log\n"
		<<"\n\n"
		<<"[ SETTINGS ]\n\n"
		<<"  movie: "<<movieFiles[ movieIndex ]<<"\n"
		<<"  width: "<<movieWidth<<"\t\tskipX: "<<skipX<<"\n"
		<<"  height: "<<movieHeight<<"\t\tskipY: "<<skipY<<"\n"
		<<"  Show movie: "<<(watchMovie ? "Yes\n" : "No\n")
		<<"  Movie step: "<<frameSkip<<"\n"
		<<"\n"
		<<"  Start frame: "<<startFrame<<"\n"
		<<"  End frame: "<<endFrame<<"\n"
		<<"  Algorithm Step: "<<runStep<<"\n\n
"
		<<"  Color method: "<<(runType!=MOTION_METHOD ? "On\n" : "Off\n")
		<<"    Color threshold: "<<colorThreshold<<"\n"
		<<"    Color space: "<<(colorSpace==RGB_COLOR_SPACE ? "RGB\n" : "L*u*v*\n")
		<<"    Color difference calculations: "
			<<(colorDiff==SQUARED_DIFF ? "Squared difference [ (x-x)^2 + (y-y)^2 + (z-z)^2]\n" : 
					"Absolute differences [ Abs(x-x) + Abs(y-y) + Abs(z-z) ]\n" )
		<<"\n  Motion method: "<<(runType!=COLOR_METHOD ? "On\n" : "Off\n")
		<<"    Motion theshold: "<<motionThreshold<<"\n"
		<<"    Same block threshold: "<<sameThreshold<<"\n"
		<<"    Motion difference calculations: "
			<<(motionDiff==SQUARED_DIFF ? "Squared difference [ (x-x)^2 + (y-y)^2 + (z-z)^2]\n" : 
					"Absolute differences [ Abs(x-x) + Abs(y-y) + Abs(z-z) ]\n")
		<<"\n"
		<<"  Cut Margin: "<<cutWindow<<"\n"
		<<"\n"
		<<"[TRUTH CUTS]\n";

	for( i=0; i<truthList.GetSize(); i++ )
	{
		switch( truthList[i].type )
		{
			case CUT_CUT:
				logOut <<"  "<<truthList[i].frame<<" normal cut\n";
				break;

			case BLACK_CUT:
				logOut <<"  "<<truthList[i].frame<<" cut to black\n";
				break;

			case GRAD_CUT:
				logOut <<"  "<<truthList[i].frame<<" - "<<truthList[i].frame2
					<<" gradual transition\n";
		}
	}

	logOut	<<"\n[ REPORTED CUTS ]\n\n";

	for( i=0; i<cutList.GetSize(); i++ )
	{
		logOut <<" "<<cutList[i]<<"\n";
	}

	logOut <<"\n[ FILTERING ]\n\n";
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void EndLog( void )
{

	logOut	<<"\n[ RESULTS ]\n\n"
		<<"  Total truths: "<<numTruths<<"\n"
		<<"  Total reported: "<<numReported<<"\n"
		<<"  Total cuts detected: "<<numDetected<<"\n"
		<<"\n"
		<<"  Precision: "<<precision<<"\n"
		<<"    Number of false alarms: "<<numFalse<<"\n"
		<<"  Recall: "<<recall<<"\n"
		<<"    Number missed: "<<numMissed
		<<"\n\n"
		<<"  # normal cuts detected: "<<cutCut.detected<<"\n"
		<<"  # normal cuts missed: "<<cutCut.missed<<"\n"
		<<"\n"
		<<"  # cut-to-black cuts detected: "<<blackCut.detected<<"\n"
		<<"  # cut-to-black cuts missed: "<<blackCut.missed<<"\n"
		<<"\n"
		<<"  # gradual transitions detected: "<<gradCut.detected<<"\n"
		<<"  # gradual transitions missed: "<<gradCut.missed<<"\n"

		<<"\n";

	logOut.close();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int GetFrame( char* line )
{
	int i=0;
	int val=0;

	while( line[i]>='0' && line[i]<='9' ){
		val = val*10 + (int)( line[i] - '0' );
		i++;
	}

	return val;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CleanUp( void )
{
	Clean_GLUI();

	if( mpegData.frame )
	{
		mpegDestroyFrame( mpegData.frame );
	}

	if( mpegData.info )
	{
		mpegClose( mpegData.info );
	}

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Init( void )
{
	GLUI_Master.set_glutIdleFunc( NULL );
	Init_GLUI();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv )
{
	glutInit( &argc, argv );

	// do initialization stuff
	Init();

	glutMainLoop();

	return 0; // never reached
}
