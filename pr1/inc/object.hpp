#pragma once
#include "ray.hpp"

#include <optional>

class Object {
  public:
	Object() = default;
	Object(const Object &) = default;
	~Object() = default;

	virtual std::optional<float> intersection(Ray ray) const = 0;
};
