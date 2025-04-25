package main

import "core:math"

Color :: [3]f32
ColorSave :: [3]u8

tonemap :: proc(#by_ptr col: Color) -> Color {
	a: f32 = 2.51
	b: f32 = 0.03
	c: f32 = 2.43
	d: f32 = 0.59
	e: f32 = 0.14
	return (col * (a * col + b)) / (col * (c * col + d) + e)
}

colorToBytes :: #force_inline proc(c: f32) -> u8 {
	return u8(math.round(clamp(math.pow(c, 1 / 2.2), 0, 1) * 255))
}

ColorToSave :: proc(c: Color) -> ColorSave {
	c := c
	c = tonemap(c)
	return ColorSave{colorToBytes(c.r), colorToBytes(c.g), colorToBytes(c.b)}
}
