#include "light.hpp"
#include "glm/geometric.hpp"
#include "types.hpp"

Light::Light()
	: type(LightType::None), pos({0.f, 0.f, 0.f}), dir({1.f, 0.f, 0.f}),
	  c({0.f, 0.f, 0.f}), att({1.f, 0.f, 0.1f}) {
}

Light::Light(LightType type, vec3 pos, vec3 dir, vec3 c, vec3 att)
	: type(type), pos(pos), dir(dir), c(c), att(att) {
}

float Light::intensity(float r) {
	return 1.f / (att.x + att.y * r + att.z * r * r);
}

tuple<color, float, vec3> Light::getLight(vec3 norm, vec3 pos) {
	if (this->type == LightType::Dir) {
		float d = dot(norm, this->dir);
		if (d > 0) {
			return {d * this->c, FLOAT_MAX, this->dir};
		}
	} else if (this->type == LightType::Dot) {
		float d = dot(norm, glm::normalize(this->pos - pos));
		if (d > 0) {
			float r = glm::length(this->pos - pos);
			return {d * this->c /
						(this->att.x + this->att.y * r + this->att.z * r * r),
					r, glm::normalize(this->pos - pos)};
		}
	}
	return {color(0.f), 0.f, {1.f, 0.f, 0.f}};
}
