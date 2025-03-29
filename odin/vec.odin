package main

import "core:math/linalg"

Vec3f :: [3]f32
Vec2u :: [2]u64
Quat :: quaternion128

dot :: linalg.dot
cross :: linalg.cross
normalize :: linalg.normalize
length :: linalg.length
mul :: linalg.mul

rotate :: proc(v: Vec3f, q: Quat) -> Vec3f {
	t := mul(q, mul(quaternion(x = v.x, y = v.y, z = v.z, w = 0), conj(q)))
	return {t.x, t.y, t.z}
}
