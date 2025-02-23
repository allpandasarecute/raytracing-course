#include "scene.hpp"
#include "types.hpp"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		return 1;
	}
	string input = string(argv[1]);
	string output = string(argv[2]);
	Scene s = Scene(string(argv[1]));
	s.generateImage();
	s.saveImage(string(argv[2]));
	return 0;
}
