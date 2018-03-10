#include <stdio.h>

int b = 9000;
int z = 1;

//typedef int b, z;

class Foo {
	int _arg0, _arg1;
	
public:
	Foo(int arg0, int arg1) {
		_arg0 = arg0;
		_arg1 = arg1;
	}
	
	int value() { return _arg0 + _arg1; }
};

Foo foo(b, z);// = {100, 500};

int main(int argc, char ** argv) {
	printf("foo.value()=%d\r\n", foo.value());
}
