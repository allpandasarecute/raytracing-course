#include "color.hpp"
#include <iostream>

int main() {
	Color c = Color(1, 1, 1);
	std::cout << sizeof(c) << std::endl;
	return 0;
}
