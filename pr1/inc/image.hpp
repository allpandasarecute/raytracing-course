#pragma once
#include "types.hpp"
#include <string>

class Image {
  public:
	Image(uint w, uint h);
	~Image() = default;

	bool save(std::string file);
};
