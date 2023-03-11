/* Asks for two numbers, x and y. Calculates x^y
 *
 * Written by Cooper Wallace (wallacec29@mymacewan.ca)
 */

package main;

const (
	notice = `Calculations for result over 2^(32-1) is unsupported`;
	prompt1 = `Calculates x^y`;
	prompt2 = `Enter value of x`;
	prompt3 = `Enter value of y`;
	unsupported = `Only Positive Powers are supported`;
	caret = `^`;
	equals = `=`;
);

var (
	x int;
	y int;
	n int;
	val int;
);

func main() {
	println(prompt1);
	println(notice);
	println(prompt2);
	readln(&x);
	println(prompt3);
	readln(&y);

	n = 0;
	val = x;

	/* Support for positive powers, loop wont run if negative or zero */
	for (n < (y-1)) {
		val = val * x;
		n += 1;
	};

	if (y >= 0) {
		println(x, caret, y, equals, val);
	} else {
		println(unsupported);
	};
};

