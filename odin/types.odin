package main

Color :: [3]f32
ColorSave :: [3]u8

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
	amb:      Color,
	lights:   [dynamic]Light,
	objects:  [dynamic]Object,
}
