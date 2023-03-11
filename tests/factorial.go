package main;

/*
 * Author: Nemi Rai
 * This program computes the factorial of a user inputted number.
 * It tests the '*' operator
 */
func main (){
	const (
		msg0 = `Enter the starting number for the factorial:`;
		msg1 = `Cannot compute the factorial of 0.`;
		msg2 = `Result of the factorial function:`;
		);
	var (
		input int;
		result int;
		);

	println (msg0);
	readln (&input);
	result = input;

	if (input == 0) {
		println (msg1);
	};

	for (input > 0) {
		input -= 1;
		result = result * input;
	};

	println (msg2);
	println (result);
};
