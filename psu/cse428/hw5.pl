/*---------------------------------------------------------+

	Jason F. Smith
	CSE 428
	Homework #5

        Notice:  I've added in the ability to use
          'who' when asking questions.  The two cases
          are explained as follows:

          'who is X' - exact same as 'what is X'

          'who is a X' - querys the knowledge base to find
             all Y's so that 'Y is a X'
             for example, try 'who is a person' to return
             all 'persons' from the knowledge base.  does
             not return 'a person is a mammal' or anything
             in similiar format since they would not answer
             the question.
 +---------------------------------------------------------*/

/*--- the knowledge base ---*/
kb([
        [homer, is, a, man],
        [marge, is, a, woman],
        [krusty, is, a, clown],
        [a, man, is, a, person],
        [a, clown, is, a, person],
        [a, woman, is, a, person],
        [a, person, is, a, mammal],
        [snowball, is, a, cat],
        [santaslittlehelper, is, a, dog],
        [itchy, is, a, mouse],
        [scratchy, is, a, cat],
        [a, dog, is, a, mammal],
        [a, cat, is, a, mammal],
        [a, dog, is, a, pet],
        [a, cat, is, a, pet],
        [a, mouse, is, a, mammal],

        [jason, is, a, comp_sci_major],
        [a, comp_sci_major, is, a, cse428_student],
        [a, comp_sci_major, is, a, student_at_psu],
        [a, student_at_psu, is, a, bankrupt_twentysomething_year_old ],
        [a, cse428_student, is, a, sml_expert],
        [a, cse428_student, is, a, prolog_expert],
        [a, cse428_student, is, a, psu_womens_volleyball_fanatic],
        [a, student_at_psu, is, a, person],

        [a, mammal, is, a, animal],
        [a, animal, is, a, being],
        [a, being, is, a, life]
]).

/*-----------------------------------+
   The prove predicate proves takes
   a simple question and determines
   if an answer can be found in the
   database.
 +-----------------------------------*/

prove( [is, X, a, Y] ) :-
  kb(L), member( [X, is, a, Y], L ).
prove( [is, X, a, Y] ) :-
  kb(L),
  member( [X, is, a, Z], L),
  prove( [is, Z, a, Y] ).

prove( [is, X, a, Y] ) :-
  kb(L), member( [a, X, is, a, Y], L ).
prove( [is, X, a, Y] ) :-
  kb(L),
  member( [a, X, is, a, Z], L ),
  prove( [is, Z, a, Y] ).


/*--------------------------------------+
	The ask(Q,A) predicate.
	Used to query the knowledge base.
 +--------------------------------------*/

/* handle 'what is a _' cases */

ask( [what, is, a, X], [a, X, is, a, Y] ) :-
  kb(L),
  member( [a, X, is, a, Y], L ).
ask( [what, is, a, X], [a, X, is, a, Z] ) :-
  kb(L),
  member( [a, X, is, a, Y], L ),
  ask( [what, is, a, Y], [a, Y, is, a, Z] ).


/* handle 'what is _' cases */

ask( [what, is, X], [X, is, a, Y] ) :-
  kb(L),
  member( [X, is, a, Y], L ).
ask( [what, is, X], [X, is, a, Z] ) :-
  kb(L),
  member( [X, is, a, Y], L ),
  ask( [what, is, a, Y], [a, Y, is, a, Z] ).

/*------------------------------------+
   handle questions about a person.
   'who is X' is equal to 'what is X'
 +------------------------------------*/
ask( [who, is, X], A ) :- ask( [what, is, X], A ).

/*------------------------------------+
   query about who is something
   for example, 'who is a person'
 +------------------------------------*/
ask( [who, is, a, X], [Y, is, a, X] ) :-
   ask( [what, is, Y], [Y, is, a, X] ).  


/* handle all other ('is _ a _') */

ask(Q,yes) :- prove(Q).
ask(Q,no) :- not( prove(Q) ).


/*-- EOF --*/
