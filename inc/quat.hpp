#pragma once
#include "types.hpp"

class Quat {
  public:
	Quat();
	Quat(const Quat &);
	Quat(vec3 v, float w);
	~Quat() = default;

	Quat conj() const;

	vec3 v;
	float w;
};

Quat operator*(const Quat &a, const Quat &b);
vec3 rotation(vec3 v, Quat rot);
