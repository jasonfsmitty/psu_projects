/*-----------------------------------------------------------------+
	defines.h

	Jason F Smith
	CSE 486 Semester Project

	Header file containing global data, function declarations, etc.

 +-----------------------------------------------------------------*/

#include "glui.h"

#include "cse486_mpeglib-cpp.h"
#include "int_queue.h"
#include "histogram.h"
#include "linklist2.h"


#ifndef		Abs

#	define	Abs(A)		((A)<0.0 ? -(A) : (A))
#	define	Max(A,B)	((A)>=(B) ? (A) : (B))
#	define	Min(A,B)	((A)>=(B) ? (A) : (B))

#endif


// Enums for the control callback Control_CB
enum {
	QUIT=99
};

// Enums for the mode changing function, EnterMode()
enum {
	OPTIONS_MODE=0,
	CALC_MODE,
	PREVIEW_MODE,
	RESULTS_MODE
};

#define		RESULT_MODE		RESULTS_MODE

// different color spaces
enum {
	LUV_COLOR_SPACE=0,
	RGB_COLOR_SPACE
};

enum {
	LUV_COLOR=LUV_COLOR_SPACE,
	RGB_COLOR=RGB_COLOR_SPACE
};

// different difference calculations
enum {
	ABS_DIFF=0,
	SQUARED_DIFF
};

// various methods
enum {
	COLOR_METHOD=0,
	MOTION_METHOD,
	BOTH_METHODS
};


struct MPEG_Data {
	mpeginfo *info;
	mpegframe *frame;

	bool isSwapped;

	int cFrame;
};


// Stuff in term2.cpp ----------------------------------------------

void CalcIdle( void );
void SpeedIdle( void );
void EnterMode( int );
void CalcResults( void );
void CleanUp( void );
void CheckForCut( void );

void CalcStats( void );
void LoadTruth( void );
int  GetFrame( char* );
void StartLog( void );
void EndLog( void );

// functions from gluiStuff.cpp ---------------------------

void Init_GLUI( void );
void Clean_GLUI( void );
void Control_CB( int );
void OpenResults( void );
void CloseResults( void );

// From mpegPlay.cpp ---------------------------------------------

void OpenMyMPEG( void );
void MPEG_Display( void );
void CloseMyMPEG( void );
bool IsOpenMPEG( void );

// needed to show the frame difference window
//  located in term2.cpp
void HIST_Display( void );
void HIST_Reshape( int, int );
void HIST_Keyboard( unsigned char, int, int );

// in methods.cpp -------------------------------------

void RunColor( void );
void RunMotion( void );


// GLOBAL DATA -----------------------------------------------------

// in gluiStuff.cpp
extern GLUI			*glui;		// Main GLUI window
extern GLUI			*result_glui; // Result GLUI window

// in term2.cpp
extern int			movieIndex;
extern int			movieWidth;
extern int			movieHeight;

extern int			skipY;		// these must be set before going into CALC_MODE
extern int			skipX;		//   they are NOT set by GLUI

extern int			watchMovie;
extern int			frameSkip;
extern int			startFrame;
extern int			endFrame;

extern int			colorSpace;
extern int			colorDiff;
extern int			colorThreshold;

extern int			blockSize;
extern int			blockRange;
extern int			motionDiff;
extern int			sameThreshold;
extern int			motionThreshold;

extern int			runType;
extern int			runStep;
extern int			cutWindow;
extern int			showPlot;
extern int			useLog;
extern char			logFile[ sizeof(GLUI_String) ];

extern MPEG_Data		mpegData;

extern char*			movieFiles[4];
extern char*			truthFiles[4];

extern HistWin			histWin;
extern IntQueue			colorQ;
extern IntQueue			motionQ;

extern unsigned char 		colorHistory[ 4300 ];
extern unsigned char 		motionHistory[ 4300 ];
extern int			historyIndex;

extern float			precision;
extern float			recall;
extern int			numTruths;
extern int			numReported;
extern int			numDetected;
extern int			numFalse;
extern int			numMissed;
