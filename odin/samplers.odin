package main

import "core:math"

LightSampler :: struct {
	obj: Object, // WARNING: obj.Shape is Ellips, Box or Triangle 
}

CosineSampler :: struct {}

MixSampler :: struct {
	samplers: [dynamic]Sampler,
}

Sampler :: union {
	LightSampler,
	CosineSampler,
	MixSampler,
}

cosineSample :: #force_inline proc(#by_ptr norm: Vec3f) -> Vec3f {
	return normalize(generateRandDir() + norm)
}

cosinePDF :: #force_inline proc(#by_ptr norm, dir: Vec3f) -> f32 {
	d := dot(norm, dir)
	return 0 if d < 0 else d / math.PI
}

mixSample :: #force_inline proc(#by_ptr s: MixSampler, #by_ptr pos, norm: Vec3f) -> Vec3f {
	return sample(s.samplers[randU64Max(u64(len(s.samplers)))], pos, norm)
}

mixPDF :: #force_inline proc(#by_ptr s: MixSampler, #by_ptr pos, norm, dir: Vec3f) -> f32 {
	temp: f32 = 0
	for &t in s.samplers {
		temp += pdf(t, pos, norm, dir)
	}
	return temp / f32(len(s.samplers))
}

ellipsSample :: #force_inline proc(#by_ptr obj: Object, #by_ptr pos: Vec3f) -> Vec3f {
	return normalize(
		rotate(obj.shape.(Ellips).rot, obj.shape.(Ellips).r * generateRandDir()) + obj.pos - pos,
	)
}

boxSample :: #force_inline proc(#by_ptr obj: Object, #by_ptr pos: Vec3f) -> Vec3f {
	dim := obj.shape.(Box).dim
	t := randF32MinMax(0, 2 * (dim.x + dim.y + dim.z))
	m: Vec3f

	if t < dim.x {
		m = {1, randF32Dir(), randF32Dir()}
	} else if t < 2 * dim.x {
		m = {-1, randF32Dir(), randF32Dir()}
	} else if t < 2 * dim.x + dim.y {
		m = {randF32Dir(), 1, randF32Dir()}
	} else if t < 2 * (dim.x + dim.y) {
		m = {randF32Dir(), -1, randF32Dir()}
	} else if t < 2 * (dim.x + dim.y) + dim.z {
		m = {randF32Dir(), randF32Dir(), 1}
	} else {
		m = {randF32Dir(), randF32Dir(), -1}
	}

	return normalize(rotate(obj.shape.(Box).rot, dim * m) + obj.pos - pos)
}

triangleSample :: #force_inline proc(#by_ptr obj: Object, #by_ptr pos: Vec3f) -> Vec3f {
	tri := obj.shape.(Triangle)
	u, v := randF32(), randF32()
	if u + v > 1 {
		u = 1 - u
		v = 1 - v
	}
	return tri.p1 + u * (tri.p2 - tri.p1) + v * (tri.p3 - tri.p1) - pos
}

lightSample :: #force_inline proc(#by_ptr s: LightSampler, #by_ptr pos: Vec3f) -> Vec3f {
	switch &_ in s.obj.shape {
	case Ellips:
		return ellipsSample(s.obj, pos)
	case Box:
		return boxSample(s.obj, pos)
	case Triangle:
		return triangleSample(s.obj, pos)
	case Plane:
		panic("Plane object in sampler")
	case:
		panic("Not inititalized object in sampler")
	}
}

ellipsPDF :: #force_inline proc(#by_ptr obj: Object, #by_ptr pos, dir: Vec3f) -> f32 {
	e := obj.shape.(Ellips)
	ans: f32 = 0
	i: Intersection
	ok: bool

	if i, ok = intersectEllips(obj, Ray{pos, dir}).?; !ok {
		return ans
	}

	whereInter := movePoint(pos + dir * i.t, dir)
	point := rotate(conj(e.rot), whereInter - obj.pos)

	sqNorm := point * point / e.r / e.r
	sqR := e.r.yxx * e.r.zzy
	sqR *= sqR
	ans +=
		length2(pos - whereInter) /
		abs(dot(i.norm, pos - whereInter)) /
		4 /
		math.PI /
		math.sqrt(dot(sqNorm, sqR))

	if i, ok = intersectEllips(obj, Ray{whereInter, dir}).?; !ok {
		return ans
	}

	whereInter = whereInter + dir * i.t
	sqNorm = point * point / e.r / e.r
	ans +=
		length2(pos - whereInter) /
		abs(dot(i.norm, pos - whereInter)) /
		4 /
		math.PI /
		math.sqrt(dot(sqNorm, sqR))

	return ans
}

boxPDF :: #force_inline proc(#by_ptr obj: Object, #by_ptr pos, dir: Vec3f) -> f32 {
	b := obj.shape.(Box)
	ans: f32 = 0
	i: Intersection
	ok: bool
	temp := 4 * b.dim.yxx * b.dim.zzy
	t := 2 * (temp.x + temp.y + temp.z)

	if i, ok = intersectBox(obj, Ray{pos, dir}).?; !ok {
		return ans
	}

	whereInter := movePoint(pos + dir * i.t, dir)
	ans += 1 / t * length2(pos - whereInter) / abs(dot(i.norm, normalize(pos - whereInter)))

	if i, ok = intersectBox(obj, Ray{whereInter, dir}).?; !ok {
		return ans
	}

	whereInter = whereInter + dir * i.t
	ans += 1 / t * length2(pos - whereInter) / abs(dot(i.norm, normalize(pos - whereInter)))

	return ans
}

trianglePDF :: #force_inline proc(#by_ptr obj: Object, #by_ptr pos, dir: Vec3f) -> f32 {
	tri := obj.shape.(Triangle)
	i: Intersection
	ok: bool
	s := length(cross(tri.p2 - tri.p1, tri.p3 - tri.p1)) / 2

	if i, ok = intersectTriangle(obj, Ray{pos, dir}).?; !ok {
		return 0
	}
	whereInter := movePoint(pos + dir * i.t, dir)
	return 1 / s * length2(pos - whereInter) / abs(dot(i.norm, normalize(pos - whereInter)))
}

lightPDF :: #force_inline proc(#by_ptr s: LightSampler, #by_ptr pos, norm, dir: Vec3f) -> f32 {
	switch &_ in s.obj.shape {
	case Ellips:
		return ellipsPDF(s.obj, pos, dir)
	case Box:
		return boxPDF(s.obj, pos, dir)
	case Triangle:
		return trianglePDF(s.obj, pos, dir)
	case Plane:
		panic("Plane object in sampler")
	case:
		panic("Not inititalized object in sampler")
	}
}

pdf :: proc(#by_ptr s: Sampler, #by_ptr pos, norm, dir: Vec3f) -> f32 {
	switch &t in s {
	case LightSampler:
		return lightPDF(t, pos, norm, dir)
	case CosineSampler:
		return cosinePDF(norm, dir)
	case MixSampler:
		return mixPDF(t, pos, norm, dir)
	case:
		panic("Not inititalized sampler")
	}
}

sample :: proc(#by_ptr s: Sampler, #by_ptr pos, norm: Vec3f) -> Vec3f {
	switch &t in s {
	case LightSampler:
		return lightSample(t, pos)
	case CosineSampler:
		return cosineSample(norm)
	case MixSampler:
		return mixSample(t, pos, norm)
	case:
		panic("Not inititalized sampler")
	}
}
