#include "quat.hpp"

Quat::Quat() : v(vec3(0.f)), w(1.f) {
}

Quat::Quat(const Quat &arg) : v(arg.v), w(arg.w) {
}

Quat::Quat(vec3 v, float w) : v(v), w(w) {
}

Quat Quat::conj() const {
	return Quat(-this->v, w);
}

Quat operator*(const Quat &a, const Quat &b) {
	return Quat(a.w * b.v + b.w * a.v + cross(a.v, b.v),
				a.w * b.w - dot(a.v, b.v));
}

vec3 rotation(vec3 v, Quat rot) {
	return (rot * Quat(v, 0.f) * rot.conj()).v;
}
