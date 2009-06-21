(*--------------------------------
	Jason F Smith 4588
	CSE 428 HW 3
 ---------------------------------*)


(*--------------- The various data types used by the meta language -------------*)

type Id = string;

datatype Oper = PlusOp 
              | MinusOp 
              | LTOp
              | GTOp
              | EqualOp;

datatype Ty = BoolTy 
            | IntTy
            | times of Ty * Ty
            | arrow of Ty * Ty
            | NoTy;

datatype Exp = IntConst of int
             | BoolConst of bool
	     | Var of string
             | Op of (Exp * Oper * Exp)
	     | If of (Exp * Exp * Exp )
             | Let of ( Dec * Exp )
             | Pair of (Exp * Exp)
             | Fst of (Exp)
             | Snd of (Exp)
	     | Apply of (Id * Exp )
	and Dec = ValDec of (Id * Exp) | FunDec of (Id * Id * Ty * Exp );

datatype Val = IntVal of int
             | BoolVal of bool
             | PairVal of (Val * Val)
             | Closure of (Id * Exp);


(*---------- Exceptions used in the typecheck and eval functions -----------*)
exception Untypeable;
exception Undeclared;

(*------------------ Various helper functions -----------------------*)

fun extend(senv, x, t) = (x,t)::senv;

fun lookup( x, nil ) = raise Undeclared
  | lookup( x, (y,t)::senv ) = 
	if x=y then t else lookup(x,senv);


(*---------------- The extended declaration of tc/typecheck. -----------------*)

local
	fun tc( IntConst n, senv ) = IntTy
          | tc( BoolConst b, senv ) = BoolTy
          | tc( Op(e1, EqualOp, e2), senv) =
		let val t1 = tc(e1,senv);
		    val t2 = tc(e2,senv);
		in
		    if t1=t2
		    then BoolTy
		    else raise Untypeable
		end
          | tc( Op(e1, BinOp, e2), senv )=
		let val t1 = tc( e1, senv );
		    val t2 = tc( e2, senv );
                in
		    if t1=IntTy andalso t2=IntTy
		    then if (BinOp=PlusOp orelse BinOp=MinusOp)
			then IntTy else BoolTy
		    else raise Untypeable
		end
          | tc( Var v, senv ) = (lookup( v, senv )
		handle Undeclared => raise Untypeable)
          | tc( If( e1, e2, e3 ), senv ) =
		let val t1 = tc( e1, senv );
		    val t2 = tc( e2, senv );
		    val t3 = tc( e3, senv );
		in
		    if t1=BoolTy andalso (t2=t3)
		    then t2
		    else raise Untypeable
		end
          | tc( Let( ValDec(x,e1), e2 ), senv ) =
		    tc( e2, extend(senv, x, tc(e1, senv) ) )
	  | tc( Let( FunDec(f,x,t1,e1), e2), senv ) =
		let val senv2 = (x,t1)::senv;
		in
		    tc( e2, extend( senv2, f, arrow(t1,tc(e1,senv2)) ))
		end
	  | tc( Pair( e1, e2 ), senv ) = 
		let val t1 = tc( e1, senv );
		    val t2 = tc( e2, senv );
		in
		    times( t1,t2 )
		end
	  | tc( Fst( e1 ), senv ) =
		let val times(x,y) = tc( e1, senv )
		in
		    x
		end
	  | tc( Snd( e ), senv ) = 
		let val times(x,y) = tc(e,senv)
		in
		    y
		end
	  | tc( Apply( id, e1 ), senv)  = 
		let val arrow(t1,t2) = lookup(id, senv)
		in
		    t2
		end;
in

	fun typecheck e = tc(e,nil) handle Untypeable => NoTy;

end; (* local *)


(*------------------ Extended definition of eval. --------------------*)

local
	fun Operate( IntVal n1, PlusOp, IntVal n2 ) = IntVal( n1+n2 )
          | Operate( IntVal n1, MinusOp, IntVal n2 ) = IntVal( n1-n2 )
          | Operate( IntVal n1, LTOp, IntVal n2 ) = BoolVal( n1<n2 )
          | Operate( IntVal n1, GTOp, IntVal n2 ) = BoolVal( n1>n2 )
          | Operate( n1, EqualOp, n2 ) = BoolVal( n1=n2 );

	fun ev( IntConst n, env ) = IntVal n
          | ev( BoolConst b, env ) = BoolVal b
          | ev( Var x, venv ) = lookup( x, venv )
          | ev( Op(e1,oper,e2), venv ) = 
		let val v1 = ev( e1, venv );
		    val v2 = ev( e2, venv );
		in
		    Operate( v1, oper, v2 )
		end
          | ev( If(e1,e2,e3), venv ) = 
		let val BoolVal v1 = ev( e1, venv )
		in
		    if v1 then ev( e2, venv )
		          else ev( e3, venv )
		end
          | ev( Let( ValDec(x,e1), e2 ), venv ) = 
		let val v1 = ev( e1, venv );
		    val v2 = ev( e2, extend(venv, x, v1 ));
		in
		    v2
		end
	  | ev( Let( FunDec(f,x,t1,e1), e2), venv ) =
		let val venv2 = extend(venv, f, Closure(x, e1));
		in
		    ev( e2, venv2)
		end
	  | ev( Pair( e1, e2 ), venv ) =
		let val v1 = ev( e1, venv );
		    val v2 = ev( e2, venv );
		in
		    PairVal( v1, v2 )
		end
	  | ev( Fst( e ), venv ) =
		let val PairVal(v1,v2) = ev( e, venv )
		in
		    v1
		end
	  | ev( Snd( e ), venv ) =
		let val PairVal(v1,v2) = ev( e, venv )
		in
		    v2
		end
	  | ev( Apply( id, e1 ), venv )  =
		let val e2 = lookup( id, venv );
		    val v1 = ev( e1, venv );
		    fun GetVar( Closure( x, e ) ) = x;
		    fun GetExp( Closure( x, e ) ) = e;
		in
		    ev( GetExp(e2), extend( venv, GetVar(e2), v1 ) )
		end;

in

	fun eval e = ev(e,nil);

end; (* local *)

(*- EOF -*)
