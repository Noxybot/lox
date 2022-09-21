# Lox

Implementation of the Lox scripting language from a beautiful book [Crafting Interpreters](http://www.craftinginterpreters.com/) by [Bob Nystrom](https://github.com/munificent).

## Description
The WIP C++ implementation of Lox programming language. The main goals of this work is to utilize and practiсe modern C++20, especially such features as modules, ranges, smart pointers, etc.  

The generation of AST's tree was done by the `ast_builder` tool, which was also written in C++ to produce modern C++20 code.

For now, I think of this work as a skeleton for my own language which will be targeted at writing microservices.

## Supported features
* Statements
* Variables
* Print
* Control flow
* `for` and `while` loops
* Functions

## Code example
```python
fun fib(n) {
  if (n <= 1) return n;
  return fib(n - 2) + fib(n - 1);
}

  var a = clock();
  print fib(10);
  print "time: " + (clock() - a);
```