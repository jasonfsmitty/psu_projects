(*----------------------------
   Jason F. Smith 4588
   CSE 428  Fall 2000
   Assignment #2
------------------------------*)


local   (* some useful function definitions *)

  (* nth(L,i) return the ith element of list L (start counting at 1) *)
  fun nth(x::xs, 1) = x
    | nth(x::xs, n) = nth(xs,n-1);

  (* all(p,L) returns true if p is true of all elements in list L *)
  fun all(p,nil) = true
    | all(p,x::xs) = p(x) andalso all(p,xs)

  (* take(n,L) returns the first n elements of L *)
  fun take(0,xs) = nil
    | take(n,x::xs) = x::take(n-1,xs);

  (* drop(n,L) skips over the first n elements of L and returns the
     remaining list *)
  fun drop(0,xs) = xs
    | drop(n,x::xs) = drop(n-1,xs);

  fun map(f,nil) = nil
    | map(f,x::xs) = f(x) :: map(f,xs);

  (* zip(f,[x1,x2,...,xn][y1,y2,...,yn]) returns 
     	[f(x1,y1),f(x2,y2),...,f(xn,yn)] *)
  fun zip(f,nil,nil) = nil
    | zip(f,x::xs,y::ys) = f(x,y)::zip(f,xs,ys)

  (* Take the length of a list *)
  fun length( x::nil ) = 1
    | length( x::xs ) = 1 + length(xs);

  (* Add two vectors together *)
  fun addvectors( nil, nil ) = nil
    | addvectors( x::xs, y::ys ) = (x+y)::addvectors( xs, ys );

  (* Recursive definition to check for legality.  Returns the length
     of the row(s), or -1 if not legal. *)
  fun lengthlegal( nil ) = 0
    | lengthlegal( x::nil) = length(x)
    | lengthlegal( x::xs ) = if length(x)=lengthlegal(xs) then 
				length(x)
			     else 0;

  (* create an nxn matrix containing k everywhere *)
  fun create(n,k) =
    let fun mkrow 0 = nil
          | mkrow m = k::mkrow(m-1);
        fun allrows 0 = nil
          | allrows m = (mkrow n) :: allrows (m-1)
    in
	allrows n
    end;

  (* function to take a matrix of zeros and row by row add in the ones to make
     it into an identity matrix *)
  fun makeidentityrow( nil,_) = nil
    | makeidentityrow( x::xs, n) = 
		    ( take(n-1,x) @ [1] @ drop(n, x) ) :: makeidentityrow( xs, n+1);


  (* Function to compute the dot product of two vectors *)
  fun dot( nil, nil ) = 0
    | dot(x::xs, y::ys) = x*y + dot(xs, ys);

  (* function to check for an identity row *)
  fun isidentityrow( nil, 0 ) = true
    | isidentityrow(x::xs, 0 ) = if x=0 then
					isidentityrow( xs, 0 )
				 else false
    | isidentityrow(x::xs, 1 ) = if x=1 then
					isidentityrow( xs, 0 )
 				 else false
    | isidentityrow(x::xs, n ) = if x=0 then
					isidentityrow( xs, n-1 )
				 else false

  (* check if identity matrix, originally  assumed to be a square matrix (nxn) *)
  fun checkidentity( nil ) = true
    | checkidentity( x::xs ) = isidentityrow(x, length(x)-length(x::xs)+1 ) andalso checkidentity( xs );

  (* function to reverse a list *)
  fun rev(nil, ys) = ys
    | rev(x::xs, ys) = rev(xs, x::ys);

  (* Get all of the columns of a matrix - equal to the transpose. *)
  fun getcolumns( _, 0 ) = nil
    | getcolumns( x, n ) = col(x, n)::getcolumns(x,n-1);

in (***** Begin public functions *****)

  (*-- Determines if all rows in the matrix have the same length --*)
  fun legal(x) = if lengthlegal(x)=0 then false
		 else true;

  (*-- Returns the ith row of the matrix --*)
  fun row(M,n) = nth(M,n);

  (*-- returns the ith column of the matrix --*)
  fun col(nil,n) = nil
    | col(x::xs, n) = nth(x,n)::col(xs,n);

  (*-- Determine the dimensions of a matrix --*)
  fun dimensions(x::xs) = (length(x), length( col(x::xs,1) ) );

  (*-- Access the element at row i, column j in matrix M --*)
  fun access(M,i,j) =  nth( nth(M,i), j );

  (*-- Returns the identity matrix of size nxn --*)
  fun identity(n) = makeidentityrow( create(n,0), 1 );


  (*-- Determines if matrix M is an identity matrix --*)
  fun isidentity(nil) = false
    | isidentity(x::xs) = if legal(x::xs) andalso length(x)=length(x::xs) then 
			  checkidentity( x::xs )
			else 
			  false;

  (*-- Takes the transpose of the matrix M --*)
  fun transpose( x::xs ) = 
	 rev( getcolumns(x::xs, length(x) ), nil );

  (*-- Add two matrices together --*)
  fun add( nil, nil )  = nil
    | add( x::xs, y::ys ) = addvectors(x, y)::add(xs,ys); 


  (*-- Multiplies the two matrices --*)
  fun multiply( nil, y::ys ) = nil
    | multiply( x::xs, y::ys ) = 
		map( fn f=>dot(x,f), transpose(y::ys) )::multiply( xs, y::ys );

end; (* local *)
