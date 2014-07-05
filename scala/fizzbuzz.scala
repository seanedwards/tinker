#!/bin/sh
exec scala $0 $@
!#

def fizzbuzz(i: Int) =
  (i % 3, i % 5) match {
    case (0, 0) => "FizzBuzz"
    case (0, _) => "Fizz"
    case (_, 0) => "Buzz"
    case (_, _) => i.toString
  }

for (i <- 1 to 100) {
  println(fizzbuzz(i))
}

