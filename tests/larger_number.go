package main;

/* Author: Nemi Rai
 * This program tells you the larger of two numbers.
 * It tests the '<' and '>' relational operators.
 */
func main (){

	const (
		msg0 = `Enter a number`;
		msg1 = `First number is greater than the second number.`;
		msg2 = `Second number is greater than the first number.`;
		msg3 = `The numbers are equal`;
		);

	var ( 
		left int;
		right int;
		);

	println (msg0);
	readln (&left);
	println (msg0);
	readln (&right);

	if (left > right) {
		println(msg1);
	} else if (left < right) {
		println(msg2);
	} else {
		println(msg3);
	};
};
