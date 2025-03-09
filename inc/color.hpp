#pragma once
#include "types.hpp"

#pragma pack(push, 1)
class Color {
  public:
	Color();
	Color(const Color &) = default;
	Color(float r, float g, float b);
	~Color() = default;

	byte r, g, b;
};
#pragma pack(pop)
