/*
  print_first_digit.go
  Author: Ryan Curtis

	This program prints the first digit of a given integer.
*/

package main;

const (
	inp_msg = `Choose a number: `;
	result_msg = `The first digit is`;
);

var (
	firstNum int;
);

func main() {
	println(inp_msg);
	readln(&firstNum);
	
  	if (firstNum < 0) {
		firstNum = firstNum * -1;
	};
	
	for {	
		if (firstNum > 10) {
			firstNum = firstNum / 10;
			continue;
		};

		break;
	};

	println(result_msg, firstNum);
};
