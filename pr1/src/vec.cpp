#include "vec.hpp"

#include <cassert>
#include <cmath>

// Vec3

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {
}

Vec3::Vec3(const Vec3 &other) : x(other.x), y(other.y), z(other.z) {
}


Vec3 &Vec3::operator=(const Vec3 &other) {
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	return *this;
}

Vec3 Vec3::operator+() const {
	return *this;
}
Vec3 Vec3::operator-() const {
	return Vec3(-this->x, -this->y, -this->z);
}

Vec3 &Vec3::operator+=(const Vec3 &other) {
	return *this = *this + other;
}
Vec3 &Vec3::operator-=(const Vec3 &other) {
	return *this = *this - other;
}
Vec3 &Vec3::operator*=(float scale) {
	return *this = *this * scale;
}
Vec3 &Vec3::operator/=(float scale) {
	return *this = *this / scale;
}

Vec3 operator+(const Vec3 &first, const Vec3 &second) {
	return Vec3(first.x + second.x, first.y + second.y, first.z + second.z);
}

Vec3 operator-(const Vec3 &first, const Vec3 &second) {
	return Vec3(first.x - second.x, first.y - second.y, first.z - second.z);
}

Vec3 operator*(const Vec3 &v, float scale) {
	return Vec3(v.x * scale, v.y * scale, v.z * scale);
}
Vec3 operator/(const Vec3 &v, float scale) {
	assert(scale != 0.f && "Can't divide by zero");
	return Vec3(v.x / scale, v.y / scale, v.z / scale);
}

Vec3 operator*(float scale, const Vec3 &v) {
	return Vec3(v.x * scale, v.y * scale, v.z * scale);
}

const Vec3 &Vec3::norm() {
	float sumSquared =
		this->x * this->x + this->y * this->y + this->z * this->z;
	assert(sumSquared != 0.f && "Can't normalize zero vector");
	return *this /= std::sqrt(sumSquared);
}

// Vec4

Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {
}

Vec4::Vec4(const Vec4 &other) : x(other.x), y(other.y), z(other.z), w(other.w) {
}
Vec4::Vec4(const Vec3 &other, float w)
	: x(other.x), y(other.y), z(other.z), w(w) {
}


Vec4 &Vec4::operator=(const Vec4 &other) {
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	this->w = other.w;
	return *this;
}

Vec4 Vec4::operator+() const {
	return *this;
}
Vec4 Vec4::operator-() const {
	return Vec4(-this->x, -this->y, -this->z, -this->w);
}

Vec4 &Vec4::operator+=(const Vec4 &other) {
	return *this = *this + other;
}
Vec4 &Vec4::operator-=(const Vec4 &other) {
	return *this = *this - other;
}
Vec4 &Vec4::operator*=(float scale) {
	return *this = *this * scale;
}
Vec4 &Vec4::operator/=(float scale) {
	return *this = *this / scale;
}

Vec4 operator+(const Vec4 &first, const Vec4 &second) {
	return Vec4(first.x + second.x, first.y + second.y, first.z + second.z,
				first.w + second.w);
}

Vec4 operator-(const Vec4 &first, const Vec4 &second) {
	return Vec4(first.x - second.x, first.y - second.y, first.z - second.z,
				first.w + second.w);
}

Vec4 operator*(const Vec4 &v, float scale) {
	return Vec4(v.x * scale, v.y * scale, v.z * scale, v.w * scale);
}
Vec4 operator/(const Vec4 &v, float scale) {
	assert(scale != 0.f && "Can't divide by zero");
	return Vec4(v.x / scale, v.y / scale, v.z / scale, v.w / scale);
}

Vec4 operator*(float scale, const Vec4 &v) {
	return Vec4(v.x * scale, v.y * scale, v.z * scale, v.w * scale);
}

const Vec4 &Vec4::norm() {
	float sumSquared = this->x * this->x + this->y * this->y +
					   this->z * this->z + this->w * this->w;
	assert(sumSquared != 0.f && "Can't normalize zero vector");
	return *this /= std::sqrt(sumSquared);
}
