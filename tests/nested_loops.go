/* Author: Scott Cameron
   This program runs through 3 nested loops
   for a user input number of times.
   It counts how many times the inner loop was ran
   and prints the result
 */

package main;

func main() {
	const (
		inputLine = `Enter 3 numbers to tell how high each loop should go`;
		iLine = `How high should the outer loop go?`;
		jLine = `How high should the middle loop go?`;
		kLine = `How high should the inner loop go?`;
		outputLine = `The number of times the inner loop looped: `;
	);

	var (
		i int;
		j int;
		k int;
		iStop int;
		jStop int;
		kStop int;
		result int;
	);

	println(inputLine);

	println(iLine);
	readln(&iStop);

	println(jLine);
	readln(&jStop);

	println(kLine);
	readln(&kStop);

	result = 0;
	i = 0;
	
	for i < iStop {
		j = 0;
		for j < jStop {
			k = 0;
			for k < kStop {
				/* increment the counter for each time the inner loop eached */
				result += 1;
				/* increment k by 1 */
				k = k * 1 + ((8 / 2 - 2) + -1);
			};
			j -= -1;
		};
		i += 1;
	};

	println(outputLine, result);
};
