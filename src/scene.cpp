#include "scene.hpp"
#include "glm/glm.hpp"
#include "light.hpp"
#include "object.hpp"
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

#define colorToBytes(c) ((byte)glm::round(c * 255))
#define clampColor(c)                                                          \
	(color(glm::clamp((c).r, 0.f, 1.f), glm::clamp((c).g, 0.f, 1.f),           \
		   glm::clamp((c).b, 0.f, 1.f)))

ColorSave::ColorSave(color c)
	: r((byte)glm::round(glm::clamp(c.r, 0.f, 1.f) * 255)),
	  g((byte)glm::round(glm::clamp(c.g, 0.f, 1.f) * 255)),
	  b((byte)glm::round(glm::clamp(c.b, 0.f, 1.f) * 255)) {
}

Scene::Scene()
	: data(vector<ColorSave>()), w(100), h(100), objs(vector<obj>()),
	  cam(Camera()), bg(color()), raydepth(1), amb({0.f, 0.f, 0.f}) {
}

Scene::Scene(string file) {
	std::ifstream input;
	input.open(file);
	try {
		string command;
		float x, y, z, w;
		bool isLastObjPushed = true;
		bool isLastLightPushed = true;
		obj prim;
		lght light;
		while (input >> command) {
			if (command == "DIMENSIONS") {
				input >> this->w >> this->h;
				this->data.resize(this->w * this->h);
			} else if (command == "BG_COLOR") {
				input >> x >> y >> z;
				this->bg = color(x, y, z);
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
				prim->c = color(x, y, z);
			} else if (command == "ROTATION") {
				input >> x >> y >> z >> w;
				prim->rot = Quat({x, y, z}, w);
			} else if (command == "RAY_DEPTH") {
				input >> this->raydepth;
			} else if (command == "METALLIC") {
				prim->mat = Material::Metallic;
			} else if (command == "DIELECTRIC") {
				prim->mat = Material::Dielectric;
			} else if (command == "IOR") {
				input >> prim->ior;
			} else if (command == "AMBIENT_LIGHT") {
				input >> x >> y >> z;
				this->amb = {x, y, z};
			} else if (command == "NEW_LIGHT") {
				if (!isLastLightPushed) {
					this->lghts.push_back(std::move(light));
					isLastLightPushed = true;
				}
				light = std::unique_ptr<Light>(new Light());
				isLastLightPushed = false;
			} else if (command == "LIGHT_INTENSITY") {
				input >> x >> y >> z;
				light->c = color(x, y, z);
			} else if (command == "LIGHT_DIRECTION") {
				input >> x >> y >> z;
				light->type = LightType::Dir;
				light->dir = {x, y, z};
			} else if (command == "LIGHT_POSITION") {
				input >> x >> y >> z;
				light->type = LightType::Dot;
				light->pos = {x, y, z};
			} else if (command == "LIGHT_ATTENUATION") {
				input >> x >> y >> z;
				light->att = {x, y, z};
			}
		}
		if (!isLastObjPushed) {
			this->objs.push_back(std::move(prim));
			isLastObjPushed = true;
		}
		if (!isLastLightPushed) {
			this->lghts.push_back(std::move(light));
			isLastLightPushed = true;
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
					 this->data.size() * sizeof(ColorSave));
		output.close();
	} catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
		output.close();
		return false;
	}
	return true;
}

Ray Scene::generateRay(ind coord) {
	float newX = (2.f * (coord.x + 0.5f) / this->w - 1) * this->cam.fovx;
	float newY = -(2.f * (coord.y + 0.5f) / this->h - 1) * this->cam.fovy;
	return Ray(this->cam.pos,
			   glm::normalize(newX * this->cam.right + newY * this->cam.up +
							  this->cam.forward));
}

optional<tuple<float, color, vec3>> Scene::intersect(Ray ray) {
	optional<tuple<float, color, vec3>> ans = std::nullopt;
	intersection t;
	for (const obj &o : this->objs) {
		t = o->intersect(ray);
		if (t.has_value() && t->first > 0) {
			if (!ans.has_value() || (std::get<0>(ans.value()) > t->first)) {
				ans = optional(std::make_tuple(t->first, o->c, t->second));
			}
		}
	}
	return ans;
}

color Scene::raytrace(Ray ray) {
	auto i = this->intersect(ray);
	if (!i.has_value()) {
		return this->bg;
	}

	color objColor = std::get<1>(i.value());
	vec3 objNorm = std::get<2>(i.value());
	vec3 whereInter = ray.pos + ray.dir * std::get<0>(i.value());

	color ansColor = objColor * this->amb;

	for (auto &l : this->lghts) {
		if (l->type == LightType::Dir) {
			float d = dot(objNorm, l->dir);
			if (!this->intersect(Ray(whereInter + objNorm * 0.0001f, l->dir))
					 .has_value() &&
				d > 0) {
				ansColor += objColor * d * l->c;
			}
		}
		// } else if (l->type == LightType::Dot) {
		// 	float d = dot(objNorm, whereInter - l->pos);
		// 	if (d < 0) {
		// 		ansColor -= objColor * d *
		// 					l->intensity(glm::length(whereInter - l->pos)) *
		// 					l->c;
		// 	}
		// }
	}
	return ansColor;
	// return std::get<1>(i.value());
}

void Scene::generateImage() {
	for (uint y = 0; y < this->h; ++y) {
		for (uint x = 0; x < this->w; ++x) {
			data[w * y + x] =
				ColorSave(this->raytrace(this->generateRay({x, y})));
		}
	}
}
