#pragma once
#include "camera.hpp"
#include "light.hpp"
#include "object.hpp"
#include "types.hpp"

#pragma pack(push, 1)
class ColorSave {
  public:
	ColorSave() = default;
	ColorSave(color c);

	byte r, g, b;
};
#pragma pack(pop)

class Scene {
  public:
	Scene();
	Scene(const Scene &) = delete;
	Scene(string file);
	~Scene() = default;

	void generateImage();
	bool saveImage(string file);

	Ray generateRay(uvec2 coord);
	optional<tuple<float, vec3, uint, bool>> intersect(Ray ray, float r);
	color raytrace(Ray ray, uint depth);

	vector<ColorSave> data;
	uint w, h;
	vector<obj> objs;
	Camera cam;
	color bg;
	uint raydepth;
	vec3 amb;
	vector<lght> lghts;
};
