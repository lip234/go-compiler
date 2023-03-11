/*
  array_compare.go
  Author: Ryan Curtis
	
	This program prints the smallest or largest number in an array
*/

package main;

const (
	inp_msg = `Choose small (1) or large (2): `;
	small_msg = `The smallest number in array is`;
	large_msg = `The largest number in array is`;
	invalid_msg = `Invalid choice!`;
);

var (
	array[5] int;
	min int;
	max int;
	choice int;
);

func main() {
	var i int;

	array[0] = 1;
	array[1] = 15;
	array[2] = 200;
	array[3] = 0x05;
	array[4] = -35;
	
	println(inp_msg);
	readln(&choice);

	switch choice {
		case 1:
			min = array[0];
			i = 1;
			for (i < 5) {
				if (array[i] < min) {
					min = array[i];
				};
				i += 1;
			};
			println(small_msg, min);
			break; 
		case 2:
			max = array[0];
			i = 1;
			for (i < 5) {
				if (array[i] > max) {
					max = array[i];
				};
				i += 1;
			};
			println(large_msg, max);
			break;
		default:
			println(invalid_msg);
			break;
	};
};
