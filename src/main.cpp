#include "scene.hpp"
#include "types.hpp"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		return 1;
	}

	Scene s = Scene(argv[1]);
	s.generateImage();
	s.saveImage(argv[2]);

	return 0;
}
