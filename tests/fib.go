/* Fibonacci Implementation
 * Written by Cooper Wallace (wallacec29@mymacewan.ca)
 */

package main;

const (
	prompt = `Terms to output:`;
	seperator = `Printing terms to `;
	negative = `Can only print terms for integers > 0`;
);

func main() {
	var (
		fib [3]int;
		count int;
		term int;
	);

	count = 0;
	fib[0] = 0;
	fib[1] = 1;

	println(prompt);
	readln(&term);

	if (term <= 0) {
		println(negative);
	} else {
		println(seperator, term);
		for (count <= term) {
			println(fib[0]);
			fib[2] = fib[1];
			fib[1] = fib[0] + fib[1];
			fib[0] = fib[2];
			count += 1;
		};
	};
};
