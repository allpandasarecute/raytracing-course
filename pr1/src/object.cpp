#include "object.hpp"
#include "types.hpp"
#include <iostream>

#define swapIfMin(x, y)                                                        \
	{                                                                          \
		if (x > y)                                                             \
			std::swap(x, y);                                                   \
	}

// Ellips

Ellips::Ellips() : pos(vec3(0.f)), r(vec3(1.f)), color(Color()) {
}

Ellips::Ellips(const Ellips &arg) : pos(arg.pos), r(arg.r), color(arg.color) {
}

Ellips::Ellips(vec3 pos, vec3 r, Color color) : pos(pos), r(r), color(color) {
}

intersect Ellips::intersection(Ray ray) const {
	vec3 newPos = (ray.pos - this->pos) / this->r;
	vec3 newDir = ray.dir / this->r;
	float a = dot(newDir, newDir);
	float b = 2 * dot(newPos, newDir);
	float c = dot(newPos, newPos) - 1;
	float d = b * b - 4 * a * c;
	if (d < 0) {
		return std::nullopt;
	}
	d = glm::sqrt(d);
	float t = (-b - d) / (2 * a);
	if (t > 0) {
		return intersect(t);
	}
	t += d / a;
	if (t > 0) {
		return intersect(t);
	}
	return std::nullopt;
};

Color Ellips::c() const {
	return this->color;
}

// Box

Box::Box()
	: pos(vec3(0.f)), dim(vec3(1.f)), rot(Quat({0.f, 0.f, 0.f}, 1.f)),
	  color(Color()) {
}

Box::Box(const Box &arg)
	: pos(arg.pos), dim(arg.dim), rot(arg.rot), color(arg.color) {
}

Box::Box(vec3 pos, vec3 dim, Quat rot, Color color)
	: pos(pos), dim(dim), rot(rot), color(color) {
}

int counter = 0;

intersect Box::intersection(Ray ray) const {
	vec3 newPos =
		(this->rot.conj() * Quat(ray.pos - this->pos, 0.f) * this->rot).v;
	vec3 newDir = (this->rot.conj() * Quat(ray.dir, 0.f) * this->rot).v;
	Ray newRay = Ray(newPos, newDir);

	intersect tx1 = Plane({this->dim.x, 0.f, 0.f}, {1.f, 0.f, 0.f}, this->color)
						.intersection(newRay);
	intersect tx2 =
		Plane({-this->dim.x, 0.f, 0.f}, {-1.f, 0.f, 0.f}, this->color)
			.intersection(newRay);
	intersect ty1 = Plane({0.f, this->dim.y, 0.f}, {0.f, 1.f, 0.f}, this->color)
						.intersection(newRay);
	intersect ty2 =
		Plane({0.f, -this->dim.y, 0.f}, {0.f, -1.f, 0.f}, this->color)
			.intersection(newRay);
	intersect tz1 = Plane({0.f, 0.f, this->dim.z}, {0.f, 0.f, 1.f}, this->color)
						.intersection(newRay);
	intersect tz2 =
		Plane({0.f, 0.f, -this->dim.z}, {0.f, 0.f, -1.f}, this->color)
			.intersection(newRay);

	swapIfMin(tx1, tx2);
	swapIfMin(ty1, ty2);
	swapIfMin(tz1, tz2);

	float t1 =
		glm::max(glm::max(tx1.value_or(FLOAT_MIN), ty1.value_or(FLOAT_MIN)),
				 tz1.value_or(FLOAT_MIN));
	float t2 =
		glm::min(glm::min(tx2.value_or(FLOAT_MAX), ty2.value_or(FLOAT_MAX)),
				 tz2.value_or(FLOAT_MAX));
	if (t1 > t2 || t2 < 0) {
		return std::nullopt;
	}
	if (t1 > 0) {
		return intersect(t1);
	}
	return intersect(t2);
};

Color Box::c() const {
	return this->color;
}

// Plane

Plane::Plane() : pos(vec3(0.f)), norm(vec3(0.f, 1.f, 0.f)), color(Color()) {
}

Plane::Plane(const Plane &arg)
	: pos(arg.pos), norm(arg.norm), color(arg.color) {
}

Plane::Plane(vec3 pos, vec3 norm, Color color)
	: pos(pos), norm(norm), color(color) {
}

intersect Plane::intersection(Ray ray) const {
	float k = dot(this->norm, ray.dir);
	vec3 newPos = ray.pos - this->pos;
	float step = -dot(this->norm, newPos);
	return (k == 0.f) ? std::nullopt : intersect(step / k);
}

Color Plane::c() const {
	return this->color;
}
