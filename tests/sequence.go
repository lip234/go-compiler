/* Author: Laroy Milton
 * This program prints out a sequence of numbers given start, step, stop
 */

package main;

func main() {
	const (
			msg1 = `Please enter start: `;
			msg2 = `Please enter step: `;
			msg3 = `Please enter stop: `;
	);
	var (
			start int;
			step int;
			stop int;
	);
	
	/* Get Input */
	println(msg1);
	readln(&start);
	println(msg2);
	readln(&step);
	println(msg3);
	readln(&stop);

	/* Print sequence */
	for (start <= stop) {
		println(start);
		start += step;
	};
};	
