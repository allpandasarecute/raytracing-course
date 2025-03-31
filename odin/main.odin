package main

import "core:math"
import "core:os"

main :: proc() {
	if (len(os.args) != 3) {
		os.exit(1)
	}
	defer delete(os.args)

	s := getScene(os.args[1])
	generateImage(s)
	saveImage(s, os.args[2])
	destroyScene(s)
}
