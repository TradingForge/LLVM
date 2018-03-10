#include <iostream>
#include <string>

std::string ask_name();
void say_hello_to(std::string name);

int number_of_years(10);
double age(number_of_years);

int main(int argc, char ** argv)
{
    say_hello_to(ask_name());
    return 0;
}

std::string ask_name()
{
	std::string name;
	std::cin >> name;
	return name;
}

void say_hello_to(std::string name)
{
	std::cout << "Hello, " << name << "!";
}
