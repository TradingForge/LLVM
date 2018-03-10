#include <stdio.h>

//typedef int Foo;
//typedef int Bar, Baz;

//Foo foo(Bar(b), Baz(z));

int foobar = 9001;
int froboz(foobar);

int main(int argc, char ** argv) {
	// printf("foo(9000, 1)=%d\r\n", foo(9000, 1));
	printf("froboz=%d\r\n", froboz);
}

//Foo foo(Bar(b), Baz(z))
//{
//	return b + z;
//}
