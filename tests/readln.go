// This file, readln.go, implements the MacEwan Go "readln" statement.
// This implementation exists so that you can test your MacEwan Go
// programs with the standard Go compiler.
//
// For example:
//   go run readln.go <MacEwan Go program>.go
package main

import "fmt"

func readln(pointers ...*int) {
	for _, pointer := range pointers {
		fmt.Scanf("%d", pointer)
	}
}
