#include "scene.hpp"
#include "glm/geometric.hpp"
#include "quat.hpp"
#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <string>

Scene::Scene()
	: data(vector<Color>()), w(100), h(100), objs(vector<obj>()), cam(Camera()),
	  bg(Color()) {
}

Scene::Scene(const Scene &arg)
	: data(arg.data), w(arg.w), h(arg.h), objs(arg.objs), cam(arg.cam),
	  bg(arg.bg) {
}

Scene::Scene(string file) {
}


Color &Scene::operator[](ind coord) {
	return data[coord.second * w + coord.first];
}

bool Scene::saveImage(string file) {
	std::ofstream output;
	try {
		output.open(file, std::ios::out | std::ios::binary);
		if (!output.is_open()) {
			return false;
		}
		output << "P6\n"
			   << std::to_string(this->w) << ' ' + std::to_string(this->h)
			   << '\n'
			   << "255" << '\n';
		for (const auto &c : data) {
			output.put((std::uint8_t)glm::round(c.r * 255));
			output.put((std::uint8_t)glm::round(c.g * 255));
			output.put((std::uint8_t)glm::round(c.b * 255));
		}
	} catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
		output.close();
		return false;
	}
	output.close();
	return true;
}

Ray Scene::generateRay(ind coord) {
	float newX = (2.f * coord.first / this->w - 1) * this->cam.fovx + 0.5f;
	float newY = (2.f * coord.second / this->h - 1) * this->cam.fovy + 0.5f;
	return Ray(this->cam.pos,
			   glm::normalize(newX * this->cam.right + newY * this->cam.up +
							  this->cam.forward));
}

optional<pair<float, Color>> Scene::intersection(Ray ray) {
	optional<pair<float, Color>> ans = std::nullopt;
	intersect t;
	for (const obj &o : this->objs) {
		t = o->intersection(ray);
		if (t.has_value()) {
			if (!ans.has_value() || (ans.value().first > t.value())) {
				ans = optional(std::make_pair(t.value(), o->c()));
			}
		}
	}
	return ans;
}

Color Scene::raytrace(Ray ray) {
	optional<pair<float, Color>> i = this->intersection(ray);
	// std::cout << i.has_value() << std::endl;
	return i.has_value() ? i.value().second : this->bg;
}

void Scene::generateImage() {
	for (uint y = 0; y < this->h; ++y) {
		for (uint x = 0; x < this->w; ++x) {
			data[w * y + x] = this->raytrace(this->generateRay({x, y}));
		}
	}
}
