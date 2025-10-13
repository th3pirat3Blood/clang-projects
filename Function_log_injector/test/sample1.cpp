/*
 * Sample file to run loginjector on
 */ 

int foo() {
	int x = 4;
	return x++;
}

int bar() {
	int y = 5;
	return foo() + y;
}

int main() {
	int z = 3;
	return z + bar();
}
