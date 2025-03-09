#include "light.hpp"

Light::Light()
	: type(LightType::Dot), pos({0.f, 0.f, 0.f}), dir({1.f, 0.f, 0.f}),
	  c({0.f, 0.f, 0.f}), att({0.f, 1.f, 1.f}) {
}

Light::Light(LightType type, vec3 pos, vec3 dir, vec3 c, vec3 att)
	: type(type), pos(pos), dir(dir), c(c), att(att) {
}

float Light::intensity(float r) {
	return 1.f / (att.x + att.y * r + att.z * r * r);
}
