#include "color.hpp"
#include "vec.hpp"
#include <iostream>

int main() {
	auto v = Vec3(1, 2, 3);
	Color c = Color(1, 1, 1);
	std::cout << sizeof(c) << std::endl;
	return 0;
}
