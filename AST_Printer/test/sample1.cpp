/*
 * Just a sample file
 */ 

int foo(int a) {
	return a + a+1;
}

int bar(int b) {
	return b + foo(b+1);
}

int main() {

	return bar(1);
}
