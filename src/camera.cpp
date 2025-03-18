#include "camera.hpp"
#include <glm/ext/scalar_constants.hpp>

Camera::Camera()
	: pos(vec3(0.f)), right(vec3(1.f, 0.f, 0.f)), up(vec3(0.f, 1.f, 0.f)),
	  forward(vec3(0.f, 0.f, 1.f)), fovx(glm::pi<float>() / 2.f),
	  fovy(glm::pi<float>() / 2.f) {
}

Camera::Camera(vec3 pos, vec3 right, vec3 up, vec3 forward, float fovx,
			   uvec2 imageSize)
	: pos(pos), right(right), up(up), forward(forward),
	  fovx(glm::tan(fovx / 2.f)), fovy(this->fovx * imageSize.y / imageSize.x) {
}

Camera::Camera(const Camera &arg)
	: pos(arg.pos), right(arg.right), up(arg.up), forward(arg.forward),
	  fovx(arg.fovx), fovy(arg.fovy) {
}
