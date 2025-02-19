#include "camera.hpp"
#include "color.hpp"
#include "object.hpp"
#include "scene.hpp"
#include "types.hpp"
#include <iostream>
#include <ostream>

int main() {
	Scene s = Scene();
	s.w = 640;
	s.h = 480;
	s.bg = Color(0.f, 0.f, 0.5f);
	s.cam = Camera({0.f, 1.5f, 0.f}, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f},
				   {0.f, 0.f, -1.f}, 1.5708, {s.w, s.h});
	Plane p = Plane({0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f});
	Ellips e = Ellips({-1.f, 1.f, -5.f}, {2.f, 2.f, 2.f}, {1.f, 0.f, 0.f});
	Box b = Box({1.f, 2.5f, -3.f}, {0.5f, 0.5f, 0.5f},
				{{0.31246f, 0.15623f, 0.15623f}, 0.92388f}, {1.f, 1.f, 0.f});
	s.objs.push_back(&p);
	s.objs.push_back(&e);
	s.objs.push_back(&b);
	s.data.resize(s.w * s.h);
	std::cout << s.objs.size() << std::endl;
	s.generateImage();
	s.saveImage("test.ppm");
	return 0;
}
