package main

import "core:bufio"
import "core:fmt"
import "core:io"
import "core:math"
import "core:os"
import "core:slice"
import "core:strconv"
import "core:strings"

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

generateRay :: proc(#by_ptr s: Scene, x: u64, y: u64) -> Ray {
	newX := (2 * (f32(x) + 0.5) / f32(s.w) - 1) * s.cam.fovx
	newY := -(2 * (f32(y) + 0.5) / f32(s.h) - 1) * s.cam.fovy
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
		ansColor := s.amb
		for &l in s.lights {
			if light, ok := getLight(l, whereInter, si.norm).?;
			   ok &&
			   intersectScene(s, getRay(movePoint(whereInter, si.norm), light.dir), light.dist) ==
				   nil {
				ansColor += light.color
			}
		}
		return ansColor * s.objects[si.objIndex].color
	case Metallic:
		return(
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
			return reflectedColor
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
		return r * reflectedColor + (1 - r) * refractedColor
	case:
		panic("Not initialized object material")
	}
}

generateImage :: proc(#by_ptr s: Scene) {
	for y in 0 ..< s.h {
		for x in 0 ..< s.w {
			s.data[s.w * y + x] = ColorToSave(raytraceScene(s, generateRay(s, x, y), 0))
		}
	}
}

destroyScene :: proc(s: Scene) {
	delete(s.lights)
	delete(s.objects)
	delete(s.data)
}
