#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>

using namespace cv;

/*
 * Captura la imagen de la camara y crea otra imagen que
 * imita a un espejo, es decir, capta los movimientos en
 * el eje X igual que en la realidad.
 */
void espejo() {
	VideoCapture cap(0);
	Mat original, nueva, map_x, map_y;

	if (cap.isOpened()) {
		while (1) {
			cap >> original;
			nueva.create(original.size(), original.type());
			map_x.create(original.size(), CV_32F);
			map_y.create(original.size(), CV_32F);

			if (original.empty())
					break; // end of video stream

			imshow("Original", original);

			/* Crea el mapa que invierte la posicion de las x */
			for (int i=0; i<original.rows; i++) {
				for (int j=0; j<original.cols; j++) {
					map_x.at<float>(i,j) = original.cols - j;
					map_y.at<float>(i,j) = i;
				}
			}

			/* Transforma la imagen utilizando los mapas creados */
			remap(original, nueva, map_x, map_y, CV_INTER_LINEAR,
					BORDER_CONSTANT, Scalar(0,0,0));
			imshow("Espejo", nueva);

			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}
	}
}
