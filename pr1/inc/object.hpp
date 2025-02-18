#pragma once
#include "color.hpp"
#include "quat.hpp"
#include "ray.hpp"
#include "types.hpp"

class Object {
  public:
	Object() = default;
	Object(const Object &) = default;
	virtual ~Object() = default;

	virtual intersect intersection(Ray ray) const = 0;
};

class Ellips : public Object {
  public:
	Ellips();
	Ellips(const Ellips &);
	Ellips(vec3 pos, vec3 r, Color color);
	~Ellips() override = default;

	intersect intersection(Ray ray) const override;

	vec3 pos, r;
	Color color;
};

class Box : public Object {
  public:
	Box();
	Box(const Box &);
	Box(vec3 pos, vec3 dim, Quat rot, Color color);
	~Box() override = default;

	intersect intersection(Ray ray) const override;

	vec3 pos, dim;
	Quat rot;
	Color color;
};

class Plane : public Object {
  public:
	Plane();
	Plane(const Plane &);
	Plane(vec3 pos, vec3 norm, Color color);
	~Plane() override = default;

	intersect intersection(Ray ray) const override;

	vec3 pos, norm;
	Color color;
};
