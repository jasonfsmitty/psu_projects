/*---------------------------------------------------------------------------------------------+
	Jason F Smith   4588
	CSE 428  Homework 6

	Zebra problem:  five people, five houses, five pets, five motorcycles, and five drinks.

 +----------------------------------------------------------------------------------------------*/
/* Helper functions for the program */

/* member(H,L) is already a library function */

/*---  nth(N,L,V) -> V is the Nth element of L  ---*/
nth(0, [V|_], V ).
nth(N,[_|Ls],V) :- nth( N-1, [Ls], V ).

/*--- right(A,B,L) -> B is to the right of A in L (or A to the left, same thing) ---*/
right(A, B, [A,B,_,_,_] ).
right(A, B, [_,A,B,_,_] ).
right(A, B, [_,_,A,B,_] ).
right(A, B, [_,_,_,A,B] ).

/*--- nextto(A,B,L) -> A and B are next to each other in list L ---*/
nextto(A,B,L) :- right(A,B,L).
nextto(A,B,L) :- right(B,A,L).

/*--------------------------------------------------------+
	The actual zebra predicate.
	The list is arranged as follows:
	[ nationality, pet, drink, color, motorcycle ]
	Valid values for:
	(1) Nationalities -> Englishwoman, Spaniard, 
			 Ukrainian, Norwegian, Japanese
	(2) Pet -> Zebra, Dog, Snails, Fox, Horse
	(3) Drink -> Coffee, Tea, Milk, OrangeJuice, (?)Vodka
	(4) Color (of house) -> Red, Green, Ivory, Yellow, Blue
	(5) Motorcycle -> Harley, Suzuki, Ducati, Kawasaki, BMW

 +--------------------------------------------------------*/

zebra( H ) :- H = 
	[ [norwegian,_,_,_,_],_,[_,_,milk,_,_],_,_],
	member( [englishwoman,_,_,red,_], H ),
	member( [spaniard,dog,_,_,_], H ),
	member( [_,_,coffee,green,_], H ),
	member( [ukrainian,_,tea,_,_], H ),
	right( [_,_,_,ivory,_], [_,_,_,green,_], H ),
	member( [_,snails,_,_,harley], H ),
	member( [_,_,_,yellow,suzuki], H ),
	nextto( [_,_,_,_,ducati], [_,fox,_,_,_], H ), 
	nextto( [_,_,_,_,suzuki], [_,horse,_,_,_], H ),
	member( [_,_,orangeJuice,_,kawasaki], H ), 
	member( [japanese,_,_,_,bmw], H ),
	nextto( [norwegian,_,_,_,_], [_,_,_,blue,_], H ).
