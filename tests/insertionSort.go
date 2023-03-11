/* Insertion Sort Test program
 * Asks for numelem numbers, and then prints them.
 *
 * The number of elements read is adjustable using the numelem constant
 *
 * Insertion Sort sourced from:
 * https://www.golangprograms.com/golang-program-for-implementation-of-insertionsort.html
 *
 * Written by Cooper Wallace (wallacec29@mymacewan.ca)
 */

package main;

const (
	prompt = `Please enter`;
	prompt2 = `unsorted numbers`;
	endprompt = `Sorted List`;
	numelem = 8;
);

var (
	tmp int;
	a [numelem]int;
	i int;
);

func main() {
	/* Adjust number of elements */
	println(prompt, numelem, prompt2);
	i = 0;
	for (i < numelem) {
		readln(&a[i]);
		i += 1;
	};

	i = 1;
	for (i < numelem) {
		var j int;
		j = i;
		for (j > 0) {
			if (a[j-1] > a[j]) {
				tmp = a[j-1];
				a[j-1] = a[j];
				a[j] = tmp;
			};
			j -= 1;
		};
		i += 1;
	};

	println(endprompt);
	i = 0;
	for (i < numelem) {
		println(a[i]);
		i += 1;
	};
};
