/*------------------------------------------------------------------+

	proj_db.h

	Jason F Smith

	A class to handle all of the databasing methods, calls, etc. 
	  that deal with the ground truth and the collected cut
	  scenes from the project.

 +-------------------------------------------------------------------*/

#ifndef PROJECT_DATABASE_H
#define PROJECT_DATABASE_H

#include <iostream.h>
#include <fstream.h>
#include "linklist2.h"	// an old template I dusted off.
#include "myString.h"

#ifndef MAX_FILENAME
#define MAX_FILENAME	128
#endif

#ifndef MAX_LINE
#define	MAX_LINE		128
#endif

#if defined( DEBUG )
#	define DATABASE_DEBUG
#	pragma message ("INFO - Cut database debugging: ON")
#else
#	pragma message ("INFO - Cut database debugging: OFF")
#endif

/* to make it easy to cut and paste new debug sections 
#if defined( DATABASE_DEBUG )
#endif // DATABASE_DEBUG
*/

struct CutNode{
	int frameNum;
	int color_diff;
	int color_thresh;
	int motion_diff;
	int motion_thresh;
};


class Cut_DB {
	protected:
		char logFile[MAX_FILENAME];

		LinkList<int> truths;
		LinkList<CutNode> cuts;

		int rStart;		// Range starting frame number
		int rEnd;		// Range ending frame number

		float precision;	// The most recently calculated precision.
		float recall;		// The most recently calculated recall.
		int numTruths;		// The number of truths in the range
		int numDetections;
		int missed;
		int falseAlarms;

		bool modified;		// Tells if recall/precision is accurate

		int cutThresh;

	public:

		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		Cut_DB( void )
		{
			Reset();
			strcpy( logFile, "my_truths.log" );
		};


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		~Cut_DB( void )
		{
			Reset();
		};

		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		LinkList<CutNode>* GetCuts( void ) { return &cuts; };

		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		LinkList<int>* GetTruths( void ){ return &truths; };


		/////////////////////////////////////////////////////////////
		//	Clears all of the cuts, and deletes all of the ground
		//	  truth data.
		/////////////////////////////////////////////////////////////
		void Reset( void )
		{
			truths.DeleteAll();
			cuts.DeleteAll();

			rStart = -1;
			rEnd = -1;
			numTruths=0;
			precision = recall = 0.0f;

			cutThresh = 5;
			modified = true;

#if defined( DATABASE_DEBUG )
			cout <<"DEBUG - Reseting DB...\n";
#endif // DATABASE_DEBUG
		};

		/////////////////////////////////////////////////////////////
		//	Erases all of the cut detections from the list.
		/////////////////////////////////////////////////////////////
		void ClearCuts( void )
		{
			cuts.DeleteAll();
			modified = true;
		};


		/////////////////////////////////////////////////////////////
		//  Sets the range for the precision and 
		/////////////////////////////////////////////////////////////
		void SetRange( int start, int end )
		{
			rStart = start;
			rEnd = end;
			modified = true;
		};


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		void SetStart( int start )
		{
			rStart = start;
			modified = true;
		};


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		void SetEnd( int end )
		{
			rEnd = end;
			modified = true;
		};

		//////////////////////////////////////
		/////////////////////////////////////
		void SetCutThreshold( int cutRange )
		{
			cutThresh = cutRange;
			modified = true;
		};


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		void AddCut( CutNode cut )
		{
			if( (cut.frameNum>=rStart) && (rEnd<0 || cut.frameNum<=rEnd) ){
				modified = true;
				cuts.InsertAtTail( cut );

#if defined( DATABASE_DEBUG )
				cout <<"DEBUG - added cut scene at "<<cut.frameNum<<"\n";
#endif // DATABASE_DEBUG
			}
#if defined( DATABASE_DEBUG )
			else {
				cout <<"DEBUG - add cut scene ignored (outside range) - Cut_DB::AddCut()\n";
			}
#endif // DATABASE_DEBUG
		};


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		bool LoadTruth( char* truthFile )
		{
			ifstream in;
			char line[500];

			int index;
			int num;
			long	loop=500;

			modified = true;

#if defined( DATABASE_DEBUG )
//			cout <<"DEBUG - loading truth file...\n";
#endif

			if( truthFile==NULL || strlen(truthFile)==0 )
			{
#if defined( DATABASE_DEBUG )
				cout <<"DEBUG - could not open ground truth file (invalid file) Cut_DB::LoadTruth()\n";
#endif // DATABASE_DEBUG
				return false;
			}

			in.open( truthFile );

			if( in.fail() ){
				in.close();
#if defined( DATABASE_DEBUG )
				cout <<"DEBUG - could not load ground truth file (error opening file) - Cut_DB::LoadTruth()\n";
#endif // DATABASE_DEBUG
				return false;
			}

			while( !in.eof() && (--loop)>0 )
			{
				in.getline( line, 500, '\n' );
				
				// first read in the line number
				index = 0;
				
				if( strlen(line)>0 && line[0]!='!' )
				{
					if( strstr( line," cut")!=NULL || strstr(line," CUT")!=NULL )
//						|| strstr( line, " gradtrans")!=NULL || strstr( line, "GRADTRANS")!=NULL )
					{
						index = 0;
						num = 0;
						while( line[index]!=' ' ){
							num = num*10 + (int)line[index++] - (int)'0';
						}
#if defined( DATABASE_DEBUG )
						//cout <<"DEBUG - adding truth cut at frame "<<num<<"\n";
#endif // DATABASE_DEBUG
						truths.InsertAtTail( num );
					}
				}
			}

			in.close();
			modified = true;
#if defined( DATABASE_DEBUG )
			cout <<"DEBUG - loaded "<<truths.GetSize()<<" truths\n";
#endif

			return true;
		};



		/////////////////////////////////////////////////////////////
		//  Brings both the recall and precision up to date.
		/////////////////////////////////////////////////////////////
		void CalcAll( void )
		{
			numDetections=0;
			falseAlarms=0;
			missed=0;

			int i, j;
			bool cutFound;
			LinkList<int> temp;

			truths.MoveToHead();
			cuts.MoveToHead();

cout <<"Calculating...\n";

			// create a temporary list to hold only the truth data which is
			//  in the range we specified
			for( i=0; i<truths.GetSize(); i++ ){
				if( truths[i]>=rStart ){
					if( rEnd>=0 ){
						if( truths[i] <= rEnd ){
							temp.InsertAtTail( truths[i] );
						}
					} else {
						temp.InsertAtTail( truths[i] );
					}
				}
			}

			numTruths = temp.GetSize();

			// we assume that the cut data for both lists are in increasing
			//   order...

			if( cutThresh<0 )
				cutThresh=0;

			for( i=0; i<cuts.GetSize(); i++ )
			{
				cutFound = false;
				for( j=0; j<temp.GetSize() && !cutFound; j++ )
				{
					if( Abs(cuts[i].frameNum-temp[j]) <= cutThresh )
					{
						numDetections++;
						cutFound=true;
						j++;
					}
				}

				if( !cutFound ){
					falseAlarms++;
				}
			}

			missed = temp.GetSize() - numDetections;

			// finally calculate what we came for...
			if( (numDetections+falseAlarms)==0 )
			{
#if defined( DATABASE_DEBUG )
				cout <<"DEBUG - precision had divisor of zero - Cut_DB::CalcAll()\n";
#endif // DATABASE_DEBUG
				precision = 0.0f;
			}
			else
				precision = ((float)numDetections) / ((float)(numDetections+falseAlarms));
			
			if( (numDetections+missed)==0 )
			{
#if defined( DATABASE_DEBUG )
				cout <<"DEBUG - recall had divisior of zero - Cut_DB::CalcAll()\n";
#endif // DATABASE_DEBUG
				recall = 0.0f;
			}
			else
				recall = ((float)numDetections) / ((float)(numDetections+missed));

cout <<"Done calculating...\n";
			modified=false;
		};

		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		bool IsAccurate( void ){ return !modified; };


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		int GetNumTruths( void ){ 
			if( modified )
				CalcAll();
			return numTruths;
		};


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		float GetPrecision( void )
		{
			if( modified )
				CalcAll();
			return precision;
		};


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		float GetRecall( void )
		{
			return recall;
		};
		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		int GetNumDetections( void ){
			if( modified )
				CalcAll();

			return numDetections;
		};


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		int GetNumFalse( void ){
			if( modified )
				CalcAll();
			return falseAlarms;
		};


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		int GetNumMissed( void ){
			if( modified )
				CalcAll();

			return missed;
		};



		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		bool SetLogFile( char* filename )
		{
			if( !filename )
			{
#if defined( DATABASE_DEBUG )
				cout <<"DEBUG - could not set log file (filename==NULL) - Cut_DB::SetLogFile()\n";
#endif // DATABASE_DEBUG
				return false;
			}

			strcpy( logFile, filename );
			return true;
		};


		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		bool WriteLog( void )
		{
			ofstream out;
			int i, j;

			if( modified )
				CalcAll();

			out.open( logFile );

			if( out.fail() )
			{
				out.close();
#if defined( DATABASE_DEBUG )
				cout <<"DEBUG - unable to open log file - Cut_DB::WriteLog()\n";
#endif // DATABASE_DEBUG
				return false;
			}

cout <<"Writing log...\n";

			out <<"CSE 486 - Term Project by Jason F Smith\n"
				<<"[cut detection log file]\n"
				<<"\n"
				<<"  Total truth: "<<numTruths<<"\n"
				<<"  Total reported: "<<cuts.GetSize()<<"\n"
				<<"  Num detected: "<<numDetections<<"\n"
				<<"  Precision: "<<100.0f*precision<<"%\n"
				<<"  Recall: "<<100.0f*recall<<"%\n"
				<<"  Num false alarms: "<<falseAlarms<<"\n"
				<<"  Num missed: "<<missed<<"\n"
				<<"\n"
				<<" Cut threshold: "<<cutThresh<<"\n"
				<<" Clip range: "<<rStart<<" thru "<<rEnd<<"\n"
				<<"\n";


			for( i=0, j=0; (i<cuts.GetSize() || j<truths.GetSize()); )
			{
				if( rEnd>0 && i<cuts.GetSize() ){
					if( cuts[i].frameNum>rEnd )
						i = cuts.GetSize()+1;
				}

				if( rEnd>0 && j<truths.GetSize() ){
					if(  truths[i]>rEnd )
						j = truths.GetSize()+1;
				}

				if( i<cuts.GetSize() && j<truths.GetSize() )
				{
					// choose which one to output first
					if( cuts[i].frameNum < truths[j] )
					{
						// display the cut first
						out <<"- "<<cuts[i].frameNum<<" detected cut\n"
							<<"      color diff: "<<cuts[i].color_diff<<"\n"
							<<"      color thre: "<<cuts[i].color_thresh<<"\n"
							<<"      motion dif: "<<cuts[i].motion_diff<<"\n"
							<<"      motion thr: "<<cuts[i].motion_thresh<<"\n";
						i++;
					}
					else // display the truth
					{
						if( truths[j]>=rStart && ( rEnd<0 || truths[j]<=rEnd ) )
							out <<"+ "<<truths[j]<<" ground truth cut\n";

						j++;
					}
				} 
				else if( i<cuts.GetSize() )
				{
					// write out the cut data
					out <<"- "<<cuts[i].frameNum<<" detected cut\n"
						<<"      color diff: "<<cuts[i].color_diff<<"\n"
						<<"      color thre: "<<cuts[i].color_thresh<<"\n"
						<<"      motion dif: "<<cuts[i].motion_diff<<"\n"
						<<"      motion thr: "<<cuts[i].motion_thresh<<"\n";
					i++;
				}
				else if( j<truths.GetSize() )
				{
					// write out the truth data
					if( truths[j]>=rStart && (rEnd<0 || truths[j]<=rEnd ) )
						out <<"+ "<<truths[j]<<" ground truth cut\n";

					j++;
				}

			}// for

			out.close();

cout <<"Finished writing log...\n";

			return true;
		}; // WriteLog()


}; // Cut_DB


//////////////////////////////////////////////////////////////////
#ifdef DATABASE_DEBUG
#undef DATABASE_DEBUG
#endif
//////////////////////////////////////////////////////////////////

#endif // PROJECT_DATABASE_H
