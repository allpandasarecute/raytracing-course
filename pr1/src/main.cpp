#include "color.hpp"
#include "object.hpp"
#include "types.hpp"
#include <iostream>
#include <ostream>

int main() {
	Color c = Color(1, 1, 1);
	std::cout << sizeof(c) << std::endl;
	std::cout << (vec3(1.f) / vec3(2.f, 3.f, 4.f)).y << std::endl;
	std::cout << Box()
					 .intersection(Ray({0.f, -2.f, 0.f}, {0.f, 1.f, 0.f}))
					 .value_or(-25.f)
			  << std::endl;
	return 0;
}
