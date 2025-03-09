#pragma once
#include "color.hpp"
#include "quat.hpp"
#include "ray.hpp"
#include "types.hpp"
#include <memory>

enum class Shape {
	None,
	Plane,
	Ellips,
	Box
};

enum class Material {
	Diffuse,
	Metallic,
	Dielectric
};

class Object {
  public:
	Object();
	Object(const Object &) = default;
	~Object() = default;
	Object(Shape s, vec3 pos, vec3 dim, Quat rot, Color color, Material mat,
		   float ior);

	intersection intersect(Ray ray) const;

	Shape s;
	vec3 pos, dim; // for Plane dim == norm
	Quat rot;
	Color color;
	Material mat;
	float ior;
};

typedef std::unique_ptr<Object> obj;

intersection intersectBox(const Object &o, Ray ray);
intersection intersectPlane(const Object &o, Ray ray);
intersection intersectEllips(const Object &o, Ray ray);
