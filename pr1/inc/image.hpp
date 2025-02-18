#pragma once
#include "color.hpp"
#include "types.hpp"

class Image {
  public:
	Image(uint w, uint h);
	~Image() = default;

	Color &operator[](ind coord);

	bool save(string file);

  private:
	vector<Color> data;
	uint w, h;
};
