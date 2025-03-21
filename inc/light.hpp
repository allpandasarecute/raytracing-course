#pragma once
#include "types.hpp"
#include <memory>

enum LightType {
	None,
	Dot,
	Dir
};

class Light {
  public:
	Light();
	Light(const Light &) = default;
	Light(LightType type, vec3 pos, vec3 dir, vec3 c, vec3 att);
	~Light() = default;

	float intensity(float r);
	tuple<color, float, vec3> getLight(vec3 norm, vec3 pos);

	LightType type;
	vec3 pos, dir, c, att;
};

typedef std::unique_ptr<Light> lght;
