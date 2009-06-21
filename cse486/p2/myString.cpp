#include <iostream.h>
#include <stdio.h>
#include <string.h>

/*-----------------------------------------------------------------+
	Quick and dirty function to convert a string into an integer
 +-----------------------------------------------------------------*/
int StringToInt( char *str )
{
	int total = 0;
	int i=0;
	for( i=0; i<128 && str[i]!=NULL && str[i]!='\n'; i++ ){
		if( str[i]>='0' || str[i]<='9' ){
			total = total*10 + (str[i]-'0');
		}
	}
	return total;
}

/*-------------------------------------------------------------------+
	Another quick and dirty function.  This one converts an
	integer to a char string
 +-------------------------------------------------------------------*/
char* IntToString( int value )
{
	static char temp[64];
	char *ptr;
	temp[63] = '\0';
	ptr = temp+63;

	do{
		ptr--;
		*ptr = (char)(value%10)+'0';
		value = value/10;
	} while( value>0 );

	return ptr;
}

/*---------------------------------------------------------------+
	Turn a float into a string
 +---------------------------------------------------------------*/
char* FloatToString( float value )
{
	static char temp[64];	
	sprintf( temp, "%3.3f", value );
	return temp;
}


/*-------------------------------------------------------+
	Turn a string into a float
 +-------------------------------------------------------*/
float StringToFloat( char* str )
{
	float value=0.0f;
	float index=1.0f;
	bool foundPeriod = false;
	bool foundNegative = false;

	while( *str != NULL ){
		if( *str >='0' && *str <= '9' ){
			// add the value to the float
			if( foundPeriod ){
				value += (float)(*str-'0') * index;
				index /= 10.0f;
			} else {
				// add normal
				value *= 10.0f;
				value += (float)(*str-'0');
			}
		} else if( *str == '-' ){
			foundNegative=true;
		} else if( *str == '.' && !foundPeriod ){
			foundPeriod=true;
			index = .1f;
		}

		str++;
	}

	if( foundNegative )
		value = -value;
	return value;
}

