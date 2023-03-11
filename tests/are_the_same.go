package main;

/* Author: Nemi Rai
 * This program prints whether two numbers are the same or not.
 * It tests the '==' and '!=' relational operators.
 */
func main (){
	const (
		msg0 = `Enter a number:`;
		msg1 = `The numbers are the same`;
		msg2 = `The numbers are different`;
		);

	var (
		left int;
		right int;
		);

	println (msg0);
	readln (&left);
	println (msg0);
	readln (&right);

	if (left == right) {
		println(msg1);
	} else if (left != right) {
		println(msg2);
	};
};
