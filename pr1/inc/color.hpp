#pragma once

#pragma pack(push, 1)
class Color {
  public:
	Color();
	Color(const Color &);
	Color(float r, float g, float b);
	~Color() = default;

	float r, g, b;
};
#pragma pack(pop)
