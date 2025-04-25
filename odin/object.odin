package main

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

Triangle :: struct {
	p1, p2, p3: Vec3f,
	rot:        Quat,
}

Ellips :: struct {
	r:   Vec3f,
	rot: Quat,
}

Plane :: struct {
	norm: Vec3f,
}

Shape :: union {
	Triangle,
	Plane,
	Ellips,
	Box,
}

Finite :: union {
	Ellips,
	Box,
	Triangle,
}

Infinite :: union {
	Plane,
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

intersectTriangle :: proc(#by_ptr o: Object, #by_ptr ray: Ray) -> Maybe(Intersection) {
	tri := o.shape.(Triangle)
	newPos := rotate(conj(tri.rot), ray.pos - o.pos)
	newDir := rotate(conj(tri.rot), ray.dir)

	c1 := tri.p2 - tri.p1
	c2 := tri.p3 - tri.p1
	c3 := -newDir
	c4 := newPos - tri.p1

	m := matrix[3, 3]f32{
		c1.x, c2.x, c3.x, 
		c1.y, c2.y, c3.y, 
		c1.z, c2.z, c3.z, 
	}

	ans := mul(inverse(m), c4) // (u, v, t)

	if ans.x < 0 || ans.y < 0 || ans.x + ans.y > 1 || ans.z < 0 {
		return nil
	}

	norm := normalize(rotate(tri.rot, cross(c1, c2)))
	isInside := (dot(newDir, norm) > 0)
	return Intersection{ans.z, -norm if isInside else norm, isInside}
}

intersectEllips :: proc(#by_ptr o: Object, #by_ptr ray: Ray) -> Maybe(Intersection) {
	e := o.shape.(Ellips)
	newPos := rotate(conj(e.rot), ray.pos - o.pos) / e.r
	newDir := rotate(conj(e.rot), ray.dir) / e.r

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
			norm = -normalize(rotate(e.rot, (newPos + x2 * newDir) / e.r)),
			isInside = true,
		}
	}
	return Intersection {
		t = x1,
		norm = normalize(rotate(e.rot, (newPos + x1 * newDir) / e.r)),
		isInside = false,
	}
}

intersectPlane :: proc(#by_ptr o: Object, #by_ptr ray: Ray) -> Maybe(Intersection) {
	p := o.shape.(Plane)
	d := dot(p.norm, ray.dir)
	if d == 0 {
		return nil
	}

	t := -dot(p.norm, ray.pos - o.pos) / d
	if t <= 0 {
		return nil
	}
	isInside: bool = (d > 0)
	return Intersection{t = t, norm = -p.norm if isInside else p.norm, isInside = isInside}
}

intersectBox :: proc(#by_ptr o: Object, #by_ptr ray: Ray) -> Maybe(Intersection) {
	b := o.shape.(Box)
	newPos := rotate(conj(b.rot), ray.pos - o.pos)
	newDir := rotate(conj(b.rot), ray.dir)

	tp1 := (b.dim - newPos) / newDir
	tp2 := (-b.dim - newPos) / newDir

	t1 := max(min(tp1.x, tp2.x), min(tp1.y, tp2.y), min(tp1.z, tp2.z))
	t2 := min(max(tp1.x, tp2.x), max(tp1.y, tp2.y), max(tp1.z, tp2.z))

	if t1 > t2 || t2 <= 0 {
		return nil
	}

	t := t2 if t1 <= 0 else t1
	isInside: bool = (t1 < 0)
	norm := (newPos + t * newDir) / b.dim

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
		norm = rotate(b.rot, -norm if isInside else norm),
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
	case Triangle:
		return intersectTriangle(o, ray)
	case:
		panic("Not initialized object")
	}
}
