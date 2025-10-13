/*
 * Sample file to run loginjector on
 */ 

#include <stdio.h>

int foo(int a, int b) {
	int x = 4 + b;
	return x++ + a;
}

int bar() {
	int y = 5;
	return foo(1, 1) + y;
}

void foobar(int a, int b) {
	for (int i = a; i <= ((a<b)? b:a); i++)
		 i = i*2;
}

