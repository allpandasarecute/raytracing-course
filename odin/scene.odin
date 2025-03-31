package main

import "core:bufio"
import "core:fmt"
import "core:io"
import "core:math"
import "core:math/rand"
import "core:os"
import "core:slice"
import "core:strconv"
import "core:strings"

Camera :: struct {
	pos, right, up, forward: Vec3f,
	fovx, fovy:              f32,
}

Scene :: struct {
	data:     [dynamic]ColorSave,
	w, h:     u64,
	cam:      Camera,
	bg:       Color,
	raydepth: u64,
	samples:  u64,
	lights:   [dynamic]Light,
	objects:  [dynamic]Object,
}

SceneIntersection :: struct {
	using _:  Intersection,
	objIndex: int,
}

saveImage :: proc(#by_ptr s: Scene, #by_ptr file: string) {
	f, e := os.open(file, os.O_CREATE | os.O_WRONLY, 0o644)
	defer os.close(f)

	w: bufio.Writer
	bufio.writer_init(&w, os.stream_from_handle(f), 4096)
	defer bufio.writer_destroy(&w)
	defer bufio.writer_flush(&w)

	b := strings.builder_make()
	defer strings.builder_destroy(&b)

	strings.write_string(&b, "P6\n")
	strings.write_u64(&b, s.w)
	strings.write_string(&b, " ")
	strings.write_u64(&b, s.h)
	strings.write_string(&b, "\n255\n")

	bufio.writer_write_string(&w, strings.to_string(b))
	bufio.writer_write(&w, slice.bytes_from_ptr(&s.data[0], len(s.data) * size_of(ColorSave)))
}

randF32Dir :: #force_inline proc() -> f32 {
	return rand.float32_uniform(-1, 1)
}

randF32 :: #force_inline proc() -> f32 {
	return rand.float32_uniform(0, 1)
}

generateRandDir :: proc() -> Vec3f {
	v := Vec3f{randF32Dir(), randF32Dir(), randF32Dir()}
	l := length(v)
	return v / l if l <= 1 else generateRandDir()
}

generateRandReflection :: #force_inline proc(#by_ptr norm: Vec3f) -> Vec3f {
	v := generateRandDir()
	return v if dot(norm, v) > 0 else -v
}

generateRay :: proc(#by_ptr s: Scene, x, y: u64, dx, dy: f32) -> Ray {
	newX := (2 * (f32(x) + dx) / f32(s.w) - 1) * s.cam.fovx
	newY := -(2 * (f32(y) + dy) / f32(s.h) - 1) * s.cam.fovy
	return getRay(s.cam.pos, newX * s.cam.right + newY * s.cam.up + s.cam.forward)
}

intersectScene :: proc(#by_ptr s: Scene, #by_ptr ray: Ray, r: f32) -> Maybe(SceneIntersection) {
	ans: Maybe(SceneIntersection) = nil
	for &o, i in s.objects {
		if temp, ok := intersectObject(o, ray).?;
		   ok && temp.t < r && (ans == nil || ans.(SceneIntersection).t > temp.t) {
			ans = SceneIntersection{temp, i}
		}
	}
	return ans
}

raytraceScene :: proc(#by_ptr s: Scene, #by_ptr ray: Ray, depth: u64) -> Color {
	if depth >= s.raydepth {
		return Color{0, 0, 0}
	}

	si: SceneIntersection
	ok: bool
	if si, ok = intersectScene(s, ray, math.F32_MAX).?; !ok {
		return s.bg
	}

	whereInter := ray.pos + si.t * ray.dir

	switch mat in s.objects[si.objIndex].mat {
	case Diffuse:
		dir := generateRandReflection(si.norm)
		return(
			s.objects[si.objIndex].emm +
			2 *
				raytraceScene(s, getRay(movePoint(whereInter, si.norm), dir), depth + 1) *
				s.objects[si.objIndex].color *
				dot(dir, si.norm) \
		)
	case Metallic:
		return(
			s.objects[si.objIndex].emm +
			s.objects[si.objIndex].color *
				raytraceScene(
					s,
					getRay(
						movePoint(whereInter, si.norm),
						ray.dir - 2 * dot(ray.dir, si.norm) * si.norm,
					),
					depth + 1,
				) \
		)
	case Dielectric:
		d := dot(si.norm, ray.dir)
		reflectedDir := ray.dir - 2 * d * si.norm
		reflectedRay := getRay(movePoint(whereInter, si.norm), reflectedDir)
		reflectedColor := raytraceScene(s, reflectedRay, depth + 1)

		p1 := mat.ior if si.isInside else 1
		p2 := 1 if si.isInside else mat.ior

		sin2 := p1 / p2 * math.sqrt(1 - d * d)
		if abs(sin2) > 1 {
			return s.objects[si.objIndex].emm + reflectedColor
		}

		cos2 := math.sqrt(1 - sin2 * sin2)
		refractedDir := normalize((p1 / p2) * ray.dir - (p1 / p2 * d + cos2) * si.norm)
		refractedRay := getRay(movePoint(whereInter, -si.norm), refractedDir)
		refractedColor := raytraceScene(s, refractedRay, depth + 1)

		if !si.isInside {
			refractedColor *= s.objects[si.objIndex].color
		}
		r0 := (p1 - p2) * (p1 - p2) / ((p1 + p2) * (p1 + p2))
		r := r0 + (1 - r0) * math.pow(1 + d, 5)
		if randF32() < r {
			return s.objects[si.objIndex].emm + reflectedColor
		}
		return s.objects[si.objIndex].emm + refractedColor
	case:
		panic("Not initialized object material")
	}
}

generateImage :: proc(#by_ptr s: Scene) {
	c: Color
	for y in 0 ..< s.h {
		for x in 0 ..< s.w {
			c = {0, 0, 0}
			for _ in 0 ..< s.samples {
				c += raytraceScene(s, generateRay(s, x, y, randF32(), randF32()), 0)
			}
			s.data[s.w * y + x] = ColorToSave(c / f32(s.samples))
		}
	}
}

destroyScene :: proc(s: Scene) {
	delete(s.lights)
	delete(s.objects)
	delete(s.data)
}
