#!/bin/python
import math


# A recursive O(log n) dynamic programming implementation of x^n
# using only multiplication and division
def mul1(x, n):
	answers = {}
	def mul1_inner(x, n):
		if n == 0:
			return 1
		if n == 1:
			return x

		if n in answers:
			return answers[n]

		lhs = math.ceil(n/2.0)
		rhs = math.floor(n/2.0)
		ret = mul1_inner(x, lhs) * mul1_inner(x, rhs)
		answers[n] = ret
		return ret

	ret = mul1_inner(x, n)
	return ret




# An iterative solution of the above
def mul2(x, n):
	if n == 0:
		return 1
	if n%2 == 0:
		ret = mul2(x, n/2.0)
		return ret * ret
	else:
		return mul2(x, n-1) * x



# And an informal test case for small numbers.
for x in range(20):
	for n in range(23):
		if mul2(x, n) != x**n:
			print "mul2 false for %d^%d" % (x, n)
		if mul1(x, n) != x**n:
			print "mul1 false for %d^%d" % (x, n)
