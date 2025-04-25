package main

import "core:math/linalg"

Vec3f :: [3]f32
Vec2u :: [2]u64
Quat :: quaternion128

dot :: linalg.dot
cross :: linalg.cross
normalize :: linalg.normalize
length :: linalg.length
length2 :: linalg.length2
mul :: linalg.mul
inverse :: linalg.inverse

rotate :: linalg.quaternion_mul_vector3
refract :: linalg.refract
reflect :: linalg.reflect
