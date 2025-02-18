#pragma once
#include "types.hpp"

class Scene {
  public:
	Scene() = delete;
	Scene(const Scene &) = default;
	Scene(string file);
	~Scene() = default;
};
