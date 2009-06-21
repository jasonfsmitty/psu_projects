(* Jason F. Smith 4588 *)
(*  CSE 428 HW1        *)


(* ==== #1. n!!=dblfact(n) ==== *)
fun dblfact(0) = 1 
  | dblfact(1) = 1 
  | dblfact(n) = n * dblfact(n-2);


(* ==== #2. pochhammer's symbol ==== *)
fun pochhammer(z,0) = 1 
  | pochhammer(z,n) = (z+n-1) * pochhammer(z,n-1);


(* ==== #3. Real.Math.exp approximation (n=30) ==== *)
fun rpower(n,0) = 1.0
  | rpower(n,m) = real(n) * rpower(n,m-1);

fun rfact(0) = 1.0 
  | rfact(n) = real(n) * rfact(n-1);

fun eapprox(z,0) = 1.0 
  | eapprox(z,n) = (rpower(z,n)/rfact(n)) + eapprox(z,n-1);

(* here is the final function *)
fun expapprox(z) = eapprox(z,30);


(* ==== #4.  primetest ==== *)
local
  fun isDivisor(n,d) = ((n mod d)=0 );

  fun GreatestDivisor(n,0) = 1
    | GreatestDivisor(n,n) = GreatestDivisor(n,n-1)
    | GreatestDivisor(n,s) = if( isDivisor(n,s) ) then s else GreatestDivisor(n,s-1);
in
  fun primetest(0) = 0
    | primetest(n) = ((GreatestDivisor(n,n-1) = 1);
end


(* ==== #5.  zip(f,n,m) ==== *)

fun sumRow(f,n,0) = f(n,0)
  | sumRow(f,n,m) = f(n,m) + sumRow(f,n,m-1);

fun sumCol(f,0,m) = f(0,m)
  | sumCol(f,n,m) = f(n,m) + sumCol(f,n-1,m);

(* actual zip function *)
fun zip(f,n,m) = sumRow(f,n,m) + sumCol(f,n,m) - f(n,m) + zip(f,n-1,m-1)
  | zip(f,0,m) = sumRow(f,0,m)
  | zip(f,n,0) = sumCol(f,n,0)
  | zip(f,0,0) = f(0,0);