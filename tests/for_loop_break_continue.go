/*
 * CMPT 399: Compilers
 * Author: Peng Li
 * This file is to test whether break and continue statements in a for loop
 * for MacEwan Go compiler.
 * If both implemented properly, the result should be 165(sum of all numbers between 10 and 20 inclusive).
 */

package main;

func main() {
    var (
        i int;
        sum int;
    );
    i = 0;
    sum = 0;

   for i < 100 {
        i += 1;
        if (i < 10) {
            continue;
        };
        sum += i;

        if (i >= 20) {
            break;
        };
   };
   println(sum);
};
