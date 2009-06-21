	/*

	Jason F. Smith  4588
	CSE 418   Project 2

	Defines the bugs for the project.
*/


#ifndef MYBUG_DEFS_H
#define MYBUG_DEFS_H

#include <GL/glut.h>
#include <iostream.h>

#include "p2_defs.h"
#include "myTransform.h"

// If the following define is commented out, then
//   the ghosts eyes are drawn with rectangles.
//#define		DRAW_GHOSTS_EYES_WITH_TRIANGLES

// Draws rotating sparks on the Power Pellets
#define		DRAW_SPARKS_ON_FOOD

// Struct to save a point ------------------------------------------
struct bugPts{
	double x, y;
};

enum	{	GHOST_RED=1,
			GHOST_BLUE=2,
			GHOST_PURPLE=3,
			GHOST_ORANGE=4
		};

int nextGhost = GHOST_RED;

/*------------------------------------------------------------------+
	Class for the base bug.
 +------------------------------------------------------------------*/
class BasicBug{
	public:
		GLfloat x, y, theta;  // position and orientation.
		double vx, vy;	// velocity vector components.
		double stride; // current 'step'
		
		int bx, by;  // bounding box width and height.
		int type;
		int size;

		BasicBug *next;
		BasicBug *prev;

		BasicBug( void ){ x=y=theta=0.0;  vx=vy=stride=0.0;
				  bx=by=0;  next=prev=NULL;	    };
		~BasicBug( void ){};
		
		virtual void Draw( void ){};
		virtual void Move( void ){};
		virtual void SetSize( int newSize ){ size = newSize; };
		virtual void EatFood( void ) {};

		friend inline bool bugsCollide( BasicBug *b1, BasicBug *b2 );
		friend inline void bugDeflect( BasicBug *b1, BasicBug *b2 );
}; // BasicBug class


/*-----------------------------------------------------------------+
 +-----------------------------------------------------------------*/
class Food : public BasicBug{
	public:

		Food( void ) : BasicBug() {
			size = FOOD_SIZE;
			bx = by = size;
			type = FOOD_TYPE;
		};
		~Food( void ){};

		void Draw( void ){
			myPushMatrix();
			myTranslate( x, y );
			//myRotate( stride );

			glLineWidth( 1.0 );

			glColor3f( PREY_R, PREY_G, PREY_B );

			double pt = ((double)size)/2.0;
			glBegin( GL_POLYGON );
				myVertex2d( pt, (double)size );
				myVertex2d( (double)size, pt );
				myVertex2d( (double)size, -pt );
				myVertex2d( pt, -(double)size );
				myVertex2d( -pt, -(double)size );
				myVertex2d( -(double)size, -pt );
				myVertex2d( -(double)size, pt );
				myVertex2d( -pt, (double)size );
			glEnd();
			myPopMatrix();
			
			// Draw some sparks
#if defined( DRAW_SPARKS_ON_FOOD )
			glColor3f( 0.8, 0.8, 1.0 );

			myPushMatrix();
			myTranslate( x, y );
			myRotate( theta );
			
			double sine = sin( stride );
			double cosine = cos( stride );

			glBegin( GL_LINE_STRIP );
				myVertex2d( ((double)size)*cosine,
							((double)size) );
				myVertex2d( 0.0, 
							((double)size)+((double)size)*Abs( sine )/2.0 );
				myVertex2d( ((double)size)*sine/2.0, 
							((double)size)+1.5*((double)size)*Abs( cosine ) );
				myVertex2d( ((double)size)*cosine*1.5,
							0.0 );
				myVertex2d( ((double)size)*drand48(),
							-((double)size)*2.0*sine );
			glEnd();
			myPopMatrix();

			myPushMatrix();
			myTranslate(x,y);
			myRotate( (-theta) );

			sine *=drand48();
			cosine *=drand48();

			glBegin( GL_LINE_STRIP );
				myVertex2d( ((double)size)*cosine,
							((double)size) );
				myVertex2d( 0.0, 
							((double)size)+((double)size)*Abs( sine )/2.0 );
				myVertex2d( ((double)size)*sine/2.0, 
							((double)size)+0.5*((double)size)*Abs( cosine ) );
				myVertex2d( ((double)size)*cosine*1.5,
							0.0 );
				myVertex2d( ((double)size)*drand48(),
							-((double)size)*2.5*cosine );
			glEnd();

			myPopMatrix();
#endif
		}; // Draw

		void Move( void ){
			theta += FOOD_ROTATE_STEP * dt;
			if( theta > 180.0 )
				theta -= 360.0;
			stride += ELECTRIC_STEP * dt;
			if( stride > 180.0 )
				stride -= 360.0;
		}; // Move

}; // Food class.

/*-----------------------------------------------------------------+
	PacMan
 +-----------------------------------------------------------------*/
class Prey : public BasicBug{
	public:
		GLfloat r, b, g;
		bugPts pts[PREY_RESOLUTION+3];
		int numPts;
		double isCharged;

		Prey( void ) : BasicBug() { 
			r = PREY_R;
			g = PREY_G;
			b = PREY_B;
			SetSize( PREY_SIZE );
			theta = 0.0;
			type = PREY_TYPE;
		};
		~Prey( void ){};

		virtual void SetSize( int newSize ){
			size = bx = by = Min( newSize, PREY_MAX_SIZE);
			numPts=Max(PREY_RESOLUTION, 0)+3;
			int i;
			pts[0].x=pts[0].y= 0.0;
			pts[1].x = (double)size;
			pts[1].y = 0.0;
			double step = PI/(2.0*((double)PREY_RESOLUTION));
			double angle = 0.0;
			for( i=1; i<numPts; i++ ){
				pts[i].x = ((double)size)*cos( angle );
				pts[i].y = ((double)size)*sin( angle );
				angle = Min( angle+step, (PI/2.0) );
			} // for
			pts[numPts-1].x = 0.0;
			pts[numPts-1].y = (double)size;
		};

		virtual void EatFood( void ){
			if( foodReaction == MENU_REV ){
				// chase the ghosts.
				isCharged = PREY_CHARGE;
			} else { // grow larger
				SetSize( size+SIZE_STEP );
			}// if/else
		}; // EatFood

		virtual void Draw( void ){
			int i;
			myPushMatrix();
			myTranslate( x, y );
			myRotate( theta );
			if( isCharged > 0.0 ){ // change color
				r = PREY_R * cos( isCharged );
				g = PREY_G * sin( isCharged );
				b = PREY_B * cos( isCharged+45.0 );
				isCharged -= CHARGE_STEP*dt;
				if( isCharged < 0.0 ){
					r = PREY_R;
					g = PREY_G;
					b = PREY_B;
					isCharged = 0.0;
				}
			}
				glColor3f( r, g, b );

			// draw the back half, this never changes.
			glBegin( GL_POLYGON );
				for( i=numPts-1; i>0; i-- )
					myVertex2d( -(pts[i].x), pts[i].y );
				for( i=1; i<numPts; i++ )
					myVertex2d( -(pts[i].x), -(pts[i].y) );
				// thats it.
			glEnd();

			glBegin( GL_POLYGON);
				double stop = Abs(size*cos( stride ) );
				for( i=0; pts[i].y<=stop && i<numPts; i++ )
					myVertex2d( pts[i].y, pts[i].x );	
			glEnd();
			glBegin( GL_POLYGON );
				for( i=0; pts[i].y <= stop && i<numPts; i++ )
					myVertex2d( pts[i].y, -pts[i].x );
			glEnd();

			myPopMatrix();
		}; // Draw


		virtual void Move( void ){
			vx += 0.01*(drand48()-0.5);
			vx = Max( vx, -1.0 );
			vx = Min( vx, 1.0 );
			vy += 0.01*(drand48()-0.5);
			vy = Max( vy, -1.0 );
			vy = Min( vy, 1.0 );
			if( y+by >= winHeight ){
				vy = -( Abs(vy) );
			} else if( y-by < 0.0 ){
				vy = (double)fabs(vy);
			}
			
			if( x+bx > winWidth ){
				vx = -( Abs(vx) );
			} else if( x-bx < 0.0 ) {
				vx = (float)fabs(vx);
			}

			x += (vx*dt);
			y += (vy*dt);
			theta = (180.0/3.14159)*atan2( vy, vx );
			stride += PREY_STEP * dt;
			if( stride > PI )
				stride=0.0;
		};

}; // Prey class

/*-----------------------------------------------------------------+
 +-----------------------------------------------------------------*/
class Predator : public BasicBug{
	public:
		GLfloat r, b, g;
		bugPts pts[PRED_RESOLUTION+5];
		int numPts;
		bugPts cp[PRED_CAPE_RES*3];
		int numCape;
		double flap;

		void SetCape( void )
		{
			numCape = PRED_CAPE_RES*3;
			cp[0].x = (double)size;
			cp[0].y = -2.0*((double)size)/3.0;
			double step = ((double)size)/( ((double)PRED_CAPE_RES) );
			for( int i=1; i<numCape; i++ ){
				if( i%3 == 0 )
					cp[i].x = cp[i-1].x;
				else
					cp[i].x = cp[i-1].x - step;
				if( i%3 == 1 )
					cp[i].y = -4.0*((double)size)/3.0;
				else
					cp[i].y = cp[0].y;
			}// for
			flap = step;
		}; // setCape

		Predator( void ) : BasicBug() { 
			type = PRED_TYPE;
			SetSize( PRED_SIZE );
			/*	I got these colors from PacMan for Nintendo
				running under Nesticle for DOS.  They don't
				match very well though.
			*/
			switch( nextGhost ){
				case GHOST_ORANGE: 
					r = 0.90588;  g = 0.37255;  b = 0.07451;
					nextGhost = GHOST_PURPLE;
					break;
				case GHOST_BLUE:
					r = 0.24706;  g = 0.0;  b = 0.74902;
					nextGhost = GHOST_ORANGE;
					break;		
				case GHOST_PURPLE:
					r = 0.84706;  g = 0.72157;  b = 0.97255;
					nextGhost = GHOST_RED;
					break;
				case GHOST_RED:
				default: // draw red
					r = 0.67059;  g = 0.07451;  b = 0.0;
					nextGhost = GHOST_BLUE;
					break;
			};// switch
		};

		~Predator( void ){};

		virtual void EatFood( void ){
			SetSize( size+SIZE_STEP );
		};

		void Draw( void ){
			int i;
			myPushMatrix();
			myTranslate( x, y );
			myRotate( theta-90.0);
			//if( gmState == REV_GM ){ // draw scared

			//} else { // draw normal
				glColor3f( r, g, b );

				glBegin( GL_POLYGON );
				// draw body
				for( i=0; i<numPts; i++ )
					myVertex2d( pts[i].x, pts[i].y );
				glEnd();

				// draw the cape
				double displace = flap * sin(stride);
				glBegin( GL_TRIANGLES );
				for( i=0; i<numCape; i++ ){
					if( i%3 == 1 )
						myVertex2d( cp[i].x+displace, cp[i].y );
					else
						myVertex2d( cp[i].x, cp[i].y );
				}// end for loop
				glEnd();

#if defined( DRAW_GHOSTS_EYES_WITH_TRIANGLES )
				glColor3f( 1.0, 1.0, 1.0 );
				myTriangle( (2.0*(double)size)/3.0, ((double)size)/4.0,
							((double)size)/4.0, ((double)size)/4.0,
							(3.0*(double)size)/8.0, 0.0					);
				myTriangle( (-2.0*(double)size)/3.0, ((double)size)/4.0,
							(-(double)size)/4.0, ((double)size)/4.0,
							(-3.0*(double)size)/8.0, 0.0					);
				glColor3f( 0.0, 0.0, 0.0 );
				myTriangle( (2.0*(double)size)/3.0, ((double)size)/4.0,
							((double)size)/4.0, ((double)size)/4.0,
							(3.0*(double)size)/8.0, ((double)size)/2.0 	);
				myTriangle( (-2.0*(double)size)/3.0, ((double)size)/4.0,
							(-(double)size)/4.0, ((double)size)/4.0,
							(-3.0*(double)size)/8.0, ((double)size)/2.0 	);
#else
				// Draw the two eyes
					// first the whites of the eyes
				glColor3f( 1.0, 1.0, 1.0 );
				myRect( ((double)size)/4.0, ((double)size)/4.0,
						((double)size)/2.0, 0.0 );
				myRect( (-(double)size)/4.0, ((double)size)/4.0,
						(-(double)size)/2.0, 0.0 );
				// now the black
				glColor3f( 0.0, 0.0, 0.0 );
				myRect( ((double)size)/4.0, ((double)size)/2.0,
						((double)size)/2.0, ((double)size)/4.0 );
				myRect( (-(double)size)/4.0, ((double)size)/2.0,
						(-(double)size)/2.0, ((double)size)/4.0 );
#endif

			//}// else
			myPopMatrix();
		}; // Draw

		void SetSize( int newSize ){
			size = bx = by = Min( newSize, PRED_MAX_SIZE);
			numPts = PRED_RESOLUTION+5;
			int i;
			double step = PI/((double)PRED_RESOLUTION);
			double angle = step;

			pts[0].x = (double)(-size);
			pts[0].y = 0.0;
			pts[1].x = (double)(-size);
			pts[1].y = -2.0*((double)size)/3.0;
			pts[2].x = (double)size;
			pts[2].y = -2.0*((double)size)/3.0;
			pts[3].x = (double)size;
			pts[3].y = 0.0;

			for( i=4; i<(numPts); i++ ){
				pts[i].x = ((double)size)*cos( angle );
				pts[i].y = ((double)size)*sin( angle );
				angle = Min( angle+step, (PI) );
			} // for
			SetCape();
		}; // SetSize

		void Move( void ){
			vx += 0.1*(drand48()-0.5);
			vx = Max( vx, -1.0 );
			vx = Min( vx, 1.0 );
			vy += 0.01*(drand48()-0.5);
			vy = Max( vy, -1.0 );
			vy = Min( vy, 1.0 );

			if( y+by >= winHeight ){
				vy = -( Abs(vy) );
			} else if( y-by < 0.0 ){
				vy = (double)fabs(vy);
			}
			
			if( x+bx > winWidth ){
				vx = -( Abs(vx) );
			} else if( x-bx < 0.0 ) {
				vx = (float)fabs(vx);
			}

			x += (vx*dt);
			y += (vy*dt);
			theta = (180.0/3.14159)*atan2( vy, vx );
			stride += PRED_STEP * dt;
			if( stride > 2.0*PI )
				stride=0.0;
		}; // Move

};  // Predator class


/*-----------------------------------------------------------------+
	PacMan dying.
 +-----------------------------------------------------------------*/
class Extra : public Prey{
	public:
		double nr, ng, nb;
		double death;

		Extra( Prey* old ) : Prey() {
			theta = old->theta;
			nr=r=old->r;  ng=g=old->g;  nb=b=old->b;
			x = old->x;  y = old->y;
			stride = old->stride;
			size = old->size;
			Prey::SetSize( size );
			type = EXTRA_TYPE; 
			death = PI/2.0;
		};

		~Extra( void ) {};

		void Move( void ){
			vx = 0.0;
			vy = 0.0;
			death -= ((double)EXTRA_STEP) * dt;
			r = nr * sin(death*2.0);
			g = ng * sin(death);
			b = nb * sin(death);
		};
}; // Extra

/*-----------------------------------------------------------------+
	Ghost dying.
 +-----------------------------------------------------------------*/
class DyingGhost : public Predator{
	public:
		double nr, ng, nb;
		double death;

		DyingGhost( Predator* old ) : Predator() {
			theta = old->theta;
			nr=r=old->r;  ng=g=old->g;  nb=b=old->b;
			x = old->x;  y = old->y;
			stride = old->stride;
			size = old->size;
			Predator::SetSize( size );
			type = DEAD_GHOST; 
			death = PI/2.0;
		};

		~DyingGhost( void ) {};

		void Move( void ){
			vx = 0.0;
			vy = 0.0;
			death -= ((double)EXTRA_STEP) * dt;
			r = nr * sin(death*2.0);
			g = ng * sin(death);
			b = nb * sin(death*3.0);
		};
}; // Extra
/*-----------------------------------------------------------------+
 +-----------------------------------------------------------------*/
class BugList{
	private:
		BasicBug *head;
		BasicBug *tail;
		int size;
	public:
		BugList( void ){ head=tail=NULL; size=0; };
		~BugList( void ){ DeleteAll(); };

		BasicBug* GetHead( void ){ return head; };

		void Add( BasicBug *add ){
			if( size==0 ){
				head = tail = add;
				add->next = add->prev = NULL;
			} else {
				tail->next = add;
				add->next = NULL;
				add->prev = tail;
				tail = add;
			} // else
			size++;
		}; // Add

		void AddFirst( BasicBug *add ){
			if( size==0 ){
				head = tail = add;
				add->next = add->prev = NULL;
			} else {
				head->prev = add;
				add->prev = NULL;
				add->next = head;
				head = add;
			}
			size++;
		}; // AddFirst


		void Remove( int index ){
			int i;
			BasicBug *t;
			for( t=head, i=0; i<index, t!=NULL; i++, t=t->next ) {};
			if( t!=NULL ){
				if( t->prev )
					t->prev->next = t->next;
				if( t->next )
					t->next->prev = t->prev;
				delete t;
				size--;
			}// if found
		}; // Remove by index.

		BasicBug* operator[](int index){
			BasicBug *t;
			int i;
			for( i=0, t=head; i<index, t!=NULL; i++, t=t->next ){};
			return t;
		}; // operator[]
		

		void DeleteAll( void ){
			while( head ){
				tail=head;
				head=head->next;
				delete tail;
			}// while
			head=tail=NULL;
			size=0;
		}; // DeleteAll

		void MoveAll( void ){
			BasicBug *t;
			BasicBug *w;
			t=head;
			while( t ){
				t->Move();
				if( t->type == EXTRA_TYPE ){
					if( ((Extra*)t)->death < 0.0 ){
						// remove from list.
						w = t;
						t = t->next;

						if( w->prev != NULL ){
							w->prev->next = w->next;
						} else {
							head = w->next;
						}

						if( w->next != NULL ){
							w->next->prev = w->prev;
						} else {
							tail = w->prev;
						}
						delete w;
					} else {
						t = t->next;
					}
				} else if( t->type == DEAD_GHOST ){
					if( ((DyingGhost*)t)->death < 0.0 ){
						// remove from list.
						w = t;
						t = t->next;

						if( w->prev != NULL ){
							w->prev->next = w->next;
						} else {
							head = w->next;
						}

						if( w->next != NULL ){
							w->next->prev = w->prev;
						} else {
							tail = w->prev;
						}
						delete w;
					} else {
						t = t->next;
					}
				} else {
					t=t->next;
				}
			} // while

			// check bounding boxes for everything.
			t = head;
			BasicBug *p;
			BasicBug *d;
			while( t ){
				p = t->next;
				while( p ){
					if( bugsCollide(t,p) ){
						// make the bugs collide!!
						//printf( "Bugs collided!!\n" );
						switch( (t->type | p->type) ){
							case ( PREY_TYPE | EXTRA_TYPE):
							case (PRED_TYPE | EXTRA_TYPE):
							case (FOOD_TYPE | EXTRA_TYPE):
							case (EXTRA_TYPE | EXTRA_TYPE ):
							case (PRED_TYPE | FOOD_TYPE):
							case (DEAD_GHOST ):
							case (DEAD_GHOST | PREY_TYPE ):
							case (DEAD_GHOST | EXTRA_TYPE ):
							case (PRED_TYPE | DEAD_GHOST ):
							case (DEAD_GHOST | FOOD_TYPE ):
								p = p->next;
								break;
							case ( PREY_TYPE | FOOD_TYPE): // prey should eat the food
								// the food disappears
								if( p->type == FOOD_TYPE ){
									// either grow or reverse PacMan
									((Prey*)t)->EatFood();
									d = p;
									p = p->next;
									d->prev->next = d->next;
									if( d->next )
										d->next->prev = d->prev;
									else // next is NULL, so it is the tail
										tail = d->prev;
									delete d;
									break;
								} else { // t is food type, more complicated
									((Prey*)p)->EatFood();
									d = t;
									if( d->prev )
										d->prev->next = d->next;
									else // its the head
										head = d->next;

									d->next->prev = d->prev;
									// set things up again
									t = t->next;
									if(t)
										p = t->next;
									else
										p = NULL;
									delete d;
									break;
								}
								break;
							case ( PREY_TYPE | PRED_TYPE): // pred on prey
								// predator 'kills' the prey
								// add a 'bug' to the list which animates
								//   the prey dying or pred eating.
								// the food disappears
								if( p->type == PREY_TYPE ){
									if(  ((Prey*)p)->isCharged > 0.0 ){
										// delete the predator!!
										DyingGhost* g;
										g = new DyingGhost( (Predator*)t );
										AddFirst( g );
										d = t;
										if( d->prev )
											d->prev->next = d->next;
										else // its the head
											head = d->next;

										d->next->prev = d->prev;
										// set things up again
										t = t->next;
										if(t)
											p = t->next;
										else
											p = NULL;
										delete d;
										break;
									} else { // do normal
										((Predator*)t)->EatFood();
										BasicBug* e;
										e = new Extra( (Prey*)p );
										AddFirst( e );
										d = p;
										p = p->next;
										d->prev->next = d->next;
										if( d->next )
											d->next->prev = d->prev;
										else // next is NULL, so it is the tail
											tail = d->prev;
										delete d;
										break;
									} // if else
									break;
								} else { // t is food type, more complicated
									if( ((Prey*)t)->isCharged > 0.0 ){
										// delete predator!!
										DyingGhost* g;
										g = new DyingGhost( (Predator*)p );
										AddFirst( g );
										d = p;
										p = p->next;
										d->prev->next = d->next;
										if( d->next )
											d->next->prev = d->prev;
										else // next is NULL, so it is the tail
											tail = d->prev;
										delete d;
										break;
									} else { // normal
										((Predator*)p)->EatFood();
										BasicBug *e;
										e = new Extra( (Prey*)t );
										AddFirst(e);
										d = t;
										if( d->prev )
											d->prev->next = d->next;
										else // its the head
											head = d->next;

										d->next->prev = d->prev;
										// set things up again
										t = t->next;
										if(t)
											p = t->next;
										else
											p = NULL;
										delete d;
										break;
									}// else
									break;
								}
								break;
							case ( PREY_TYPE ): // prey on prey collision
							case ( PRED_TYPE ): // pred on pred collision
							case ( FOOD_TYPE ):
							default: // deflect away from each other
								bugDeflect( t, p );	// deflect t by p
								bugDeflect( p, t ); // deflect p by t
								p = NULL;
								break;
						}; // end switch
					} else {
						p = p->next;
					}
				} // check against all others.
				t = t->next;
			}// while( check each )

		}; // MoveAll

		void DrawAll( void ){
			BasicBug *t;
			t=head;
			while( t ){
				t->Draw();
				t=t->next;
			} // while
		};  // DrawAll

		int GetSize( void ){ return size; };

		bool isThere( BasicBug* bug )
		{
			BasicBug *t;
			t=head;
			while( t ){
				if( bugsCollide( bug, t ) )
					return true;
				t = t->next;
			}// while

			return false;
		}; // isThere
}; // BugList


inline bool bugsCollide( BasicBug* b1, BasicBug* b2 )
{
	int w = b1->bx + b2->bx;
	int h = b1->by + b2->by;
	if(  (b1->x < b2->x+w) && (b1->x > b2->x-w ) &&
	     (b1->y < b2->y+h) && (b1->y > b2->y-h )   )
		return true;
	return false;
}

inline void bugDeflect( BasicBug *b1, BasicBug *b2 )
/*
	A function which deflects a bug based on
	where the two bugs collide.  This function 
	assumes that the two bugs have already
	been determined to be colliding.

	It takes into account the current velocity
	of the bugs.  This allows one bug to hit
	another from behind, and only the 'back' bug
	changes direction.
*/
{
	int w = b1->bx + b2->bx;
	int h = b1->by + b2->by;
	
	// determine x deflection
	if( ( b1->x > (b2->x - b2->bx) ) &&
		( b1->x < (b2->x + b2->bx) )    ){
		if( Sign(b1->vy) == Sign( (b2->y - b1->y) ) )
			b1->vy = -(b1->vy);
	// determine y deflection
	} else if(  (b1->y > (b2->y - b2->by)) &&
				(b1->y < (b2->y + b2->by))     ){
		if( Sign(b1->vx) == Sign( (b2->x - b1->x) ) )
			b1->vx = -(b1->vx);
	} else {
		if( Sign(b1->vx) == Sign( (b2->x - b1->x) ) )
			b1->vx = -(b1->vx);
		if( Sign(b1->vy) == Sign( (b2->y - b1->y) ) )
			b1->vy = -(b1->vy);
	}// end if.. else if.. else
}
#endif
