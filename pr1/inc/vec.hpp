#pragma once

class Vec3 {
  public:
	explicit Vec3(float x, float y, float z);
	Vec3(const Vec3 &other);
	Vec3(Vec3 &&other) noexcept = default;
	~Vec3() = default;

	Vec3 &operator=(const Vec3 &other);
	Vec3 &operator=(Vec3 &&other) noexcept = default;

	Vec3 operator+() const;
	Vec3 operator-() const;

	Vec3 &operator+=(const Vec3 &other);
	Vec3 &operator-=(const Vec3 &other);
	Vec3 &operator*=(float scale);
	Vec3 &operator/=(float scale);

	const Vec3 &norm();

	float x, y, z;
};

Vec3 operator+(const Vec3 &first, const Vec3 &second);
Vec3 operator-(const Vec3 &first, const Vec3 &second);
Vec3 operator*(const Vec3 &v, float scale);
Vec3 operator/(const Vec3 &v, float scale);
Vec3 operator*(float scale, const Vec3 &v);


class Vec4 {
  public:
	explicit Vec4(float x, float y, float z, float w);
	Vec4(const Vec4 &other);
	Vec4(const Vec3 &other, float w);
	Vec4(Vec4 &&other) noexcept = default;
	~Vec4() = default;

	Vec4 &operator=(const Vec4 &other);
	Vec4 &operator=(Vec4 &&other) noexcept = default;

	Vec4 operator+() const;
	Vec4 operator-() const;

	Vec4 &operator+=(const Vec4 &other);
	Vec4 &operator-=(const Vec4 &other);
	Vec4 &operator*=(float scale);
	Vec4 &operator/=(float scale);

	const Vec4 &norm();

	float x, y, z, w;
};

Vec4 operator+(const Vec4 &first, const Vec4 &second);
Vec4 operator-(const Vec4 &first, const Vec4 &second);
Vec4 operator*(const Vec4 &v, float scale);
Vec4 operator/(const Vec4 &v, float scale);
Vec4 operator*(float scale, const Vec4 &v);
