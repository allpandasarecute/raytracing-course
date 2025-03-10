#pragma once

#include "glm/glm.hpp"
#include <limits>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>


const float FLOAT_MAX = std::numeric_limits<float>::infinity();
const float FLOAT_MIN = -std::numeric_limits<float>::infinity();

using glm::dot, glm::cross;
using glm::vec3, glm::uvec2;
using std::string, std::vector, std::optional, std::pair, std::tuple;

typedef std::uint8_t byte;
typedef unsigned int uint;
typedef uvec2 ind;
typedef vec3 color;
typedef optional<pair<float, vec3>> intersection;
