package main

import "core:bufio"
import "core:math"
import "core:math/rand"
import "core:os"
import "core:slice"
import "core:strings"
import "core:sync"
import "core:thread"

processTile :: proc(m: ^sync.Mutex, s: ^Scene, q: ^u64) {
	state := rand.create(randU64())
	context.random_generator = rand.default_random_generator(&state)

	i: Vec2u
	for {
		sync.mutex_lock(m)
		i = Vec2u{q^ % s.xTiles, q^ / s.xTiles}
		q^ += 1
		sync.mutex_unlock(m)

		(i.y < s.yTiles) or_break

		c: Color
		for x in i.x * s.tileSize ..< min((i.x + 1) * s.tileSize, s.w) {
			for y in i.y * s.tileSize ..< min((i.y + 1) * s.tileSize, s.h) {
				c = {0, 0, 0}
				for _ in 0 ..< s.samples {
					c += raytraceScene(s^, generateRay(s^, x, y), 0)
				}
				s.data[s.w * y + x] = ColorToSave(c / f32(s.samples))
			}
		}
	}
}

generateImage :: proc(#by_ptr s: Scene) {
	c: Color
	for y in 0 ..< s.h {
		for x in 0 ..< s.w {
			c = {0, 0, 0}
			for _ in 0 ..< s.samples {
				c += raytraceScene(s, generateRay(s, x, y), 0)
			}
			s.data[s.w * y + x] = ColorToSave(c / f32(s.samples))
		}
	}
}

generateImageMultiThread :: proc(s: ^Scene) {
	m: sync.Mutex

	MAX_THREADS := os.processor_core_count()

	threads := make([]^thread.Thread, MAX_THREADS)
	defer delete(threads)

	q: u64 = 0

	for i in 0 ..< len(threads) {
		threads[i] = thread.create_and_start_with_poly_data3(&m, s, &q, processTile)
	}

	thread.join_multiple(..threads[:])

	for t in threads {
		thread.destroy(t)
	}
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
