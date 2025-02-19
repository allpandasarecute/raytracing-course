#pragma once

#include "types.hpp"

class Camera {
  public:
	Camera();
	Camera(vec3 pos, vec3 right, vec3 up, vec3 forward, float fovx,
		   ind imageSize);
	Camera(const Camera &);
	~Camera() = default;

	vec3 pos, right, up, forward;
	float fovx, fovy;
};
