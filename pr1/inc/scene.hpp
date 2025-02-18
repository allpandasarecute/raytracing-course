#pragma once
#include "camera.hpp"
#include "color.hpp"
#include "object.hpp"
#include "types.hpp"

class Scene {
  public:
	Scene();
	Scene(const Scene &);
	Scene(string file);
	~Scene() = default;

	vector<Object> objs;
	Camera cam;
	Color bg;
};
