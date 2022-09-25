# Lox

Implementation of the Lox scripting language from a beautiful book [Crafting Interpreters](http://www.craftinginterpreters.com/) by [Bob Nystrom](https://github.com/munificent).

## Description
The WIP C++ implementation of Lox programming language. The main goals of this work is to utilize and practiсe modern C++20, especially such features as modules, ranges, smart pointers, etc.  

The generation of AST's tree was done by the `ast_generator` tool, which was also written in C++ to produce modern C++20 code.

For now, I think of this work as a skeleton for my own language which will be targeted at writing microservices.

## Supported features
* Statements
* Variables
* Print
* Control flow
* `for` and `while` loops
* Functions
* Resolving and binding variables
* Classes, properties, methods, `this`, init-constructors
* Inheritance, access to `super` class methods

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

```python
var a = "outer";
{
  var a = "inner";
  print a;
}
#prints "outer outer"
```

```python
class Thing {
  getCallback() {
    fun localFunction() {
      print this;
    }

    return localFunction;
  }
}

var callback = Thing().getCallback();
callback();
#prints "Thing instance"
```

```python
class Doughnut {
  cook() {
    print "Fry until golden brown.";
  }
}

class BostonCream < Doughnut {
  cook() {
    super.cook();
    print "Pipe full of custard and coat with chocolate.";
  }
}

BostonCream().cook();
#prints 
Fry until golden brown.
Pipe full of custard and coat with chocolate.
```

## Optimizations

* Compare `typeid` in `CheckAnyType` instead of using `std::any_cast` and catching exceptions. It improves perfomance by 20 times in equality.lox benchmark.
* Cache strings in Environment, avoid unnecessary copies when compairing and accessing strings.
```bash
# Before optimization
>lox.exe equality.lox
loop
18.000000
elapsed
553.000000
equals
535.000000
# After
>lox.exe equality.lox
loop
7.000000
elapsed
14.000000
equals
7.000000
```

## TODO
* Add string pool (same as in Java)
