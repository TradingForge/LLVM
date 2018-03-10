#include <string>

using namespace std;

typedef int myint;
int an_int(myint); // It's a function

int myvar = 0;
int another_int(myvar); // It's a variable

const char * str = "hello";
char * ch_ptr;

int * ptr;
int ** ptrptr;

int (* fn_ptr[]) (int arg0, double arg2) = { NULL, NULL };
int (* (fun(char * str))) (int arg0, double arg2);

int ** arr[10], arr1[11];

int main()
{
    return 0;
}
