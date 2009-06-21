/* CSE 428, Fall 2000    */
/* Assignment 6 Solution */
/* John Hannan           */

/* The small Zebra Problem */

 smallzebra([(N1,P1,D1),(N2,P2,D2),(N3,P3,D3)]) :-
	D2=milk,              /* person in 2nd house drinks milk */

	N1=norwegian,         /* norwegian lives in the first house */

	( N1=spaniard,P1=dog; /* spaniard owns the dog */
	  N2=spaniard,P2=dog;
	  N3=spaniard,P3=dog ),

	( N1=ukranian,D1=tea; /* ukranian drinks tea */
	  N2=ukranian,D2=tea;
	  N3=ukranian,D3=tea ),

	( D1=juice,P1=fox;    /* juice drinker owns fox */
	  D2=juice,P2=fox;
	  D3=juice,P3=fox ).

/***********************************************************************/


bigzebra(Houses) :-
   length(Houses,5),
   nth(3,Houses,(_,_,milk,_,_)),
   nth(1,Houses, (norwegian,_,_,_,_)),
   member((englishman,_,_,red,_),Houses),
   member((spaniard,dog,_,_,_),Houses),
   member((_,_,coffee,green,_),Houses),
   member((ukranian,_,tea,_,_),Houses),
   right((_,_,_,ivory,_),(_,_,_,green,_),Houses),
   member((_,snails,_,_,harley),Houses),
   member((_,_,_,yellow,suzuki),Houses),
   nextto((_,_,_,_,ducati),(_,fox,_,_,_),Houses),
   nextto((_,_,_,_,suzuki),(_,horse,_,_,_),Houses),
   member((_,_,juice,_,kawaski),Houses),
   member((japanese,_,_,_,bmw),Houses),
   nextto((norwegian,_,_,_,_),(_,_,_,blue,_),Houses).

/* we can add these last two constraints to specify what the fifth
   drink and fifth pet are */
/* member((_,_,water,_,_), Houses),
   member((_,zebra,_,_,_), Houses). */


/* Auxiliary Predicates */

nth(1,[H|_],H).
nth(N,[_|Hs],H) :- M is N-1, nth(M,Hs,H).

right(A,B,[A,B|_]).
right(A,B,[_|Y]) :- right(A,B,Y).

nextto(A,B,L) :- right(A,B,L) ; right(B,A,L).


