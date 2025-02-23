#pragma once

#include "types.hpp"

class Ray {
  public:
	Ray();
	Ray(const Ray &);
	Ray(vec3 pos, vec3 dir);
	~Ray() = default;

	vec3 pos, dir;
};
