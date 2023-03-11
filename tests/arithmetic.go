/* Test statements for Arithmetic Operators
 * Written by Cooper Wallace (wallacec29@mymacewan.ca)
 */

package main;

func main() {
	/* Expression tests */

	/* Unary Operators */
								/* Expected Result from Go compiler*/
	println(-5);				/* -5 */
	println(+5);				/* 5 */
	/* Addition */
	println(256 + 256);			/* 512*/
	println(0x0 + 2);			/* 2 */
	println(0xFF + 2);			/* 257 */
	/* Subtraction */
	println(512 - 256);			/* 256 */
	println(0 - 256);			/* -256 */
	println(0xFF - 2);			/* 253 */

	/* Binary Shift */

	/* Multiplication */
	println(10 * 21);			/* 210 */
	/* Division */
	println(5 / 1);				/* 5 */
	/* Remainder */
	println(5 % 2);				/* 1 */

	/* Overflow Example */
	println(2_147_483_647 + 1); /* -2,147,483,648*/

	/* Testing Associativity*/

	println(1 + 2 * 3 + 4 + 5);			/* 16 */
	println(1 == 1 -2 || 1 == 2 - 1);	/* 1 */
	println(1 + 2 - 3 * 4 % 1);			/* 3 */
	println(5 / 2 * 3);					/* 6 */
};
