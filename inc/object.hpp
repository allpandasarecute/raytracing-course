#pragma once
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

enum class NormDir {
	Out,
	In
};

class Object {
  public:
	Object();
	Object(const Object &) = default;
	~Object() = default;
	Object(Shape s, vec3 pos, vec3 dim, Quat rot, vec3 c, Material mat,
		   float ior);

	intersection intersect(Ray ray) const;
	vec3 norm(vec3 pos) const;

	Shape s;
	vec3 pos, dim; // for Plane dim == norm
	Quat rot;
	vec3 c;
	Material mat;
	float ior;
};

typedef std::unique_ptr<Object> obj;

intersection intersectBox(const Object &o, Ray ray);
intersection intersectPlane(const Object &o, Ray ray);
intersection intersectEllips(const Object &o, Ray ray);

bool comp(const intersection &a, const intersection &b);
