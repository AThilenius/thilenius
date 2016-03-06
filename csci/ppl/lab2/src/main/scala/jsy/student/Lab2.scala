package jsy.student

object Lab2 extends jsy.util.JsyApplication {
  import jsy.lab2.Parser
  import jsy.lab2.ast._
  import scala.util.Try

  /*
   * CSCI 3155: Lab 2
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

  /* Some useful Scala methods for working with Scala values include:
   * - Double.NaN
   * - s.toDouble (for s: String)
   * - n.isNaN (for n: Double)
   * - n.isWhole (for n: Double)
   * - s (for n: Double)
   * - s format n (for s: String [a format string like for printf], n: Double)
   */

  def toNumber(v: Expr): Double = {
    require(isValue(v))
    (v: @unchecked) match {
      case N(n) => n
      case B(true) => 1.0
      case B(false) => 0.0
      case S(s) => try s.toDouble catch { case _: Throwable => Double.NaN }
      case null => 0.0
      case Undefined => Double.NaN
      case _ => throw new UnsupportedOperationException
    }
  }

  def toBoolean(v: Expr): Boolean = {
    require(isValue(v))
    (v: @unchecked) match {
      case B(b) => b
      case N(n) => n != 0.0 && n != Double.NaN
      case S(s) => s != ""
      case null => false
      case Undefined => false
      case _ => false
    }
  }

  def toStr(v: Expr): String = {
    require(isValue(v))
    (v: @unchecked) match {
      case S(s) => s
      case Undefined => "undefined"
      case B(b) => b.toString
      case N(n) => n.toString
      case null => "null"
      case _ => throw new UnsupportedOperationException
    }
  }

  def eval(env: Env, e: Expr): Expr = {
    /* Some helper functions for convenience. */
    def eToVal(e: Expr): Expr = eval(env, e)

    e match {
      // Base Case
      case _ if (isValue(e)) => e

      case ConstDecl(x, e1, e2) => eval(extend(env, x, eToVal(e1)), e2)
      case Undefined => Undefined
      case Print(e1) => {
        println(pretty(eToVal(e1)));
        Undefined;
      }
      case Binary(And, e1, e2) => {
        if (toBoolean(e1) && !toBoolean(e2)) {
          return eToVal(e2)
        } else {
          return eToVal(e1)
        }
      }
      case Binary(Or, e1, e2) => {
        if (!toBoolean(e1) && toBoolean(e2)) {
          return eToVal(e2)
        } else {
          return eToVal(e1)
        }
      }
      case Binary(Plus, e1, e2) => N(toNumber(eToVal(e1)) + toNumber(eToVal(e2)))
      case Binary(Minus, e1, e2) => N(toNumber(eToVal(e1)) - toNumber(eToVal(e2)))
      case Binary(Times, e1, e2) => N(toNumber(eToVal(e1)) * toNumber(eToVal(e2)))
      case Binary(Div, e1, e2) => N(toNumber(eToVal(e1)) / toNumber(eToVal(e2)))
      case Binary(Lt, e1, e2) => B(toNumber(eToVal(e1)) < toNumber(eToVal(e2)))
      case Binary(Le, e1, e2) => B(toNumber(eToVal(e1)) <= toNumber(eToVal(e2)))
      case Binary(Gt, e1, e2) => B(toNumber(eToVal(e1)) > toNumber(eToVal(e2)))
      case Binary(Ge, e1, e2) => B(toNumber(eToVal(e1)) >= toNumber(eToVal(e2)))
      case Binary(Eq, e1, e2) => B(eToVal(e1) == eToVal(e2))
      case Binary(Ne, e1, e2) => B(eToVal(e1) != eToVal(e2))
      case Unary(Not, e1) => B(!toBoolean(eToVal(e1)))
      case Unary(Neg, e1) => N(-toNumber(eToVal(e1)))
      case If(e1, e2, e3) => if (toBoolean(eToVal(e1))) eToVal(e2) else eToVal(e3)
      case Binary(Seq, e1, e2) => {
        eToVal(e1)
        eToVal(e2)
      }
      case _ => throw new UnsupportedOperationException
    }
  }

  // Interface to run your interpreter starting from an empty environment.
  def eval(e: Expr): Expr = eval(emp, e)

  // Interface to run your interpreter from a string.  This is convenient
  // for unit testing.
  def eval(s: String): Expr = eval(Parser.parse(s))

  /* Interface to run your interpreter from the command-line.  You can ignore what's below. */
  def processFile(file: java.io.File) {
    if (debug) { println("Parsing ...") }

    val expr = Parser.parseFile(file)

    if (debug) {
      println("\nExpression AST:\n  " + expr)
      println("------------------------------------------------------------")
    }

    if (debug) { println("Evaluating ...") }

    val v = eval(expr)

    println(pretty(v))
  }

}
