#include "ray.hpp"

Ray::Ray(vec3 pos, vec3 dir) : pos(pos), dir(glm::normalize(dir)) {
}
