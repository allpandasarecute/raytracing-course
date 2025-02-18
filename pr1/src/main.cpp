#include "color.hpp"
#include "types.hpp"
#include <iostream>
#include <ostream>

int main() {
	Color c = Color(1, 1, 1);
	std::cout << sizeof(c) << std::endl;
	std::cout << (vec3(1.f) / vec3(2.f, 3.f, 4.f)).y << std::endl;
	return 0;
}
