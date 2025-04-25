package main

import "core:math"

DELTA :: 0.0001

movePoint :: #force_inline proc(pos, dir: Vec3f) -> Vec3f {
	return pos + dir * DELTA
}
