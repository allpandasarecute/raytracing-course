#pragma once
#include "camera.hpp"
#include "light.hpp"
#include "object.hpp"
#include "types.hpp"

class Scene {
  public:
	Scene();
	Scene(const Scene &) = delete;
	Scene(string file);
	~Scene() = default;

	void generateImage();
	bool saveImage(string file);

	Ray generateRay(ind coord);
	optional<tuple<float, color, vec3>> intersect(Ray ray);
	color raytrace(Ray ray);

	vector<color> data;
	uint w, h;
	vector<obj> objs;
	Camera cam;
	color bg;
	uint raydepth;
	vec3 amb;
	vector<lght> lghts;
};
