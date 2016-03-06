package jsy.student

object Lab3 extends jsy.util.JsyApplication {
  import jsy.lab3.Parser
  import jsy.lab3.ast._

  /*
   * CSCI 3155: Lab 3
   * Alec Thilenius
   *
   * Partner: <Your Partner's Name>
   * Collaborators: <Any Collaborators>
   */

  type Env = Map[String, Expr]
  val emp: Env = Map()
  def get(env: Env, x: String): Expr = env(x)
  def extend(env: Env, x: String, v: Expr): Env = {
    require(isValue(v))
    env + (x -> v)
  }

  /*
   * The implementations of these helper functions for conversions can come
   * Lab 2. The definitions for the new value type for Function are given.
   */

  def toNumber(v: Expr): Double = {
    require(isValue(v))
    (v: @unchecked) match {
      case N(n) => n
      case B(false) => 0
      case B(true) => 1
      case Undefined => Double.NaN
      case S(s) => try s.toDouble catch { case _ => Double.NaN }
      case Function(_, _, _) => Double.NaN
    }
  }

  def toBoolean(v: Expr): Boolean = {
    require(isValue(v))
    (v: @unchecked) match {
      case B(b) => b
      case Function(_, _, _) => true
      case N(n) if n == 0.0 || n.isNaN => false
      case N(_) => true
      case S("") => false
      case S(_) => true
      case Undefined => false
    }
  }

  def toStr(v: Expr): String = {
    require(isValue(v))
    (v: @unchecked) match {
      case N(n) => n.toString
      case B(b) => b.toString
      case Undefined => "undefined"
      case S(s) => s
      case Function(_, _, _) => "function"
    }
  }

  /*
   * Helper function that implements the semantics of inequality
   * operators Lt, Le, Gt, and Ge on values.
   *
   * We suggest a refactoring of code from Lab 2 to be able to
   * use this helper function in eval and step.
   */
  def inequalityVal(bop: Bop, v1: Expr, v2: Expr): Boolean = {
    require(isValue(v1))
    require(isValue(v2))
    require(bop == Lt || bop == Le || bop == Gt || bop == Ge)
    (v1, v2) match {
      case (S(s1), S(s2)) =>
        (bop: @unchecked) match {
          case Lt => s1 < s2
          case Le => s1 <= s2
          case Gt => s1 > s2
          case Ge => s1 >= s2
        }
      case _ =>
        val (n1, n2) = (toNumber(v1), toNumber(v2))
        (bop: @unchecked) match {
          case Lt => n1 < n2
          case Le => n1 <= n2
          case Gt => n1 > n2
          case Ge => n1 >= n2
        }
    }
  }

  /* Big-Step Interpreter with Dynamic Scoping */

  /*
   * This code is a reference implementation of JavaScripty without
   * strings and functions (i.e., Lab 2).  You are to welcome to
   * replace it with your code from Lab 2.
   */
  def eval(env: Env, e: Expr): Expr = {
    def eToN(e: Expr): Double = toNumber(eval(env, e))
    def eToB(e: Expr): Boolean = toBoolean(eval(env, e))
    def eToVal(e: Expr): Expr = eval(env, e)
    e match {
      case _ if isValue(e) => e
      case Var(x) => get(env, x)
      case Print(e1) =>
        println(pretty(eval(env, e1))); Undefined
      case Call(e1, e2) => eToVal(e1) match {
        case Function(None, x, ebody) => eval(extend(env, x, eToVal(e2)), ebody)
        case Function(Some(x1), x2, ebody) =>
          val env1 = extend(env, x1, Function(Some(x1), x2, ebody))
          val env2 = extend(env1, x2, eToVal(e2))
          eval(env2, ebody)
        case _ => throw DynamicTypeError(e)
      }
      case Unary(Neg, e1) => N(-eToN(e1))
      case Unary(Not, e1) => B(!eToB(e1))
      case Binary(Minus, e1, e2) => N(eToN(e1) - eToN(e2))
      case Binary(Times, e1, e2) => N(eToN(e1) * eToN(e2))
      case Binary(Div, e1, e2) => N(eToN(e1) / eToN(e2))
      case Binary(Eq, e1, e2) => (eToVal(e1), eToVal(e2)) match {
        case (Function(_, _, _), _) => throw DynamicTypeError(e)
        case (_, Function(_, _, _)) => throw DynamicTypeError(e)
        case (v1, v2) => B(v1 == v2)
      }
      case Binary(Ne, e1, e2) => (eToVal(e1), eToVal(e2)) match {
        case (Function(_, _, _), _) => throw DynamicTypeError(e)
        case (_, Function(_, _, _)) => throw DynamicTypeError(e)
        case (v1, v2) => B(v1 != v2)
      }
      case Binary(Plus, e1, e2) => (eToVal(e1), eToVal(e2)) match {
        case (S(s1), v2) => S(s1 + toStr(v2))
        case (v1, S(s2)) => S(toStr(v1) + s2)
        case (v1, v2) => N(toNumber(v1) + toNumber(v2))
      }
      case Binary(Lt, e1, e2) => B(eToN(e1) < eToN(e2))
      case Binary(Le, e1, e2) => B(eToN(e1) <= eToN(e2))
      case Binary(Gt, e1, e2) => B(eToN(e1) > eToN(e2))
      case Binary(Ge, e1, e2) => B(eToN(e1) >= eToN(e2))
      case Binary(And, e1, e2) if isValue(e1) => if (toBoolean(e1)) e2 else e1
      case Binary(Or, e1, e2) if isValue(e1) => if (toBoolean(e1)) e1 else e2
      case Binary(Seq, e1, e2) =>
        eToVal(e1); eToVal(e2)
      case If(e1, e2, e3) => if (eToB(e1)) eToVal(e2) else eToVal(e3)
      case ConstDecl(x, e1, e2) => eval(extend(env, x, eToVal(e1)), e2)
    }
  }

  /* Small-Step Interpreter with Static Scoping */

  def substitute(e: Expr, v: Expr, x: String): Expr = {
    require(isValue(v))
    /* Simple helper that calls substitute on an expression
     * with the input value v and variable name x. */
    def subst(e: Expr): Expr = substitute(e, v, x)
    /* Body */
    e match {
      case B(_) => e
      case Binary(op, e1, e2) => Binary(op, subst(e1), subst(e2))
      case Call(e1, e2) => Call(subst(e1), subst(e2))
      case ConstDecl(y, e1, e2) =>
        ConstDecl(y, subst(e1), if (x == y) e2 else subst(e2))
      case Function(None, y, e1) =>
        if (x == y) e else Function(None, y, subst(e1))
      case Function(Some(y1), y2, e1) =>
        if (x == y1 || x == y2) e else Function(Some(y1), y2, subst(e1))
      case If(e1, e2, e3) => If(subst(e1), subst(e2), subst(e3))
      case N(_) => e
      case Print(e1) => Print(subst(e1))
      case S(_) => e
      case Unary(uop, e1) => Unary(uop, subst(e1))
      case Undefined => e
      case Var(y) => if (x == y) v else e
    }
  }

  def step(e: Expr): Expr = {
    e match {
      case Print(v1) if (isValue(v1)) => println(pretty(v1)); Undefined
      /* Base Cases: Do Rules */
      case Binary(And, v1, e2) if isValue(v1) => if (toBoolean(v1)) e2 else v1
      case Binary(Div, v1, v2) if isValue(v1) && isValue(v2) =>
        N(toNumber(v1) / toNumber(v2))
      case Binary(Eq | Ne, Function(_, _, _), _) => throw DynamicTypeError(e)
      case Binary(Eq | Ne, v1, Function(_, _, _)) if isValue(v1) =>
        throw DynamicTypeError(e)
      case Binary(Eq, v1, v2) if isValue(v1) && isValue(v2) => B(v1 == v2)
      case Binary(Minus, v1, v2) if isValue(v1) && isValue(v2) =>
        N(toNumber(v1) - toNumber(v2))
      case Binary(Ne, v1, v2) if isValue(v1) && isValue(v2) => B(v1 != v2)
      case Binary(Or, v1, e2) if isValue(v1) => if (toBoolean(v1)) v1 else e2
      case Binary(Plus, S(s1), v2) if isValue(v2) => S(s1 + toStr(v2))
      case Binary(Plus, v1, S(s2)) if isValue(v1) => S(toStr(v1) + s2)
      case Binary(Plus, v1, v2) if isValue(v1) && isValue(v2) =>
        N(toNumber(v1) + toNumber(v2))
      case Binary(Seq, v1, e2) if isValue(v1) => e2
      case Binary(Times, v1, v2) if isValue(v1) && isValue(v2) =>
        N(toNumber(v1) * toNumber(v2))
      case Binary(Lt, v1, v2) if isValue(v1) && isValue(v2) =>
        B(inequalityVal(Lt, v1, v2))
      case Binary(Le, v1, v2) if isValue(v1) && isValue(v2) =>
        B(inequalityVal(Le, v1, v2))
      case Binary(Gt, v1, v2) if isValue(v1) && isValue(v2) =>
        B(inequalityVal(Gt, v1, v2))
      case Binary(Ge, v1, v2) if isValue(v1) && isValue(v2) =>
        B(inequalityVal(Ge, v1, v2))
      case Call(v1, v2) if isValue(v1) && isValue(v2) => v1 match {
        case Function(None, x, e1) => substitute(e1, v2, x)
        case Function(Some(x1), x2, e1) =>
          substitute(substitute(e1, v2, x2), v1, x1)
        case _ => throw DynamicTypeError(e)
      }
      case ConstDecl(x, v1, e2) if isValue(v1) => substitute(e2, v1, x)
      case If(v1, e2, e3) if isValue(v1) => if (toBoolean(v1)) e2 else e3
      case Unary(Neg, v1) if isValue(v1) => N(-toNumber(v1))
      case Unary(Not, v1) if isValue(v1) => B(!toBoolean(v1))

      /* Inductive Cases: Search Rules */
      case Print(e1) => Print(step(e1))
      case Unary(uop, e1) => Unary(uop, step(e1))
      case Binary(bop, v1, e2) if isValue(v1) => Binary(bop, v1, step(e2))
      case Binary(bop, e1, e2) => Binary(bop, step(e1), e2)
      case If(e1, e2, e3) => If(step(e1), e2, e3)
      case ConstDecl(x, e1, e2) => ConstDecl(x, step(e1), e2)
      // The @ here passes what ever was matched in as a val named function
      // because we need to name the 'thing' that matched with 3 wildcards
      case Call(function @ Function(_, _, _), e2) => Call(function, step(e2))
      case Call(e1, e2) => Call(step(e1), e2)
      /* Cases that should never match. */
      case Var(_) => throw new AssertionError(
        "Gremlins: internal error, not closed expression."
      )
      case N(_) | B(_) | Undefined | S(_) | Function(_, _, _) =>
        throw new AssertionError(
          "Gremlins: internal error, step should not be called on values."
        );
    }
  }

  /* External Interfaces */

  //this.debug = true // uncomment this if you want to print debugging information

  // Interface to run your big-step interpreter starting from an empty environment and print out
  // the test input if debugging.
  def evaluate(e: Expr): Expr = {
    require(closed(e))
    if (debug) {
      println("------------------------------------------------------------")
      println("Evaluating with eval ...")
      println("\nExpression:\n  " + e)
    }
    val v = eval(emp, e)
    if (debug) {
      println("Value: " + v)
    }
    v
  }

  // Convenience to pass in a jsy expression as a string.
  def evaluate(s: String): Expr = evaluate(Parser.parse(s))

  // Interface to run your small-step interpreter and print out the steps of evaluation if debugging. 
  def iterateStep(e: Expr): Expr = {
    require(closed(e))
    def loop(e: Expr, n: Int): Expr = {
      if (debug) { println("Step %s: %s".format(n, e)) }
      if (isValue(e)) e else loop(step(e), n + 1)
    }
    if (debug) {
      println("------------------------------------------------------------")
      println("Evaluating with step ...")
    }
    val v = loop(e, 0)
    if (debug) {
      println("Value: " + v)
    }
    v
  }

  // Convenience to pass in a jsy expression as a string.
  def iterateStep(s: String): Expr = iterateStep(Parser.parse(s))

  def processFile(file: java.io.File) {
    var testCOG = true // Change this to true when submitting to COG and disregard the 3 local .jsy tests that fail.
    this.keepGoing = true

    if (debug) {
      println("============================================================")
      println("File: " + file.getName)
      println("Parsing ...")
    }

    val e1 =
      handle(None: Option[Expr]) {
        Some {
          Parser.parseFile(file)
        }
      } getOrElse {
        return
      }

    if (testCOG) {
      handle() {
        println("# Evaluating ...")
        val v1 = evaluate(e1)
        println(pretty(v1))
      }

      handle() {
        println("# Stepping ...")
        def loop(e: Expr, n: Int): Expr = {
          println("## %4d: %s".format(n, e))
          if (isValue(e)) e else loop(step(e), n + 1)
        }
        val v1 = loop(e1, 0)
        println(pretty(v1))
      }
    } else {
      handle() {
        val v1 = iterateStep(e1)
        println(pretty(v1))
      }
    }
  }

}
