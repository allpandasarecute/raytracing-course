package main

import "core:math"
import "core:os"

main :: proc() {
	// s: Scene
	// s.w = 1920
	// s.h = 1080
	// s.raydepth = 8
	// s.bg = {0.5, 0.5, 2}
	// s.amb = {0.025, 0.025, 0.025}
	// append(
	// 	&s.lights,
	// 	Light{LightDir{dir = {0.534522, 0.801784, 0.267261}}, {1, 1, 1}},
	// 	Light{LightDot{pos = {-1, 0.5, -2}, att = {1, 0, 0.1}}, {0.125, 2, 0.125}},
	// 	Light{LightDot{pos = {1, 0.5, -6}, att = {1, 0, 0.1}}, {0.125, 0.125, 4}},
	// )
	// s.cam.pos = {0, 1.25, 2}
	// s.cam.right = {1, 0, 0}
	// s.cam.forward = {0, 0, -1}
	// s.cam.up = {0, 1, 0}
	// s.cam.fovx = math.tan(f32(1.5708) / 2)
	// s.cam.fovy = s.cam.fovx * f32(s.h) / f32(s.w)
	// append(
	// 	&s.objects,
	// 	Object {
	// 		shape = Ellips{r = {1, 1, 1}, rot = quaternion(x = 0, y = 0, z = 0, w = 1)},
	// 		pos = {2, 1, -2},
	// 		color = {1, 1, 1},
	// 		mat = Dielectric{ior = 1.04},
	// 	},
	// 	Object {
	// 		shape = Ellips{r = {1, 1, 1}, rot = quaternion(x = 0, y = 0, z = 0, w = 1)},
	// 		pos = {-2, 1, -2},
	// 		color = {0.7, 0.4, 0.1},
	// 		mat = Metallic{},
	// 	},
	// 	Object {
	// 		shape = Ellips{r = {1, 1, 1}, rot = quaternion(x = 0, y = 0, z = 0, w = 1)},
	// 		pos = {0, 1, -3},
	// 		color = {1, 0.25, 0.25},
	// 		mat = Diffuse{},
	// 	},
	// 	Object {
	// 		shape = Plane{norm = {0, 1, 0}},
	// 		pos = {0, 0, 0},
	// 		color = {0.2, 0.2, 0.2},
	// 		mat = Diffuse{},
	// 	},
	// 	Object {
	// 		shape = Box {
	// 			dim = {0.5, 0.5, 2.5},
	// 			rot = quaternion(x = 0.31246, y = 0.15623, z = 0.15623, w = 0.92388),
	// 		},
	// 		pos = {3, 2, -8},
	// 		color = {1, 1, 0.25},
	// 		mat = Diffuse{},
	// 	},
	// )
	// resize(&s.data, s.w * s.h)

	if (len(os.args) != 3) {
		os.exit(1)
	}
	defer delete(os.args)

	s := getScene(os.args[1])
	generateImage(s)
	saveImage(s, os.args[2])
	destroyScene(s)
}
