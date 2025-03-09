#include "color.hpp"

Color::Color() : r(0), g(0), b(0) {
}

Color::Color(float r, float g, float b)
	: r((byte)glm::round(r * 255)), g((byte)glm::round(g * 255)),
	  b((byte)glm::round(b * 255)) {
}
