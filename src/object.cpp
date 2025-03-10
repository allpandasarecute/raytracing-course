#include "object.hpp"
#include "quat.hpp"
#include "types.hpp"
#include <optional>


bool operator<(const intersection &a, const intersection &b) {
	return ((a.has_value() && b.has_value() && a->first < b->first) ||
			(!a.has_value() && b.has_value()));
}

intersection maxInter(const intersection &a, const intersection &b) {
	return a < b ? b : a;
}

intersection minInter(const intersection &a, const intersection &b) {
	return a < b ? a : b;
}

#define swapIfMin(x, y)                                                        \
	{                                                                          \
		if (y < x)                                                             \
			std::swap(x, y);                                                   \
	}

Object::Object()
	: s(Shape::None), pos(vec3(0.f)), dim(vec3(1.f)), rot({vec3(0.f), 1.f}),
	  c(color()), mat(Material::Diffuse), ior(0.f) {
}

Object::Object(Shape s, vec3 pos, vec3 dim, Quat rot, color c, Material mat,
			   float ior)
	: s(s), pos(pos), dim(dim), rot(rot), c(c), mat(mat), ior(ior) {
}

intersection Object::intersect(Ray ray) const {
	switch (this->s) {
	case Shape::Plane:
		return intersectPlane(*this, ray);
		break;
	case Shape::Ellips:
		return intersectEllips(*this, ray);
		break;
	case Shape::Box:
		return intersectBox(*this, ray);
		break;
	};
	return std::nullopt;
}

intersection intersectEllips(const Object &o, Ray ray) {
	vec3 newPos = rotation(ray.pos - o.pos, o.rot.conj()) / o.dim;
	vec3 newDir = rotation(ray.dir, o.rot.conj()) / o.dim;
	float a = dot(newDir, newDir);
	float b = 2 * dot(newPos, newDir);
	float c = dot(newPos, newPos) - 1;
	float d = b * b - 4 * a * c;
	if (d < 0) {
		return std::nullopt;
	}
	d = glm::sqrt(d);
	float t = (-b - d) / (2 * a);
	vec3 whereInter = newPos + t * newDir;
	if (t > 0) {
		return optional(
			std::make_pair(t, rotation(glm::normalize(whereInter), o.rot)));
	}
	t += d / a;
	whereInter = newPos + t * newDir;
	if (t > 0) {
		return optional(
			std::make_pair(t, rotation(glm::normalize(whereInter), o.rot)));
	}
	return std::nullopt;
}

intersection intersectPlane(const Object &o, Ray ray) {
	float k = dot(o.dim, ray.dir);
	if (k == 0.f) {
		return std::nullopt;
	}
	return optional(std::make_pair(-dot(o.dim, ray.pos - o.pos) / k,
								   k < 0.f ? o.dim : -o.dim));
}

intersection intersectBox(const Object &o, Ray ray) {
	vec3 newPos = rotation(ray.pos - o.pos, o.rot.conj()) / o.dim;
	vec3 newDir = rotation(ray.dir, o.rot.conj()) / o.dim;
	Ray newRay = Ray(newPos, newDir);

	intersection tx1 = Object(Shape::Plane, {1.f, 0.f, 0.f}, {1.f, 0.f, 0.f},
							  Quat(), o.c, o.mat, o.ior)
						   .intersect(newRay);
	intersection tx2 = Object(Shape::Plane, {-1.f, 0.f, 0.f}, {-1.f, 0.f, 0.f},
							  Quat(), o.c, o.mat, o.ior)
						   .intersect(newRay);
	intersection ty1 = Object(Shape::Plane, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
							  Quat(), o.c, o.mat, o.ior)
						   .intersect(newRay);
	intersection ty2 = Object(Shape::Plane, {0.f, -1.f, 0.f}, {0.f, -1.f, 0.f},
							  Quat(), o.c, o.mat, o.ior)
						   .intersect(newRay);
	intersection tz1 = Object(Shape::Plane, {0.f, 0.f, 1.f}, {0.f, 0.f, 1.f},
							  Quat(), o.c, o.mat, o.ior)
						   .intersect(newRay);
	intersection tz2 = Object(Shape::Plane, {0.f, 0.f, -1.f}, {0.f, 0.f, -1.f},
							  Quat(), o.c, o.mat, o.ior)
						   .intersect(newRay);

	swapIfMin(tx1, tx2);
	swapIfMin(ty1, ty2);
	swapIfMin(tz1, tz2);

	auto t1 = maxInter(maxInter(tx1, ty1), tz1);
	auto t2 = minInter(minInter(tx2, ty2), tz2);

	if (t2 < t1 || t2 < optional(std::make_pair(0.f, vec3(0.f)))) {
		return std::nullopt;
	}
	if (optional(std::make_pair(0.f, vec3(0.f))) < t1) {
		return t1;
	}
	return t2;
}
