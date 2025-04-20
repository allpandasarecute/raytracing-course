package main

import "core:math"
import "core:os"
import "core:slice"
import "core:strconv"
import "core:strings"

readU64 :: #force_inline proc(n: ^u64, #by_ptr s: string) {
	ok: bool
	n^, ok = strconv.parse_u64_of_base(s, 10)
	assert(ok, "Can't read u64")
}

readF32 :: #force_inline proc(n: ^f32, #by_ptr s: string) {
	ok: bool
	n^, ok = strconv.parse_f32(s)
	assert(ok, "Can't read f32")
}

read3F32 :: #force_inline proc(n: ^Vec3f, s: []string) {
	readF32(&n[0], s[0])
	readF32(&n[1], s[1])
	readF32(&n[2], s[2])
}

read4F32 :: #force_inline proc(n: ^[4]f32, s: []string) {
	readF32(&n[0], s[0])
	readF32(&n[1], s[1])
	readF32(&n[2], s[2])
	readF32(&n[3], s[3])
}

getScene :: proc(#by_ptr file: string) -> Scene {
	s: Scene
	data, ok := os.read_entire_file(file)
	assert(ok, "Can't read scene file")
	defer delete(data)
	samplers: [dynamic]Sampler
	defer delete(samplers)

	newPrimitive := false
	object := Object {
		mat = Diffuse{},
	}
	vBuf: Vec3f
	qBuf: [4]f32
	it := string(data)
	for line in strings.split_lines_iterator(&it) {
		line := strings.trim_space(line)
		if line == "" {
			continue
		}

		temp := strings.split(line, " ")
		defer delete(temp)

		t := slice.filter(temp, proc(str: string) -> bool {return len(str) > 0}) 	// BUG: if add #by_ptr then sometimes crash on last line
		defer delete(t)

		if newPrimitive {
			if t[0] == "EMISSION" {
				read3F32(&object.emm, t[1:])
				continue
			}
			if t[0] == "METALLIC" {
				object.mat = Metallic{}
				continue
			}
			if t[0] == "DIELECTRIC" {
				object.mat = Dielectric {
					ior = 1,
				}
				continue
			}
			if t[0] == "IOR" {
				f: f32
				readF32(&f, t[1])
				object.mat = Dielectric {
					ior = f,
				}
				continue
			}
			if t[0] == "PLANE" {
				read3F32(&vBuf, t[1:])
				object.shape = Plane {
					norm = vBuf,
				}
				continue
			}
			if t[0] == "ELLIPSOID" {
				read3F32(&vBuf, t[1:])
				object.shape = Ellips {
					r   = vBuf,
					rot = quaternion(x = 0, y = 0, z = 0, w = 1),
				}
				continue
			}
			if t[0] == "BOX" {
				read3F32(&vBuf, t[1:])
				object.shape = Box {
					dim = vBuf,
					rot = quaternion(x = 0, y = 0, z = 0, w = 1),
				}
				continue
			}
			if t[0] == "COLOR" {
				read3F32(&object.color, t[1:])
				continue
			}
			if t[0] == "POSITION" {
				read3F32(&object.pos, t[1:])
				continue
			}
			if t[0] == "ROTATION" {
				read4F32(&qBuf, t[1:])
				switch shape in object.shape {
				case Ellips:
					object.shape = Ellips {
						r   = shape.r,
						rot = quaternion(x = qBuf[0], y = qBuf[1], z = qBuf[2], w = qBuf[3]),
					}
				case Box:
					object.shape = Box {
						dim = shape.dim,
						rot = quaternion(x = qBuf[0], y = qBuf[1], z = qBuf[2], w = qBuf[3]),
					}
				case Plane:
					panic("Wrong command for PLANE shape")
				case:
					panic("Wrong command for undefined shape")
				}
				continue
			}
			newPrimitive = false
			append(&s.objects, object)
			if object.emm != {0, 0, 0} && type_of(object.shape) == Box ||
			   type_of(object.shape) == Ellips {
				append(&samplers, LightSampler{object})
			}
			object = Object {
				mat = Diffuse{},
			}
		}
		if t[0] == "NEW_PRIMITIVE" {
			newPrimitive = true
			continue
		}
		if t[0] == "DIMENSIONS" {
			readU64(&s.w, t[1])
			readU64(&s.h, t[2])
			continue
		}
		if t[0] == "RAY_DEPTH" {
			readU64(&s.raydepth, t[1])
			continue
		}
		if t[0] == "SAMPLES" {
			readU64(&s.samples, t[1])
			continue
		}
		if t[0] == "BG_COLOR" {
			read3F32(&s.bg, t[1:])
			continue
		}
		if t[0] == "CAMERA_POSITION" {
			read3F32(&s.cam.pos, t[1:])
			continue
		}
		if t[0] == "CAMERA_RIGHT" {
			read3F32(&s.cam.right, t[1:])
			continue
		}
		if t[0] == "CAMERA_UP" {
			read3F32(&s.cam.up, t[1:])
			continue
		}
		if t[0] == "CAMERA_FORWARD" {
			read3F32(&s.cam.forward, t[1:])
			continue
		}
		if t[0] == "CAMERA_FOV_X" {
			readF32(&s.cam.fovx, t[1])
			s.cam.fovx = math.tan(s.cam.fovx / 2)
			s.cam.fovy = s.cam.fovx * f32(s.h) / f32(s.w)
			continue
		}
	}
	if newPrimitive {
		append(&s.objects, object)
		if object.emm != {0, 0, 0} && type_of(object.shape) == Box ||
		   type_of(object.shape) == Ellips {
			append(&samplers, LightSampler{object})
		}
	}
	resize(&s.data, s.w * s.h)

	if len(samplers) > 0 {
		fin := MixSampler{}
		append(&fin.samplers, MixSampler{make([dynamic]Sampler, len(samplers))}, CosineSampler{})
		s.sampler = fin
	} else {
		s.sampler = CosineSampler{}
	}

	return s
}
