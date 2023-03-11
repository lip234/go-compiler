/*
 * CMPT 399: Compilers
 * Author: Peng Li
 * This file is to test whether MacEwan Go compiler handles operator precedence properly.
 * ref: https://golang.org/ref/spec#Operators
 * Operators with same precedence should be processed in left-to-right order.
 */
package main;

func main() {
    const (
    a = `Expected result: 5 actual result `;
    b = `Logical operator test failed`;
    c = `Logical operator test successful`;
    );
    var (i int;);
    /* test arithmetic operator precedence */
    i = 1 + 2 * 3 << 4 / 5 >> ( -1 * -2);
    println(a, i);
    /* Test logical operator precedence */
    if 1 > 2 && 2 > 1 ||  3 > 4 && 4 > 3 || 5 != 2+3  {
        println(b);
    } else {
        println(c);
    };
};
