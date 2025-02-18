#pragma once
#include "types.hpp"

#pragma pack(push, 1)
class Color {
  public:
	Color() = default;
	Color(byte r, byte g, byte b);
	~Color() = default;

	byte r = 0, g = 0, b = 0;
};
#pragma pack(pop)
