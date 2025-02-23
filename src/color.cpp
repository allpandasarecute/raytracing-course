#include "color.hpp"

Color::Color() : r(0.f), g(0.f), b(0.f) {
}

Color::Color(float r, float g, float b)
	: r((byte)glm::round(r * 255)), g((byte)glm::round(g * 255)),
	  b((byte)glm::round(b * 255)) {
}

Color::Color(const Color &arg) : r(arg.r), g(arg.g), b(arg.b) {
}
