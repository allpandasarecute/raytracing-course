#pragma once

#include "types.hpp"

class Ray {
  public:
	Ray() = delete;
	Ray(vec3 pos, vec3 dir);

	vec3 pos, dir;
};
