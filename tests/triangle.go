/* Author: Scott Cameron
 * This program asks for a 1 digit input and prints
 * 2 triangles to form a square, the left triangle consists of
 * your input symbol and the right consists of the number 0
 */

package main;



func main() {
	const inputLine = `Please enter a positive 1 digit number that isn't 0`;
	var (
		i int;
		symbol int;
		line [10] int;
	);


	symbol = -1;

	/* repeatedly ask for input until valid input is given */
	for symbol < 1 || symbol > 9 {
		println(inputLine);
		readln(&symbol);
	};

	i = 0;

	/* initialize array to all 0's */
	for i < 10 {
		line[i] = 0;
		i += 1;
	};

	i = 0;


	/* print the triangle */
	for i < 10 {
		line[i] = symbol;
		println(line[0], line[1], line[2], line[3], line[4], line[5], line[6], line[7], line[8], line[9]);
		i += 1;
	};



};
