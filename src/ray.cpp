#include "ray.hpp"

Ray::Ray() : pos(vec3(0.f)), dir(vec3(0.f, 0.f, 1.f)) {
}

Ray::Ray(const Ray &arg) : pos(arg.pos), dir(arg.dir) {
}

Ray::Ray(vec3 pos, vec3 dir) : pos(pos), dir(glm::normalize(dir)) {
}
