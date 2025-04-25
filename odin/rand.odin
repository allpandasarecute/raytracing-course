package main

import "core:math/rand"

randF32Dir :: #force_inline proc() -> f32 {
	return rand.float32_uniform(-1, 1)
}

randF32 :: #force_inline proc() -> f32 {
	return rand.float32_uniform(0, 1)
}

randF32MinMax :: #force_inline proc(x, y: f32) -> f32 {
	return rand.float32_uniform(x, y)
}

randU64Max :: #force_inline proc(x: u64) -> u64 {
	return rand.uint64() % x
}

randU64 :: #force_inline proc() -> u64 {
	return rand.uint64()
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
