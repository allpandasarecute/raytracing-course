#pragma once

#include "glm/glm.hpp"
#include <limits>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>


constexpr float FLOAT_MAX = std::numeric_limits<float>::infinity();
constexpr float FLOAT_MIN = -std::numeric_limits<float>::infinity();
constexpr float DELTA = 0.0001f;

using glm::dot, glm::cross;
using glm::vec3, glm::uvec2;
using std::string, std::vector, std::optional, std::pair, std::tuple;

typedef std::uint8_t byte;
typedef unsigned int uint;
typedef vec3 color;
typedef optional<tuple<float, vec3, bool>> intersection; // t, norm
