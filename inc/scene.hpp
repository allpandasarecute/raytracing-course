#pragma once
#include "camera.hpp"
#include "color.hpp"
#include "object.hpp"
#include "types.hpp"

class Scene {
  public:
	Scene();
	Scene(const Scene &) = delete;
	Scene(string file);
	~Scene() = default;

	Color &operator[](ind coord);

	void generateImage();
	bool saveImage(string file);

	Ray generateRay(ind coord);
	optional<pair<float, Color>> intersect(Ray ray);
	Color raytrace(Ray ray);

	vector<Color> data;
	uint w, h;
	vector<obj> objs;
	Camera cam;
	Color bg;
};
