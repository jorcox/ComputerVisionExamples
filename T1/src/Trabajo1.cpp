#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>

extern void contraste();
extern void alien();
extern void posterizacion();
extern void extra2();
extern void distorsion();
extern void espejo();

using namespace cv;

int main(int argc, char *argv[]) {
	if (argc > 1) {
		if (string(argv[1]) == "contraste") {
			contraste();
		} else if (string(argv[1]) == "alien") {
			alien();
		} else if (string(argv[1]) == "poster") {
			posterizacion();
		} else if (string(argv[1]) == "distorsion") {
			distorsion();
		} else if (string(argv[1]) == "extra2") {
			extra2();
		} else if (string(argv[1]) == "espejo") {
			espejo();
		} else {
			printf("Error: entrada desconocida.");
		}
	} else {
		printf("Error: introduzca un argumento.");
	}
	return 0;
}
