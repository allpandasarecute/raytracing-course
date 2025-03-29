package main

import "core:fmt"
import "core:math"

LightDot :: struct {
	pos, att: Vec3f,
}

LightDir :: struct {
	dir: Vec3f,
}

LightType :: union {
	LightDot,
	LightDir,
}

Light :: struct {
	type:  LightType,
	color: Color,
}

GetLightIntensity :: struct {
	intensity: f32,
	dist:      f32,
	dir:       Vec3f,
}

GetLight :: struct {
	color: Color,
	dist:  f32,
	dir:   Vec3f,
}

getLightDot :: proc(#by_ptr l: LightDot, pos, norm: Vec3f) -> Maybe(GetLightIntensity) {
	dir := l.pos - pos
	r := length(dir)
	dir = normalize(dir)

	d := dot(norm, dir)
	if (d > 0) {
		return GetLightIntensity{d / (l.att.x + l.att.y * r + l.att.z * r * r), r, dir}
	}

	return nil
}

getLightDir :: proc(#by_ptr l: LightDir, pos, norm: Vec3f) -> Maybe(GetLightIntensity) {
	d := dot(norm, l.dir)
	if (d > 0) {
		return GetLightIntensity{d, math.F32_MAX, l.dir}
	}

	return nil
}

getLight :: proc(#by_ptr l: Light, pos, norm: Vec3f) -> Maybe(GetLight) {
	maybeGL: Maybe(GetLightIntensity)

	switch &light in l.type {
	case LightDir:
		maybeGL = getLightDir(light, pos, norm)
	case LightDot:
		maybeGL = getLightDot(light, pos, norm)
	case:
		panic("Not-initialized light")
	}

	if gl, ok := maybeGL.?; ok {
		return GetLight{l.color * gl.intensity, gl.dist, gl.dir}
	} else {
		return nil
	}
}
