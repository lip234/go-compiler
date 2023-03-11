/* Author: Scott Cameron
 * This program simply starts with the number 1
 * and prints it then doubles the number through shifting
 */
package main;

func main() {
	var num int;

	num = 1;

	for num < 1000000000 {
		println(num);
		num = num << 1;
	};
};
