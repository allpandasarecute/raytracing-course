package main

Ray :: struct {
	pos, dir: Vec3f,
}

getRay :: #force_inline proc(pos: Vec3f, dir: Vec3f) -> Ray {
	return Ray{pos, normalize(dir)}
}
