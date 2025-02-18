#include "color.hpp"

Color::Color() : r(0), g(0), b(0) {
}

Color::Color(byte r, byte g, byte b) : r(r), g(g), b(b) {
}

Color::Color(const Color &arg) : r(arg.r), g(arg.g), b(arg.b) {
}
