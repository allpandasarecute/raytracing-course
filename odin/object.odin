package main

import "core:fmt"
import "core:math"

Diffuse :: struct {}
Metallic :: struct {}
Dielectric :: struct {
	ior: f32,
}

Material :: union {
	Diffuse,
	Metallic,
	Dielectric,
}

Box :: struct {
	dim: Vec3f,
	rot: Quat,
}

Ellips :: struct {
	r:   Vec3f,
	rot: Quat,
}

Plane :: struct {
	norm: Vec3f,
}

Shape :: union {
	Plane,
	Ellips,
	Box,
}

Object :: struct {
	shape: Shape,
	mat:   Material,
	color: Color,
	pos:   Vec3f,
	emm:   Color,
}

Intersection :: struct {
	t:        f32,
	norm:     Vec3f,
	isInside: bool,
}

sign :: #force_inline proc(x: f32) -> f32 {
	return -1 if x < 0 else 1
}

intersectEllips :: proc(#by_ptr o: Object, #by_ptr ray: Ray) -> Maybe(Intersection) {
	newPos := rotate(ray.pos - o.pos, conj(o.shape.(Ellips).rot)) / o.shape.(Ellips).r
	newDir := rotate(ray.dir, conj(o.shape.(Ellips).rot)) / o.shape.(Ellips).r

	a := dot(newDir, newDir)
	b := 2 * dot(newPos, newDir)
	c := dot(newPos, newPos) - 1

	d := b * b - 4 * a * c

	if d < 0 {
		return nil
	}

	d = math.sqrt(d)

	x1 := (-b - d) / (2 * a)
	x2 := (-b + d) / (2 * a)

	if x2 <= 0 {
		return nil
	}
	if x1 <= 0 {
		return Intersection {
			t = x2,
			norm = -normalize(
				rotate((newPos + x2 * newDir) / o.shape.(Ellips).r, o.shape.(Ellips).rot),
			),
			isInside = true,
		}
	}
	return Intersection {
		t = x1,
		norm = normalize(
			rotate((newPos + x1 * newDir) / o.shape.(Ellips).r, o.shape.(Ellips).rot),
		),
		isInside = false,
	}
}

intersectPlane :: proc(#by_ptr o: Object, #by_ptr ray: Ray) -> Maybe(Intersection) {
	d := dot(o.shape.(Plane).norm, ray.dir)
	if d == 0 {
		return nil
	}

	t := -dot(o.shape.(Plane).norm, ray.pos - o.pos) / d
	if t <= 0 {
		return nil
	}
	isInside: bool = (d > 0)
	return Intersection {
		t = t,
		norm = -o.shape.(Plane).norm if isInside else o.shape.(Plane).norm,
		isInside = isInside,
	}
}

intersectBox :: proc(#by_ptr o: Object, #by_ptr ray: Ray) -> Maybe(Intersection) {
	newPos := rotate(ray.pos - o.pos, conj(o.shape.(Box).rot))
	newDir := rotate(ray.dir, conj(o.shape.(Box).rot))

	tp1 := (o.shape.(Box).dim - newPos) / newDir
	tp2 := (-o.shape.(Box).dim - newPos) / newDir

	t1 := max(min(tp1.x, tp2.x), min(tp1.y, tp2.y), min(tp1.z, tp2.z))
	t2 := min(max(tp1.x, tp2.x), max(tp1.y, tp2.y), max(tp1.z, tp2.z))

	if t1 > t2 || t2 <= 0 {
		return nil
	}

	t := t2 if t1 <= 0 else t1
	isInside: bool = (t1 < 0)
	norm := (newPos + t * newDir) / o.shape.(Box).dim

	if (abs(norm.x) > abs(norm.y) && abs(norm.x) > abs(norm.z)) {
		norm = {sign(norm.x), 0, 0}
	} else {
		if (abs(norm.y) > abs(norm.x) && abs(norm.y) > abs(norm.z)) {
			norm = {0, sign(norm.y), 0}
		} else {
			norm = {0, 0, sign(norm.z)}
		}
	}
	return Intersection {
		t = t,
		norm = rotate(-norm if isInside else norm, o.shape.(Box).rot),
		isInside = isInside,
	}
}

intersectObject :: proc(#by_ptr o: Object, #by_ptr ray: Ray) -> Maybe(Intersection) {
	switch _ in o.shape {
	case Plane:
		return intersectPlane(o, ray)
	case Ellips:
		return intersectEllips(o, ray)
	case Box:
		return intersectBox(o, ray)
	case:
		panic("Not initialized object")
	}
}
