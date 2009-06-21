/*---------------------------------------------------------------+

	Jason F. Smith
	CSE 418  Fall 2000
	Project 3

 +---------------------------------------------------------------*/

#ifndef MY_SPLINE_H
#define MY_SPLINE_H

#include <GL/glut.h>
#include <iostream>
using namespace std;
#include <math.h>

#if defined( USE_TEXTURES )
	extern bool globalTexturingOn;
	extern GLuint defaultTextureName;
#endif


//#define		SHOW_POINTS_ON_EXIT
//#define		USE_OPENGL_SPLINES
//#define		DRAW_SPLINE_WITH_OLD_NORMALS


#define		Max(A,B)		( (A)>=(B) ? (A) : (B) )
#define		Min(A,B)		( (A)<(B) ? (A) : (B) )


#define		MAX_DIVISIONS		0x00000080
#define		MIN_DIVISIONS		0x00000002

// levels of continuity
#define		LEVEL_C0			0x00
#define		LEVEL_C1			0x01

// the various edges of the spline
#define		UP_EDGE				0x01
#define		LEFT_EDGE			0x02
#define		DOWN_EDGE			0x04
#define		RIGHT_EDGE			0x08
#define		ALL_EDGES			0x0f


//#define		NEXT_X_STEP			4
//#define		NEXT_Z_STEP			4


// for all of the vertex points
GLfloat		subPts[MAX_DIVISIONS+3][MAX_DIVISIONS+1][3];
#define		DIVIDE_VERT		1
#define		DIVIDE_HORI		2

// for the all of the normals
GLfloat		vertNormals[MAX_DIVISIONS+1][MAX_DIVISIONS+1][3];
GLfloat		polyNormals[MAX_DIVISIONS][MAX_DIVISIONS][2][3];


extern GLfloat mat_diffuse[];
extern bool globalLightingOn;
GLfloat mat_noDiffuse[] = { 0.0, 0.0, 0.0, 1.0 };

inline void subdivide( int numDivisions, int row, int col, int orientation, GLfloat *tanLeft, GLfloat* tanRight );
inline void SetNormal( GLfloat* p0, GLfloat* p1, GLfloat* p2 );
inline void CalculateAllNormals( int numDiv );
inline void CalcNormal( GLfloat* p0, GLfloat* p1, GLfloat* p2, GLfloat* n );
inline void SumNormals( int row, int col, GLfloat* n, int numDiv );

/*---------------------------------------------------------------+
	
	The spline model is designed as follows, with each pair
	being the values of the first two [] which determine
	the control point.
		i.e - to get the vertex at (2,1), use cntrlPts[2][1][0,1,2]

							 t=0 (up)
		(0,0)---------(0,1)------------(0,2)-----------(0,3)
		 |												  |
		 |                                                |
		 |											      |
		(1,0)         (1,1)            (1,2)           (1,3)
		 |												  |
    s=0	 |                                                |  s=1
  (left) |											      |  (right)
		(2,0)         (2,1)            (2,2)           (2,3)
		 |												  |
		 |                                                |
		 |											      |
		(3,0)---------(3,1)------------(3,2)-----------(3,3)
							 t=1 (down)

 +---------------------------------------------------------------*/

class mySpline{
	public:

		float cntrlPts[4][4][3];
		float adjustedPts[4][4][3];

		long numDiv;

		bool drawSolid; // Wireframe or solid mode?

		mySpline* next; // for surface class, used to link them together.
		mySpline* prev;

		char edgeFlags;  // Sets/Clears C1 continuity between two edges

		bool modified;
		GLuint theList;
		GLuint myTexName;

		// These are used to 'join' mySplines together.
		mySpline* up;
		mySpline* left;
		mySpline* down;
		mySpline* right;

// This used to be start of public

		friend class SplineList;
		mySpline( void ){
			next=prev=NULL;
			up=left=down=right=NULL;
			edgeFlags=0;
			drawSolid=true;
			numDiv=0x0010;
			theList=0;
			modified = true;

			myTexName = 0;

			int i, j, k;

			for( i=0; i<4; i++){
				for( j=0; j<4; j++ ){
					for( k=0; k<3; k++ ){
						cntrlPts[i][j][k]=0.0;
					} // for
				} // for
			} // for
#if defined( USE_OPENGL_SPLINES )
			cout <<"Using the OpenGL spline functions.\n";
#endif
		}; // mySpline()

		~mySpline( void ){
#if defined( USE_SPLINE_DISPLAY_LISTS )
			glDeleteLists( theList, 1 );
#endif
			if( up )
				up->down=NULL;
			if( left )
				left->right=NULL;
			if( down )
				down->up=NULL;
			if( right )
				right->left=NULL;

#if defined(SHOW_POINTS_ON_EXIT)
			int i, j, k;
			for( i=0; i<4; i++){
				for( j=0; j<4; j++ ){
					cout <<"(";
					for( k=0; k<3; k++ ){
						cout <<cntrlPts[i][j][k]<<", ";
					} // for
					cout <<" )\n";
				} // for
			} // for			
#endif
		}; // ~myspline()

		/* set all 16 control points */
		void SetCntrlPts( float controlPts[4][4][3] ){
			int i, j, k;

			for( i=0; i<4; i++){
				for( j=0; j<4; j++ ){
					for( k=0; k<3; k++ ){
						cntrlPts[i][j][k]=controlPts[i][j][k];
					} // for
				} // for
			} // for

			modified = true;

#if defined( USE_OPENGL_SPLINES )
			glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &cntrlPts[0][0][0] );
			glEnable( GL_MAP2_VERTEX_3 );
			glEnable( GL_AUTO_NORMAL );
			glMapGrid2f( 20, 0.0, 1.0, 20, 0.0, 1.0 );
#endif
		}; // SetCntrlPts

		/* set a single point */
		void SetPoint( int a1 /* first [] */,
					   int a2 /* second [] */,
					   float x /* new x value */,
					   float y /* new y value */,
					   float z /* new z value */	){
			if( a1>2 || a1<0 || a2>2 || a2<0 )
				return;
			cntrlPts[a1][a2][0] = x;
			cntrlPts[a1][a2][1] = y;
			cntrlPts[a1][a2][2] = z;

			modified=true;
		}; // SetPoint()

		/* returns pointer to the 3-element array of that point */
		float* GetPoint( int a1 /* first [] */, int a2 /* second [] */ ){
			if( a1>2 || a2>2 || a1<0 || a2<0 )
				return NULL;
			return cntrlPts[a1][a2];
		}; // GetPoint

		/* increase the sub-divisions by one step */
		void IncreaseDivisions( void ){
			numDiv = Min( numDiv<<1, MAX_DIVISIONS );
			modified = true;
		}; // IncreaseDivisions

		/* decrease the sub-divisions by one step */
		void DecreaseDivisions( void ){
			numDiv = Max( numDiv>>1, MIN_DIVISIONS );
			modified = true;
		}; // DecreaseDivisions

		void MinDivisions( void ){
			numDiv = MIN_DIVISIONS;
			modified = true;
		};

		void MaxDivisions( void ){
			numDiv = MAX_DIVISIONS;
			modified = true;
		};

		/* returns the current number of subdivisions */
		int GetDivisions( void ){ return numDiv; };


		/* 
			Draws the spline - does not perform any modeling
			transformations since the only thing to rotate or move is
			the viewing frustrum.  Make sure any rotations/translations
			are made before calling Draw.								
		*/
		void Draw( void ){
#if defined( USE_OPENGL_SPLINES )
			glMapGrid2f( numDiv, 0.0, 1.0, numDiv, 0.0, 1.0 );
			glEvalMesh2( GL_FILL, 0, numDiv, 0, numDiv );
#else // use or own splines
			if( drawSolid )
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			else
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	#if defined( USE_TEXTURES )
			if( globalTexturingOn  ){
				if( myTexName!=0)	
					TurnTextureOn( myTexName );
				else
					TurnTextureOn( defaultTextureName );
			}
	#endif // use textures

	#if defined( USE_SPLINE_DISPLAY_LISTS )
			if( modified ){
				glDeleteLists( theList, 1 );
				theList = glGenLists( 1 );
				glNewList( theList, GL_COMPILE_AND_EXECUTE );
	#endif // use display lists
				// begin list here ---------------------------------
				int i, j;

				for( i=0; i<4; i++ ){
					for( j=0; j<4; j++ ){
						adjustedPts[ i ][ j ][0] = cntrlPts[ i ][ j ][0];
						adjustedPts[ i ][ j ][1] = cntrlPts[ i ][ j ][1];
						adjustedPts[ i ][ j ][2] = cntrlPts[ i ][ j ][2];
					}
				} // for

				// Calculate the adjusted points
				if( up && edgeFlags&UP_EDGE ){
					for( i=0; i<4; i++ ){
						cout <<"Calculating top edge."<<endl;
						adjustedPts[0][i][0] = ( cntrlPts[0][i][0] + up->cntrlPts[0][i][0])/2.0;
						adjustedPts[0][i][1] = ( cntrlPts[0][i][1] + up->cntrlPts[0][i][1])/2.0;
						adjustedPts[0][i][2] = ( cntrlPts[0][i][2] + up->cntrlPts[0][i][2])/2.0;
					}
				}
				
				if( left && edgeFlags&LEFT_EDGE ){
					for( i=0; i<4; i++ ){
						cout <<"Calculating left edge."<<endl;
						adjustedPts[i][0][0] = ( cntrlPts[i][0][0] + left->cntrlPts[i][0][0])/2.0;
						adjustedPts[i][0][1] = ( cntrlPts[i][0][1] + left->cntrlPts[i][0][1])/2.0;
						adjustedPts[i][0][2] = ( cntrlPts[i][0][2] + left->cntrlPts[i][0][2])/2.0;
					}
				}

				if( down && edgeFlags&DOWN_EDGE ){
					for( i=0; i<4; i++ ){
						cout <<"Calc bottom edge."<<endl;
						adjustedPts[3][i][0] = ( cntrlPts[3][i][0] + down->cntrlPts[3][i][0])/2.0;
						adjustedPts[3][i][1] = ( cntrlPts[3][i][1] + down->cntrlPts[3][i][1])/2.0;
						adjustedPts[3][i][2] = ( cntrlPts[3][i][2] + down->cntrlPts[3][i][2])/2.0;
					}
				}
				
				if( right && edgeFlags&RIGHT_EDGE ){
					for( i=0; i<4; i++ ){
						cout <<"Calc right edge."<<endl;
						adjustedPts[i][3][0] = ( cntrlPts[i][3][0] + right->cntrlPts[i][3][0])/2.0;
						adjustedPts[i][3][1] = ( cntrlPts[i][3][1] + right->cntrlPts[i][3][1])/2.0;
						adjustedPts[i][3][2] = ( cntrlPts[i][3][2] + right->cntrlPts[i][3][2])/2.0;
					}
				}

				// subdivide the tangents
				// first tangent - left
				subPts[MAX_DIVISIONS+1][0][0] = adjustedPts[0][1][0];
				subPts[MAX_DIVISIONS+1][0][1] = adjustedPts[0][1][1];
				subPts[MAX_DIVISIONS+1][0][2] = adjustedPts[0][1][2];
				// right tangents
				subPts[MAX_DIVISIONS+1][numDiv][0] = adjustedPts[3][1][0];
				subPts[MAX_DIVISIONS+1][numDiv][1] = adjustedPts[3][1][1];
				subPts[MAX_DIVISIONS+1][numDiv][2] = adjustedPts[3][1][2];
				// subdivide
				subdivide( numDiv, 
					   MAX_DIVISIONS+1, 0,
					   DIVIDE_HORI,
					   adjustedPts[1][1],
					   adjustedPts[2][1] 		);
				// first tangent - left
				subPts[MAX_DIVISIONS+2][0][0] = adjustedPts[0][2][0];
				subPts[MAX_DIVISIONS+2][0][1] = adjustedPts[0][2][1];
				subPts[MAX_DIVISIONS+2][0][2] = adjustedPts[0][2][2];
				// right tangents
				subPts[MAX_DIVISIONS+2][numDiv][0] = adjustedPts[3][2][0];
				subPts[MAX_DIVISIONS+2][numDiv][1] = adjustedPts[3][2][1];
				subPts[MAX_DIVISIONS+2][numDiv][2] = adjustedPts[3][2][2];
				// subdivide
				subdivide( numDiv, 
					   MAX_DIVISIONS+2, 0, 
					   DIVIDE_HORI, 
					   adjustedPts[1][2],
					   adjustedPts[2][2] );

				// now subdivide the edges
				subPts[0][0][0] = adjustedPts[0][0][0];
				subPts[0][0][1] = adjustedPts[0][0][1];
				subPts[0][0][2] = adjustedPts[0][0][2];

				subPts[numDiv][0][0] = adjustedPts[3][0][0];
				subPts[numDiv][0][1] = adjustedPts[3][0][1];
				subPts[numDiv][0][2] = adjustedPts[3][0][2];

				subPts[0][numDiv][0] = adjustedPts[0][3][0];
				subPts[0][numDiv][1] = adjustedPts[0][3][1];
				subPts[0][numDiv][2] = adjustedPts[0][3][2];

				subPts[numDiv][numDiv][0] = adjustedPts[3][3][0];
				subPts[numDiv][numDiv][1] = adjustedPts[3][3][1];
				subPts[numDiv][numDiv][2] = adjustedPts[3][3][2];

				// subdivide on s=0 and then s=1
				subdivide( numDiv, 0, 0, DIVIDE_VERT, adjustedPts[1][0], adjustedPts[2][0] );
				subdivide( numDiv, 0, numDiv, DIVIDE_VERT, adjustedPts[1][3], adjustedPts[2][3] );

				// now subdivide the entire mesh
				for( i=0; i<=numDiv; i++ ){
					subdivide( numDiv, i, 0, DIVIDE_HORI,
						   subPts[MAX_DIVISIONS+1][i],
						   subPts[MAX_DIVISIONS+2][i]   );
				} // for


				// now that the subdivision is complete, draw the triangles that
				//  will make up the surface
				//glNormal3f( 0.0, 0.0, 1.0 );
	#if defined( DRAW_SPLINE_WITH_OLD_NORMALS )
				for( i=0; i<numDiv; i++ ){
					glBegin( GL_TRIANGLES );
					for( int j=0; j<=numDiv; j++ ){
						// compute the normal vector
						SetNormal( subPts[i][j], subPts[i+1][j], subPts[i][j+1] );
						glVertex3fv( subPts[i][j] );
						glVertex3fv( subPts[i+1][j] );
						glVertex3fv( subPts[i][j+1] );

						SetNormal( subPts[i][j+1], subPts[i+1][j], subPts[i+1][j+1] );
						glVertex3fv( subPts[i][j+1] );
						glVertex3fv( subPts[i+1][j] );
						glVertex3fv( subPts[i+1][j+1] );			
					} // for
					glEnd();
				}// for
	#else // draw with new normals

				CalculateAllNormals( numDiv );

				for( i=0; i<numDiv; i++ ){
					glBegin( GL_TRIANGLE_STRIP );
					for( int j=0; j<=numDiv; j++ ){
		#if defined( USE_TEXTURES )
						if( globalTexturingOn ){
							glTexCoord2f( ((float)j)/((float)numDiv),
								      ((float)i)/((float)numDiv) );
						}
		#endif // use textures
						glNormal3fv( vertNormals[i][j] );
						glVertex3fv( subPts[i][j] );
		#if defined( USE_TEXTURES )
						if( globalTexturingOn ){
							glTexCoord2f( ((float)(j))/((float)numDiv),
								      ((float)i+1.0)/((float)numDiv)
);
						}
		#endif // use textures
						glNormal3fv( vertNormals[i+1][j] );
						glVertex3fv( subPts[i+1][j] );
					} // for
					glEnd();
				} // for
	#endif // use old normals

	#if defined( USE_SPLINE_DISPLAY_LISTS )

				glEndList();
				modified = false;
			} else {
				glCallList( theList );
			}// else
	#endif // USE_SPLINE_DISPLAY_LISTS )				
#endif // use openGL spline drawing

		}; // Draw


		/*
			Joins two splines together with C(level) continuity.
			level may be either LEVEL_C0 or LEVEL_C1.  All other
			values are ignored and the function does nothing.
			The function uses the corner control points to judge
			which edges are closest, and hence to be joined.  
			The control points are only altered for the second mySpline.
			Once a continuity is set, both splines will be altered 
			accordingly when a change is made to either in order for
			both to continue to be at C(level) contnuity.
		*/
		friend bool SetContinuity( mySpline* one, mySpline* two, int level );

		/*
			Turns off the continuity between the two splines.  Does not
			alter any control points, so if only C0 continuity was set,
			no changes will be visible until control points along the
			edge are altered.  If C1 continuity is set, then the splines
			will probably change back to what appears to be C0 because
			the tangents of the surfaces no longer have to be equal.
		*/
		friend bool ClearContinuity( mySpline* one, mySpline* two );

		/* checks if in wireframe or solid mode. */
		bool isSolid( void ) { return drawSolid; };

		void SetModified( void ){ modified = true; };

}; // mySpline


/*--------------------------------------------------------------------------------+
	Joins two splines together with C(level) continuity. level may 
	be either LEVEL_C0 or LEVEL_C1.  All other values are ignored 
	and the function does nothing.  The function uses the corner 
	control points to judge which edges are closest, and hence to be joined.  
	The control points are only altered for the second mySpline.  Once a 
	continuity is set, both splines will be altered accordingly when a 
	change is made to either in order for both to continue to be 
	at C(level) contnuity.
 +--------------------------------------------------------------------------------*/
bool SetContinuity( mySpline* one, mySpline* two, int level )
{
	// Nothing to do here yet.
	return true;
} // SetContinuity


/*-------------------------------------------------------------------------------+
	Turns off the continuity between the two splines.  Does not
	alter any control points, so if only C0 continuity was set,
	no changes will be visible until control points along the
	edge are altered.  If C1 continuity is set, then the splines
	will probably change back to what appears to be C0 because
	the tangents of the surfaces no longer have to be equal.
 +-------------------------------------------------------------------------------*/
bool ClearContinuity( mySpline* one, mySpline* two )
{
	// Nothing to do here yet.
	return true;
} // ClearContinuity

/*===============================================================================+
	Calculate the normal for each vertex of the surface.
 +===============================================================================*/
void CalculateAllNormals( int numDiv )
{
	int i, j;
	GLfloat n[3];

	// set all of the normal vectors equal to zero
	for( i=0; i<=numDiv; i++ ){
		for( j=0; j<=numDiv; j++ ){
			vertNormals[i][j][0] = 0.0;
			vertNormals[i][j][1] = 0.0;
			vertNormals[i][j][2] = 0.0;
		} // for j
	} // for i

	// Calculate the normals for each polygon
	for( i=0; i<numDiv; i++ ){
		for( j=0; j<numDiv; j++ ){
			// Calculate normal for triangle 0
			CalcNormal( subPts[i][j], subPts[i+1][j], subPts[i][j+1], n );
			polyNormals[i][j][0][0] = n[0];
			polyNormals[i][j][0][1] = n[1];
			polyNormals[i][j][0][2] = n[2];
			
			// Calculate normal for triangle 1
			CalcNormal( subPts[i][j+1], subPts[i+1][j], subPts[i+1][j+1], n );
			polyNormals[i][j][1][0] = n[0];
			polyNormals[i][j][1][1] = n[1];
			polyNormals[i][j][1][2] = n[2];
		} // for j
	} // for i

	// At each vertex, the normal is a sum of the surrounding polygon normals
	for( i=0; i<=numDiv; i++ ){
		for( j=0; j<=numDiv; j++ ){
			SumNormals( i, j, vertNormals[i][j], numDiv );
		} // for j
	} // for i

}

/*-------------------------------------------------------------------------------+
	Sum the surrounding normals of the vertex
 +-------------------------------------------------------------------------------*/
inline void SumNormals( int row, int col, GLfloat* n, int numDiv )
{
	if( row<numDiv && col<numDiv ){
		// add in from down right
		n[0] += polyNormals[row][col][0][0];
		n[1] += polyNormals[row][col][0][1];
		n[2] += polyNormals[row][col][0][2];
	}
	if( row>0 && col<numDiv ){
		// add in from top right
		n[0] += polyNormals[row-1][col][0][0];
		n[1] += polyNormals[row-1][col][0][1];
		n[2] += polyNormals[row-1][col][0][2];

		n[0] += polyNormals[row-1][col][1][0];
		n[1] += polyNormals[row-1][col][1][1];
		n[2] += polyNormals[row-1][col][1][2];
	}
	if( row>0 && col>0 ){
		// add in the top left
		n[0] += polyNormals[row-1][col-1][1][0];
		n[1] += polyNormals[row-1][col-1][1][1];
		n[2] += polyNormals[row-1][col-1][1][2];
	}
	if( row<numDiv && col>0 ){
		// add in from bottom left
		n[0] += polyNormals[row][col-1][0][0];
		n[1] += polyNormals[row][col-1][0][1];
		n[2] += polyNormals[row][col-1][0][2];

		n[0] += polyNormals[row][col-1][1][0];
		n[1] += polyNormals[row][col-1][1][1];
		n[2] += polyNormals[row][col-1][1][2];
	}

	GLdouble length = sqrt( (double)( n[0]*n[0]+n[1]*n[1]+n[2]*n[2] ) );
	n[0] = n[0]/length;
	n[1] = n[1]/length;
	n[2] = n[2]/length;

}
/*===============================================================================+
	Calculate a normal and store it in n (normalized)
 +===============================================================================*/
inline void CalcNormal( GLfloat* p0, GLfloat* p1, GLfloat* p2, GLfloat* n )
{
	// A = (P0-P1)
	// B = (P1-P2)
	// Normal = AxB = (P0-P1)x(P1-P2)

	GLfloat a[3], b[3];
	a[0] = p1[0]-p0[0];
	a[1] = p1[1]-p0[1];
	a[2] = p1[2]-p0[2];

	b[0] = p1[0]-p2[0];
	b[1] = p1[1]-p2[1];
	b[2] = p1[2]-p2[2];

	// nx = (ay)(bz)-(az)(by)
	//n[0] = (p0[1]-p1[1])*(p1[2]-p2[2]) - (p0[2]-p1[2])*(p1[1]-p2[1]);
	n[0] =  a[1]*b[2] - a[2]*b[1];
	// ny = (az)(bx)-(ax)(bz)
	n[1] =  a[2]*b[0] - a[0]*b[2];
	// nz = (ax)(by) - (ay)(bx)
	n[2] = a[0]*b[1] - a[1]*b[0];

#if defined( MY_AUTO_NORMALIZE )
	GLdouble length = sqrt( (double)( n[0]*n[0]+n[1]*n[1]+n[2]*n[2] ) );
	n[0] = n[0]/length;
	n[1] = n[1]/length;
	n[2] = n[2]/length;
#endif
}

/*===============================================================================+
	Calculate a single normal for a point
 +===============================================================================*/
inline void SetNormal( GLfloat* p0, GLfloat* p1, GLfloat* p2 )
{
	// A = (P0-P1)
	// B = (P1-P2)
	// Normal = AxB = (P0-P1)x(P1-P2)

	GLfloat n[3], a[3], b[3];
	a[0] = p1[0]-p0[0];
	a[1] = p1[1]-p0[1];
	a[2] = p1[2]-p0[2];

	b[0] = p1[0]-p2[0];
	b[1] = p1[1]-p2[1];
	b[2] = p1[2]-p2[2];

	// nx = (ay)(bz)-(az)(by)
	//n[0] = (p0[1]-p1[1])*(p1[2]-p2[2]) - (p0[2]-p1[2])*(p1[1]-p2[1]);
	n[0] =  a[1]*b[2] - a[2]*b[1];
	// ny = (az)(bx)-(ax)(bz)
	n[1] =  a[2]*b[0] - a[0]*b[2];
	// nz = (ax)(by) - (ay)(bx)
	n[2] = a[0]*b[1] - a[1]*b[0];

	//glEnable( GL_NORMALIZE );
	// GL_NORMALIZE doesn't work for some reason !!??
	GLdouble length = sqrt( (double)( n[0]*n[0]+n[1]*n[1]+n[2]*n[2] ) );
	n[0] = n[0]/length;
	n[1] = n[1]/length;
	n[2] = n[2]/length;
	glNormal3fv( n );
}

/*===============================================================================+
	Recursive subdivision
 +===============================================================================*/
void subdivide( int numDivisions, int row, int col, int orientation, GLfloat* tanLeft, GLfloat* tanRight )
{
	// get the current points
	GLfloat* l0;
	GLfloat* r3; 
	GLfloat* l3;
	GLfloat l1[3];
	GLfloat l2[3];
	GLfloat r1[3];
	GLfloat r2[3];
	GLfloat h[3];

	// l0 = P0
	l0 = subPts[row][col];

	// r3 = P3
	r3 = ( orientation==DIVIDE_HORI ? 
		   subPts[row][col+numDivisions] : 
		   subPts[row+numDivisions][col] );

	// This is the point to be set
	l3 = ( orientation==DIVIDE_HORI ?
		   subPts[row][col+(numDivisions/2)] :
		   subPts[row+(numDivisions/2)][col] );

	// H - (P1+P2) / 2
	h[0] = ( tanLeft[0] + tanRight[0] ) / 2.0;
	h[1] = ( tanLeft[1] + tanRight[1] ) / 2.0;
	h[2] = ( tanLeft[2] + tanRight[2] ) / 2.0;
	
	// l1 = (0.5)(P0 + P1)
	l1[0] = ( l0[0] + tanLeft[0] ) / 2.0;
	l1[1] = ( l0[1] + tanLeft[1] ) / 2.0;
	l1[2] = ( l0[2] + tanLeft[2] ) / 2.0;
	
	// l2 = (0.5)( l1 + h )
	l2[0] = ( l1[0] + h[0] ) / 2.0;
	l2[1] = ( l1[1] + h[1] ) / 2.0;
	l2[2] = ( l1[2] + h[2] ) / 2.0;
	
	// r2 = (0.5)( P2+P3 )
	r2[0] = ( tanRight[0] + r3[0] ) / 2.0;
	r2[1] = ( tanRight[1] + r3[1] ) / 2.0;
	r2[2] = ( tanRight[2] + r3[2] ) / 2.0;

	// r1 = (0.5)( r2 + h )
	r1[0] = ( r2[0] + h[0] ) / 2.0;
	r1[1] = ( r2[1] + h[1] ) / 2.0;
	r1[2] = ( r2[2] + h[2] ) / 2.0;

	// r0 = l3 = (0.5)( l2 + r1 )
	l3[0] = ( l2[0] + r1[0] ) / 2.0;
	l3[1] = ( l2[1] + r1[1] ) / 2.0;
	l3[2] = ( l2[2] + r1[2] ) / 2.0;

	if( numDivisions > 2 ){
		// subdivide left side first
		subdivide( numDivisions/2, row, col, orientation, l1, l2 );
		// subdivide right next
		subdivide( numDivisions/2, 
			   (orientation==DIVIDE_HORI ? row : row+numDivisions/2 ),
			   (orientation==DIVIDE_HORI ? col+numDivisions/2 : col ),
			   orientation,
			   r1,
			   r2 	);
	}// if
} // subdivide

#endif
