/* This file implements a linked list in Javascript using only lambdas.
 * Inspired by http://stevelosh.com/blog/2013/03/list-out-of-lambda/
 */

var nil = null;

function list_to_string(l) {
	var str = "";
	
	function innerprint(l) {
		if (typeof l == "function") {
			return String(head(l)) + ', ' + innerprint(tail(l));
		}
		else {
			return String(l);
		}
	}

	return '[' + innerprint(l) + ']'
}

function head(l) {
	return l(true);
}

function tail(l) {
	return l(false);
}

function is_empty(l) {
	return l == nil;
}

function prepend(i, l) {
	return function(x) { return x ? i : l; }
}

function at(l, i) {
	if (i > 0) {
		return at(tail(l), i - 1);
	}
	return head(l);
}

function length(l) {
	if (l == null) return 0;
	return 1 + length(tail(l));
}

function map(l, f) {
	if (is_empty(l)) return nil;
	return prepend(f(head(l)), map(tail(l), f));
}

function filter(l, f) {
	if (is_empty(l)) return nil;
	if (f(head(l))) {
		return prepend(head(l), filter(tail(l), f));
	} else {
		return filter(tail(l), f);
	}
}

/* Todo:
 * drop
 * take
 * slice
 */

test_list = prepend("x", prepend("y", prepend("z", nil)));

print(at(test_list, 1));
print(length(test_list));
print(list_to_string(filter(test_list, function(x) { return x == "x"; })));

