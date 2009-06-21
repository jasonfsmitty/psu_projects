/*
	Some functions to convert strings to/from integers/floats.

	Jason F Smith
*/

#ifndef JFS_STRING_H
#define JFS_STRING_H

int StringToInt( char* str );
float StringToFloat( char* str );
char* IntToString( int value );
char* FloatToString( float value );

#endif