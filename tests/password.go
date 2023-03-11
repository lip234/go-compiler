/* Author: Laroy Milton
 * This program checks if the given input is the correct password
 */

package main;

func main() {
	const ( 
		msg1 = `Enter password: `;
		msgAccept = `Access granted `;
		msgDeny = `Access denied `;
	);
	var guess int;

	/* Get input */
	println(msg1);
	readln(&guess);
	
	/* Check Password */
	if (guess == 1234) {
		println(msgAccept);
	} else {
		println(msgDeny);
	};
};
