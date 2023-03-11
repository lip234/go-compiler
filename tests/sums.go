/* Author: Laroy Milton
 * This program sums two numbers together
 */

package main;

func main() {
	const ( 
		msg1 = `Input first number: `;
		msg2 = `Input second number: `;
	);
	var ( a int; b int; );

	/* Get input */
	println(msg1);
	readln(&a);
	println(msg2);
	readln(&b);

	println(a+b);		
};
