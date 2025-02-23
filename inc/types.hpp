#pragma once

#include "glm/glm.hpp"
#include <limits>
#include <optional>
#include <string>
#include <utility>
#include <vector>


const float FLOAT_MAX = std::numeric_limits<float>::infinity();
const float FLOAT_MIN = -std::numeric_limits<float>::infinity();

using glm::dot, glm::cross;
using glm::vec3, glm::vec4;
using std::string, std::vector, std::optional, std::pair;

typedef std::uint8_t byte;
typedef unsigned int uint;
typedef std::pair<uint, uint> ind;
typedef std::optional<float> intersection;
