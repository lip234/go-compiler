/* Author: Scott Cameron
 * This program asks the user for input and tells you if it
 * the input number is even or odd
 */

package main;

func main() {
	const (
		inputString = `Please enter a number to check: `;
		isEven = `The number is even`;
		isOdd = `The number is odd`;
		isHorriblyWrong = `Something went horribly wrong`;
	);

	var num int;
	num = 0;

	println(inputString);
	readln(&num);

	if num % 2 == 0 {
		println(isEven);
	} else {
		if num % 2 != 0 {
			println(isOdd);
		} else {
			println(isHorriblyWrong);
		};
	};
};
