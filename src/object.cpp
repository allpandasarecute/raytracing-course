#include "object.hpp"
#include "quat.hpp"
#include "types.hpp"
#include <optional>

#define swapIfMin(x, y)                                                        \
	{                                                                          \
		if (x > y)                                                             \
			std::swap(x, y);                                                   \
	}

Object::Object()
	: s(Shape::None), pos(vec3(0.f)), dim(vec3(1.f)), rot({vec3(0.f), 1.f}),
	  color(Color()) {
}

Object::Object(Shape s, vec3 pos, vec3 dim, Quat rot, Color color)
	: s(s), pos(pos), dim(dim), rot(rot), color(color) {
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
	vec3 newPos = rotation((ray.pos - o.pos) / o.dim, o.rot.conj());
	vec3 newDir = rotation(ray.dir / o.dim, o.rot.conj());
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
		return intersection(t);
	}
	t += d / a;
	if (t > 0) {
		return intersection(t);
	}
	return std::nullopt;
}

intersection intersectPlane(const Object &o, Ray ray) {
	float k = dot(o.dim, ray.dir);
	vec3 newPos = ray.pos - o.pos;
	float step = -dot(o.dim, newPos);
	return (k == 0.f) ? std::nullopt : intersection(step / k);
}

intersection intersectBox(const Object &o, Ray ray) {
	vec3 newPos = rotation(ray.pos - o.pos, o.rot.conj());
	vec3 newDir = rotation(ray.dir, o.rot.conj());
	Ray newRay = Ray(newPos, newDir);

	intersection tx1 = Object(Shape::Plane, {o.dim.x, 0.f, 0.f},
							  {1.f, 0.f, 0.f}, Quat(), o.color)
						   .intersect(newRay);
	intersection tx2 = Object(Shape::Plane, {-o.dim.x, 0.f, 0.f},
							  {-1.f, 0.f, 0.f}, Quat(), o.color)
						   .intersect(newRay);
	intersection ty1 = Object(Shape::Plane, {0.f, o.dim.y, 0.f},
							  {0.f, 1.f, 0.f}, Quat(), o.color)
						   .intersect(newRay);
	intersection ty2 = Object(Shape::Plane, {0.f, -o.dim.y, 0.f},
							  {0.f, -1.f, 0.f}, Quat(), o.color)
						   .intersect(newRay);
	intersection tz1 = Object(Shape::Plane, {0.f, 0.f, o.dim.z},
							  {0.f, 0.f, 1.f}, Quat(), o.color)
						   .intersect(newRay);
	intersection tz2 = Object(Shape::Plane, {0.f, 0.f, -o.dim.z},
							  {0.f, 0.f, -1.f}, Quat(), o.color)
						   .intersect(newRay);

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
		return intersection(t1);
	}
	return intersection(t2);
}
