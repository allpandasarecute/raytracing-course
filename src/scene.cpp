#include "scene.hpp"
#include "glm/glm.hpp"
#include "light.hpp"
#include "object.hpp"
#include "types.hpp"
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

#define colorToBytes(c)                                                        \
	((byte)glm::round(glm::pow(glm::clamp(c, 0.f, 1.f), 1.f / 2.2) * 255.f))

color tonemap(color col) {
	const float a = 2.51f;
	const float b = 0.03f;
	const float c = 2.43f;
	const float d = 0.59f;
	const float e = 0.14f;
	return (col * (a * col + b)) / (col * (c * col + d) + e);
}

ColorSave::ColorSave(color c) {
	c = tonemap(c);
	this->r = colorToBytes(c.r);
	this->g = colorToBytes(c.g);
	this->b = colorToBytes(c.b);
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

Ray Scene::generateRay(uvec2 coord) {
	float newX = (2.f * (coord.x + 0.5f) / this->w - 1) * this->cam.fovx;
	float newY = -(2.f * (coord.y + 0.5f) / this->h - 1) * this->cam.fovy;
	return Ray(this->cam.pos, newX * this->cam.right + newY * this->cam.up +
								  this->cam.forward);
}

optional<tuple<float, vec3, uint, bool>> Scene::intersect(Ray ray, float r) {
	optional<tuple<float, vec3, uint, bool>> ans = std::nullopt;
	optional<tuple<float, vec3, bool>> temp = std::nullopt;
	tuple<float, vec3, bool> t;
	optional<color> ansColor = std::nullopt;
	for (int i = 0; i < this->objs.size(); ++i) {
		temp = this->objs[i]->intersect(ray);
		if (!temp.has_value()) {
			continue;
		}
		t = temp.value();
		if (std::get<0>(t) > 0 && std::get<0>(t) < r) {
			if (!ans.has_value() ||
				(std::get<0>(ans.value()) > std::get<0>(t))) {
				ans = std::make_tuple(std::get<0>(t), std::get<1>(t), i,
									  std::get<2>(t));
			}
		}
	}
	return ans;
}

color Scene::raytrace(Ray ray, uint depth) {
	if (depth >= this->raydepth) {
		return color(0.f);
	}

	auto i = this->intersect(ray, FLOAT_MAX);
	if (!i.has_value()) {
		return this->bg;
	}

	// objNorm is always outside of an object
	auto [t, objNorm, ind, isInside] = i.value();
	color objColor = this->objs[ind]->c;
	vec3 whereInter = ray.pos + t * ray.dir;

	if (this->objs[ind]->mat == Material::Diffuse) {
		color ansColor = this->amb;
		for (auto &l : this->lghts) {
			auto [c, r, lDir] = l->getLight(objNorm, whereInter);
			if (!this->intersect(Ray(whereInter + objNorm * DELTA, lDir), r)) {
				ansColor += c;
			}
		}
		return ansColor * objColor;
	} else if (this->objs[ind]->mat == Material::Metallic) {
		return objColor *
			   this->raytrace(
				   Ray(whereInter + objNorm * DELTA,
					   ray.dir - 2 * dot(ray.dir, objNorm) * objNorm),
				   depth + 1);
	} else if (this->objs[ind]->mat == Material::Dielectric) {
		float d = dot(objNorm, ray.dir);
		vec3 reflectedDir = ray.dir - 2.f * d * objNorm;
		Ray reflectedRay = Ray(whereInter + DELTA * objNorm, reflectedDir);
		color reflectedColor = this->raytrace(reflectedRay, depth + 1);

		float p1 = (isInside ? this->objs[ind]->ior : 1.f);
		float p2 = (!isInside ? this->objs[ind]->ior : 1.f);

		float sin2 = p1 / p2 * glm::sqrt(1 - d * d);

		if (glm::abs(sin2) > 1.f) {
			return reflectedColor;
		}

		float cos2 = glm::sqrt(1 - sin2 * sin2);

		vec3 refractedDir = glm::normalize((p1 / p2) * ray.dir -
										   (p1 / p2 * d + cos2) * objNorm);
		Ray refractedRay = Ray(whereInter + DELTA * refractedDir, refractedDir);
		color refractedColor = this->raytrace(refractedRay, depth + 1);

		if (!isInside) {
			refractedColor *= objColor;
		}

		float r0 = (p1 - p2) * (p1 - p2) / ((p1 + p2) * (p1 + p2));
		float r = r0 + (1.f - r0) * glm::pow(1.f + d, 5.f);
		return r * reflectedColor + (1.f - r) * refractedColor;
	}
	return color(0.f);
}

void Scene::generateImage() {
	for (uint y = 0; y < this->h; ++y) {
		for (uint x = 0; x < this->w; ++x) {
			data[w * y + x] =
				ColorSave(this->raytrace(this->generateRay({x, y}), 0));
		}
	}
}
