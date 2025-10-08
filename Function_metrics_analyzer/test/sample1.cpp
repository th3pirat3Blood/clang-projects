/*
 * Just a sample file to check for function analyzer works properly
 */ 

void foo();

int bar() {
	int a = 5;
	int b = 3;
	return a+b;
}

int main() {

	int return_value = bar();
	return return_value;
}

