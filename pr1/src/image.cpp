#include "image.hpp"

#include <exception>
#include <fstream>
#include <ios>
#include <iostream>

Image::Image() : data(vector<Color>()), w(0), h(0) {
}

Image::Image(uint w, uint h) : data(vector<Color>(w * h)), w(w), h(h) {
}

Color &Image::operator[](ind coord) {
	return data[coord.second * w + coord.first];
}

bool Image::save(string file) {
	std::ofstream output;
	try {
		output.open(file, std::ios::out);
		if (!output.is_open()) {
			return false;
		}
		output << "P6\n" << this->w << ' ' << this->h << '\n' << 255 << '\n';
		output.close();
		output.open(file, std::ios::out | std::ios::app | std::ios::binary);
		if (!output.is_open()) {
			return false;
		}
		output.write((const char *)this->data.data(),
					 this->data.size() * sizeof(Color));
	} catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
		output.close();
		return false;
	}
	output.close();
	return true;
}
