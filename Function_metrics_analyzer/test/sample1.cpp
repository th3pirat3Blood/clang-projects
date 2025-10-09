/*
 * Just a sample file to check for function analyzer works properly
 */ 

void foo(int a);

int bar(int c) {
	int a = 5;
	int b = 3;
	return a + b + c;
}

int main() {
	int return_value = bar(1);
	return return_value;
}

