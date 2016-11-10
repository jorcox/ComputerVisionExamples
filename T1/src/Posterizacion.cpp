#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>

using namespace cv;

void posterizacion() {
	VideoCapture cap(0); /* Camara */

	int subdivB, subdivG, subdivR; /** Numero de sub-divisiones */

	/* Inicializacion de los valores */
	std::cout << "* Inserta el numero de sub-divisiones del canal azul: ";
	std::cin >> subdivB;
	std::cout << "* Inserta el numero de sub-divisiones del canal verde: ";
	std::cin >> subdivG;
	std::cout << "* Inserta el numero de sub-divisiones del canal rojo: ";
	std::cin >> subdivR;

	if (cap.isOpened()) {
		while (1) {
			Mat original;
			cap >> original;
			if (original.empty())
				break;

			imshow("Imagen original", original);
			vector<Mat> channels;
			split(original, channels);
			double rangoB = 255 / subdivB;
			double rangoG = 255 / subdivG;
			double rangoR = 255 / subdivR;

			for (int y = 0; y < original.rows; y++) {
				for (int x = 0; x < original.cols; x++) {
					int sectorB = channels[0].at<uchar>(y, x) / rangoB;
					channels[0].at<uchar>(y, x) = (rangoB * (sectorB-1) + rangoB*(sectorB+1))/2;
					int sectorG = channels[1].at<uchar>(y, x) / rangoG;
					channels[1].at<uchar>(y, x) = (rangoG * (sectorG-1) + rangoG*(sectorG+1))/2;
					int sectorR = channels[2].at<uchar>(y, x) / rangoR;
					channels[2].at<uchar>(y, x) = (rangoR * (sectorR-1) + rangoR*(sectorR+1))/2;
				}
			}

			merge(channels, original);
			imshow("Imagen posterizada", original);

			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}
	}
}
