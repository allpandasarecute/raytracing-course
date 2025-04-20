package main

import "core:math"

LightSampler :: struct {
	obj: Object, // WARNING: obj.Shape is Ellips or Box
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
		rotate(obj.shape.(Ellips).r * generateRandDir(), obj.shape.(Ellips).rot) + obj.pos - pos,
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

	return normalize(rotate(obj.shape.(Box).dim * m, obj.shape.(Box).rot))
}

lightSample :: #force_inline proc(#by_ptr s: LightSampler, #by_ptr pos: Vec3f) -> Vec3f {
	#partial switch &_ in s.obj.shape {
	case Ellips:
		return ellipsSample(s.obj, pos)
	case Box:
		return boxSample(s.obj, pos)
	case Plane:
		panic("Plane object in sampler")
	case:
		panic("Not inititalized object in sampler")
	}
}

ellipsPDF :: #force_inline proc(#by_ptr obj: Object, #by_ptr pos, dir: Vec3f) -> f32 {
	ans: f32 = 0
	i: Intersection
	ok: bool

	if i, ok = intersectEllips(obj, Ray{pos, dir}).?; !ok {
		return ans
	}

	whereInter := movePoint(pos + dir * i.t, dir)
	point := rotate(whereInter - obj.pos, conj(obj.shape.(Ellips).rot))

	sqNorm := point * point / obj.shape.(Ellips).r / obj.shape.(Ellips).r
	sqR := obj.shape.(Ellips).r.yxx * obj.shape.(Ellips).r.zzy
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
	point = rotate(whereInter - obj.pos, conj(obj.shape.(Ellips).rot))
	sqNorm = point * point / obj.shape.(Ellips).r / obj.shape.(Ellips).r
	ans +=
		length2(pos - whereInter) /
		abs(dot(i.norm, pos - whereInter)) /
		4 /
		math.PI /
		math.sqrt(dot(sqNorm, sqR))

	return ans
}

boxPDF :: #force_inline proc(#by_ptr obj: Object, #by_ptr pos, dir: Vec3f) -> f32 {
	ans: f32 = 0
	i: Intersection
	ok: bool
	temp := 4 * obj.shape.(Box).dim.yxx * obj.shape.(Box).dim.zzy
	t := 2 * (temp.x + temp.y + temp.z)

	if i, ok = intersectEllips(obj, Ray{pos, dir}).?; !ok {
		return ans
	}

	whereInter := movePoint(pos + dir * i.t, dir)
	point := rotate(whereInter - obj.pos, conj(obj.shape.(Ellips).rot))
	ans += 1 / t * length2(pos - whereInter) / abs(dot(i.norm, normalize(pos - whereInter)))

	if i, ok = intersectEllips(obj, Ray{whereInter, dir}).?; !ok {
		return ans
	}

	whereInter = whereInter + dir * i.t
	point = rotate(whereInter - obj.pos, conj(obj.shape.(Ellips).rot))
	ans += 1 / t * length2(pos - whereInter) / abs(dot(i.norm, normalize(pos - whereInter)))

	return ans
}

lightPDF :: #force_inline proc(#by_ptr s: LightSampler, #by_ptr pos, norm, dir: Vec3f) -> f32 {
	#partial switch &_ in s.obj.shape {
	case Ellips:
		return ellipsPDF(s.obj, pos, dir)
	case Box:
		return boxPDF(s.obj, pos, dir)
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
