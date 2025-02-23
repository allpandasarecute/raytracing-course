#include "scene.hpp"
#include "glm/geometric.hpp"
#include "object.hpp"
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

Scene::Scene()
	: data(vector<Color>()), w(100), h(100), objs(vector<obj>()), cam(Camera()),
	  bg(Color()) {
}

Scene::Scene(string file) {
	std::ifstream input;
	input.open(file);
	try {
		string command;
		float x, y, z, w;
		bool isLastObjPushed = true;
		obj prim;
		while (input >> command) {
			if (command == "DIMENSIONS") {
				input >> this->w >> this->h;
				this->data.resize(this->w * this->h);
			} else if (command == "BG_COLOR") {
				input >> x >> y >> z;
				this->bg = Color(x, y, z);
			} else if (command == "CAMERA_POSITION") {
				input >> x >> y >> z;
				this->cam.pos = {x, y, z};
			} else if (command == "CAMERA_RIGHT") {
				input >> x >> y >> z;
				this->cam.right = {x, y, z};
			} else if (command == "CAMERA_UP") {
				input >> x >> y >> z;
				this->cam.up = {x, y, z};
			} else if (command == "CAMERA_FORWARD") {
				input >> x >> y >> z;
				this->cam.forward = {x, y, z};
			} else if (command == "CAMERA_FOV_X") {
				// WARNING: assuming, command DIMENSIONS was before this one
				input >> x;
				this->cam.fovx = glm::tan(x / 2.f);
				this->cam.fovy = this->cam.fovx * this->h / this->w;
			} else if (command == "NEW_PRIMITIVE") {
				if (!isLastObjPushed) {
					this->objs.push_back(std::move(prim));
					isLastObjPushed = true;
				}
				prim = std::unique_ptr<Object>(new Object());
				isLastObjPushed = false;
			} else if (command == "ELLIPSOID") {
				prim->s = Shape::Ellips;
				input >> x >> y >> z;
				prim->dim = {x, y, z};
			} else if (command == "PLANE") {
				prim->s = Shape::Plane;
				input >> x >> y >> z;
				prim->dim = {x, y, z};
			} else if (command == "BOX") {
				prim->s = Shape::Box;
				input >> x >> y >> z;
				prim->dim = {x, y, z};
			} else if (command == "POSITION") {
				input >> x >> y >> z;
				prim->pos = {x, y, z};
			} else if (command == "COLOR") {
				input >> x >> y >> z;
				prim->color = Color(x, y, z);
			} else if (command == "ROTATION") {
				input >> x >> y >> z >> w;
				prim->rot = Quat({x, y, z}, w);
			}
		}
		if (!isLastObjPushed) {
			this->objs.push_back(std::move(prim));
			isLastObjPushed = true;
		}
	} catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
		input.close();
	}
	input.close();
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
		output.write((const char *)this->data.data(),
					 this->data.size() * sizeof(Color));
		output.close();
	} catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
		output.close();
		return false;
	}
	return true;
}

Ray Scene::generateRay(ind coord) {
	float newX = (2.f * (coord.first + 0.5f) / this->w - 1) * this->cam.fovx;
	float newY = -(2.f * (coord.second + 0.5f) / this->h - 1) * this->cam.fovy;
	return Ray(this->cam.pos,
			   glm::normalize(newX * this->cam.right + newY * this->cam.up +
							  this->cam.forward));
}

optional<pair<float, Color>> Scene::intersect(Ray ray) {
	optional<pair<float, Color>> ans = std::nullopt;
	intersection t;
	for (const obj &o : this->objs) {
		t = o->intersect(ray);
		if (t.has_value() && t.value() > 0) {
			if (!ans.has_value() || (ans.value().first > t.value())) {
				ans = optional(std::make_pair(t.value(), o->color));
			}
		}
	}
	return ans;
}

Color Scene::raytrace(Ray ray) {
	optional<pair<float, Color>> i = this->intersect(ray);
	return i.has_value() ? i.value().second : this->bg;
}

void Scene::generateImage() {
	for (uint y = 0; y < this->h; ++y) {
		for (uint x = 0; x < this->w; ++x) {
			data[w * y + x] = this->raytrace(this->generateRay({x, y}));
		}
	}
}
