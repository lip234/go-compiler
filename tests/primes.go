/* Author: Scott Cameron
 * this program print the first n primes
 * where n is a user input value
 */

package main;


func main() {

	const (
		inputLine = `Please enter how many primes you want to print:`;
		primeLine = `is a prime number`;
	);

	var (
		currentNum int;
		remainderCheck int;
		primeCount int;
		primeCap int;
		factors int;
	);

	println(inputLine);
	readln(&primeCap);

	currentNum = 2;
	primeCount = 0;

	for primeCount < primeCap {
		factors = 0;
		remainderCheck = 2;
		for remainderCheck < currentNum {
			if currentNum % remainderCheck == 0 {
				factors += 1;
				break;
			};
			remainderCheck += 1;
		};

		if (factors == 0) {
			println(currentNum, primeLine);
			primeCount += 1;
		};
		currentNum += 1;
	};
};
