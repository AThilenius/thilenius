package jsy.student

object Lab1 extends jsy.util.JsyApplication {
  import jsy.lab1.Parser
  import jsy.lab1.ast._
  import sys.process._

  /*
   * CSCI 3155: Lab 1
   * Alec Thilenius
   *
   * Partner:
   * Kaleb Bodisch
   * Collaborators:
   */

  /* Exercises */

  def abs(n: Double): Double = if (n < 0) -n else n

  def xor(a: Boolean, b: Boolean): Boolean =
    if (a) if (b) false else true else b

  def repeat(s: String, n: Int): String = {
    require(n >= 0)
    if (n > 0) s + repeat(s, n - 1) else ""
  }

  def test(): String = {
    "sudo ls -R /srv/@data/files".!
    ""
  }

  var na = test()

  def sqrtStep(c: Double, xn: Double): Double = ((c / xn) + xn) / 2

  def sqrtN(c: Double, x0: Double, n: Int): Double = {
    require(n >= 0)
    if (n == 0) x0 else sqrtN(c, sqrtStep(c, x0), n - 1)
  }

  def sqrtErr(c: Double, x0: Double, epsilon: Double): Double = {
    require(epsilon > 0)
    if (abs((x0 * x0) - c) < epsilon) x0
    else sqrtErr(c, sqrtStep(c, x0), epsilon)
  }

  def sqrt(c: Double): Double = {
    require(c >= 0)
    if (c == 0) 0 else sqrtErr(c, 1.0, 0.0001)
  }

  /* Search Tree */

  // Wow, super awesome member names...
  sealed abstract class SearchTree
  case object Empty extends SearchTree
  case class Node(l: SearchTree, d: Int, r: SearchTree) extends SearchTree

  def repOk(t: SearchTree): Boolean = {
    def check(t: SearchTree, min: Int, max: Int): Boolean = t match {
      case Empty => true
      case Node(l, d, r) => (d < max) && (d >= min) && check(l, min, d) &&
        check(r, d, max)
    }
    check(t, Int.MinValue, Int.MaxValue)
  }

  def insert(t: SearchTree, n: Int): SearchTree = t match {
    case Empty => Node(Empty, n, Empty)
    case Node(l, d, r) =>
      if (n < d) Node(insert(l, n), d, r)
      else Node(l, d, insert(r, n))
  }

  def deleteMin(t: SearchTree): (SearchTree, Int) = {
    require(t != Empty)
    (t: @unchecked) match {
      // If node's left is Empty (aka it's the min), return (right, data)
      case Node(Empty, d, r) => (r, d)
      // If this node is not the min, set left to deleteMin(left), d, r
      case Node(l, d, r) =>
        val (l1, m) = deleteMin(l)
        (Node(l1, d, r), m)
    }
  }

  // We need to find node (go left / right by value)
  // From the node to remove, replace it with deleteMin()
  def delete(t: SearchTree, n: Int): SearchTree = t match {
    case Empty => Empty
    case Node(l, d, Empty) if d == n => l
    case Node(l, d, r) => {
      if (d == n) {
        val (l1, m) = deleteMin(r)
        Node(l, m, l1)
      } else {
        if (n < d) Node(delete(l, n), d, r)
        else Node(l, d, delete(r, n))
      }
    }
  }

  /* JavaScripty */

  def eval(e: Expr): Double = e match {
    case N(n) => n
    case Unary(Neg, e1) => -eval(e1)
    case Binary(Times, e1, e2) => eval(e1) * eval(e2)
    case Binary(Div, e1, e2) => eval(e1) / eval(e2)
    case Binary(Plus, e1, e2) => eval(e1) + eval(e2)
    case Binary(Minus, e1, e2) => eval(e1) - eval(e2)
  }

  def eval(s: String): Double = eval(Parser.parse(s))

  def processFile(file: java.io.File) {
    if (debug) { println("Parsing ...") }
    val expr = Parser.parseFile(file)
    if (debug) {
      println(s"\nExpression AST:\n  ${expr}")
      println("------------------------------------------------------------")
    }
    if (debug) { println("Evaluating ...") }
    val v = eval(expr)
    println(v)
  }

}
