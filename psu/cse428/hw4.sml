(*-------------------------------
	Jason F Smith  4588
	CSE 428 - Fall 2000
	Assignment #4
  -------------------------------*)

Compiler.Control.printWarnings := false;

datatype Ty = BoolTy | IntTy | times of Ty * Ty | arrow of Ty * Ty | NoTy;

datatype Oper = PlusOp  |  MinusOp  |  LTOp  |  GTOp  |  EqualOp;

type Id = string;
datatype Exp = IntConst of int |   BoolConst of bool
           |   Var of string
           |   Op of (Exp * Oper * Exp)
           |   If of (Exp * Exp * Exp)
           |   Let of (Dec * Exp) 
           |   Pair of (Exp * Exp)
	     |   Fst of Exp
	     |   Snd of Exp
	     |   Apply of (Id * (Exp list))

      and Dec = ValDec of (Id * Exp) | FunDec of (Id * ((Id * Ty) list) * Exp);


datatype Val = IntVal of int  |  BoolVal of bool | PairVal of (Val * Val)
             | Closure of (Id * Exp);


datatype Instr = add 
               | br of (Instr list) * (Instr list)
		   | call of int
               | con of int 
               | eq 
               | fpop
		   | fpush of (Instr list)
               | get of int
		   | gt
               | lt
               | pop of int 
               | push of int 
               | sub;



local

type Code = Instr list;
type Stack = int list;
type Env = int list;
type Fenv = Code list;
type Stack = (Code * Stack * Env * Fenv);

fun nth(x::xs, 1) = x
    | nth(x::xs, n) = nth(xs,n-1);

exception Undeclared;
fun find( x, nil ) = raise Undeclared
  | find( x, y::ys ) = ( if x=y then 1 else (1+find(x,ys)) );

fun length( x::nil ) = 1
  | length( x::xs ) = 1 + length(xs);

fun transoper PlusOp = add
  | transoper MinusOp = sub
  | transoper GTOp = gt
  | transoper LTOp = lt
  | transoper EqualOp = eq;

fun compMap( f, nil ) = nil
  | compMap( f, x::xs ) = f(x) @ compMap(f,xs);

fun comp( IntConst n, vars, funs ) = [con n]
  | comp( BoolConst true, vars, funs ) = [con 1]
  | comp( BoolConst false, vars, funs ) = [con 0]
  | comp( Var x, vars, funs) = [get (find(x,vars)-1)]
  | comp( Op(e1,oper,e2), vars, funs ) = 
	let val cd1 = comp( e1, vars, funs )
	    val cd2 = comp( e2, vars, funs )
	in 
	    cd1 @ cd2 @ [transoper oper]
	end

  | comp( If(e1,e2,e3), vars, funs ) =
	let val cd1 = comp( e1, vars, funs )
	    val cd2 = comp( e2, vars, funs )
	    val cd3 = comp( e3, vars, funs )
	in
	    cd1 @ [ br(cd2,cd3)]
	end

  | comp( Let( FunDec(f, args, e1), e2 ), vars, funs ) =
	let val vars' = compdec( FunDec(f,args,e1), vars );
	    val funs' = f::funs
	in
	    [fpush( comp(e1, vars', funs') )] @ comp( e2, vars', funs' ) @ [fpop]
	end

  | comp( Let( ValDec(x,e1), e2), vars, funs ) = 
	let val cd1 = comp( e1, vars, funs );
	    val cd2 = comp( e2, x::vars, funs )
	in
		cd1 @ (push(1)::cd2) @ [pop(1)]
	end

  | comp( Apply( f, args ), vars, funs ) = 
	let val cd1 = compMap( fn w=>comp(w,vars,funs), args );
	    val n = length(args)
	in
	    cd1 @ [push(n)] @ [call (find(f,funs)-1)] @ [pop(n)]
	end
 
and compdec( FunDec( f, nil, e ), vars ) = vars
  | compdec( FunDec( f, (a,t)::args, e ), vars ) = compdec( FunDec(f, args, e), vars ) @ [a];


fun exec(nil, v::S, env, fenv) = v
  | exec( (con n)::P, S, env, fenv ) = exec( P, n::S, env, fenv )
  | exec( (get k)::P, S, env, fenv ) = exec( P, nth(env,k+1)::S, env, fenv )
  | exec( add::P, n2::n1::S, env, fenv ) = exec( P, (n1+n2)::S, env, fenv )
  | exec( sub::P, n2::n1::S, env, fenv ) = exec( P, (n1-n2)::S, env, fenv )
  | exec( gt::P, n2::n1::S, env, fenv ) = exec( P, (if n1>n2 then 1 else 0)::S, env, fenv )
  | exec( lt::P, n2::n1::S, env, fenv ) = exec( P, (if n1<n2 then 1 else 0)::S, env, fenv )
  | exec( eq::P, n2::n1::S, env, fenv ) = exec( P, (if n1=n2 then 1 else 0)::S, env, fenv )
  | exec( br(P1,P2)::P, 0::S, env, fenv ) = exec( P2@P, S, env, fenv )
  | exec( br(P1,P2)::P, 1::S, env, fenv ) = exec( P1@P, S, env, fenv )
  | exec( push(1)::P, v::S, env, fenv ) = exec( P, S, v::env, fenv )
  | exec( push(n)::P, v::S, env, fenv ) = exec( push(n-1)::P, S, v::env, fenv )
  | exec( pop(1)::P, S, v::env, fenv ) = exec( P, S, env, fenv )
  | exec( pop(n)::P, S, v::env, fenv ) = exec( pop(n-1)::P, S, env, fenv )
  | exec( fpush(f)::P, S, env, fenv ) = exec( P, S, env, f::fenv )
  | exec( fpop::P, S, env, f::fenv ) = exec( P, S, env, fenv )
  | exec( call(k)::P, S, env, fenv ) = exec( nth(fenv,k+1)@P, S, env, fenv );

in
	fun compile e = comp(e,nil, nil);
	fun execute cd = exec( cd, nil, nil, nil );
	fun run e = execute( compile(e) );

end (* local *)
