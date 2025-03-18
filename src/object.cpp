#include "object.hpp"
#include "glm/geometric.hpp"
#include "quat.hpp"
#include "types.hpp"
#include <optional>


bool comp(const intersection &a, const intersection &b) {
	return ((a.has_value() && b.has_value() &&
			 (std::get<0>(a.value()) < std::get<0>(b.value()))) ||
			(!a.has_value() && b.has_value()));
}

intersection maxInter(const intersection &a, const intersection &b) {
	return comp(a, b) ? b : a;
}

intersection minInter(const intersection &a, const intersection &b) {
	return comp(a, b) ? a : b;
}

#define swapIfMin(x, y)                                                        \
	{                                                                          \
		if (comp(y, x))                                                        \
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
	float t1 = (-b - d) / (2 * a);
	float t2 = (-b + d) / (2 * a);

	float x1 = glm::min(t1, t2);
	float x2 = glm::max(t1, t2);

	if (x2 <= 0) {
		return std::nullopt;
	} else if (x1 <= 0) {
		return std::make_tuple(
			x2,
			-rotation(glm::normalize((newPos + x2 * newDir) / o.dim), o.rot),
			true);
	} else {
		return std::make_tuple(
			x1, rotation(glm::normalize((newPos + x1 * newDir) / o.dim), o.rot),
			false);
	}
}

intersection intersectPlane(const Object &o, Ray ray) {
	float d = dot(o.dim, ray.dir);
	if (d == 0.f) {
		return std::nullopt;
	}
	float t = -dot(o.dim, ray.pos - o.pos) / d;
	bool isInter = (d > 0);
	return std::make_tuple(t, isInter ? -o.dim : o.dim, isInter);
}

float sign(float x) {
	return (x < 0.f) ? -1.f : 1.f;
}

intersection intersectBox(const Object &o, Ray ray) {
	vec3 newPos = rotation((ray.pos - o.pos), o.rot.conj());
	vec3 newDir = rotation(ray.dir, o.rot.conj());

	vec3 tp1 = (o.dim - newPos) / newDir;
	vec3 tp2 = (-o.dim - newPos) / newDir;

	float t1 =
		glm::max(glm::max(glm::min(tp1.x, tp2.x), glm::min(tp1.y, tp2.y)),
				 glm::min(tp1.z, tp2.z));
	float t2 =
		glm::min(glm::min(glm::max(tp1.x, tp2.x), glm::max(tp1.y, tp2.y)),
				 glm::max(tp1.z, tp2.z));

	if (t1 > t2 || t2 < 0.f) {
		return std::nullopt;
	}

	float t = (t1 < 0.f) ? t2 : t1;
	bool isInter = (t1 < 0.f);
	vec3 norm = (newPos + t * newDir) / o.dim;

	if (glm::abs(norm.x) > glm::abs(norm.y) &&
		glm::abs(norm.x) > glm::abs(norm.z)) {
		norm = {sign(norm.x), 0.f, 0.f};
	} else if ((glm::abs(norm.y) > glm::abs(norm.x) &&
				glm::abs(norm.y) > glm::abs(norm.z))) {
		norm = {0.f, sign(norm.y), 0.f};
	} else {
		norm = {0.f, 0.f, sign(norm.z)};
	}

	return std::make_tuple(t, rotation((isInter ? -1.f : 1.f) * norm, o.rot),
						   isInter);
}
