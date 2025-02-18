#pragma once
#include "types.hpp"

#pragma pack(push, 1)
class Color {
  public:
	Color(byte r, byte g, byte b);
	~Color() = default;

	byte r, g, b;
};
#pragma pack(pop)
