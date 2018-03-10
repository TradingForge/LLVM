#include <stdio.h>

class Bar;
class Froboz;

class Bar {
private:
	int _value;
public:
	Bar(int value) {
		_value = value;
	}
	
	int value() { return _value; }
};

class Froboz {
private:
	int _value;
public:
	Froboz(int value) {
		_value = value;
	}
	
	int value() { return _value; }
};

class Foo {
private:
    Bar & _b;
	Froboz & _z;
	
public:
	Foo(Bar & b, Froboz & z)
    : _b(b)
    , _z(z)
    {
	}
	
	int value() { return _b.value() + _z.value(); }
};

int b = 9000;
int z = 1;

Foo foo(Bar(b), Froboz(z));

int main(int argc, char ** argv) {
	//Foo foo(Bar(b), Froboz(z));
	printf("foo.value()=%d\r\n", foo.value());
}
