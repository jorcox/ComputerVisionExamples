#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>
#include "Alien.hpp"

using namespace cv;

int channelSelect[3] = {170, 85, 255}; // Tono para azul, verde, rojo

/*
 * Dada una imagen, modifica el tono de la piel al del color
 * indicado y maximiza la saturacion para obtener ese color puro.
 */
Mat alienize(Mat original, int channel) {
	Mat mask, converted;

	/* Crea una mascara en YCrCb */
	cvtColor(original, mask, CV_BGR2YCrCb);
	inRange(mask, Scalar(Y_MIN, Cr_MIN, Cb_MIN),
			Scalar(Y_MAX, Cr_MAX, Cb_MAX), mask);

	/* Aplica filtro de mediana para eliminar ruido en mascara */
	for (int i=1; i < 10; i+=2) {
			medianBlur(mask, mask, i);
	}
	imshow("Mask", mask);

	/* Convierte la imagen original a HSV */
	cvtColor(original, converted, CV_BGR2HSV_FULL);

	/* Suma una tonalidad a la mascara de la foto original */
	for (int i=0; i<converted.rows; i++) {
		for (int j=0; j<converted.cols; j++) {
			Vec3b color = converted.at<Vec3b>(i,j);

			int colorMask = mask.at<uchar>(i,j);

			/* Si es piel, le pone el tono deseado y satura a tope */
			if (colorMask == 255) {
				color[0] = channelSelect[channel];
				color[1] = 255;
			}
			converted.at<Vec3b>(i,j) = color;
		}
	}

	/* Devuelve la imagen a RGB */
	cvtColor(converted, original, CV_HSV2BGR_FULL);

	return original;
}

/*
 * Detecta la piel humana en la imagen de la camara, y le cambia
 * el tono por azul, verde o rojo segun indique el usuario.
 */
void alien() {
	/* Lee de la camara de forma continua */
	VideoCapture cap(0); // open the default camera

	/* Lee rojo, verde o azul de la entrada */
	String input;
	std::cout<<"Escribe el color del alien (rojo, verde o azul): "; std::cin>>input;

	/* Elige el canal a modificar segun ha indicado el usuario */
	int color;
	if (string(input) == "azul") {
		color = 0;
	} else if (string(input) == "verde"){
		color = 1;
	} else if (string(input) == "rojo") {
		color = 2;
	} else {
		std::cout << "Error: color del alien desconocido.";
		exit(1);
	}

	if (cap.isOpened()) {
		while (1) {
			Mat original, alienized;
			cap >> original;
			if (original.empty()) {
				break;
			}
			/* Convierte la imagen en una de 3 canales */
			original.convertTo(original, CV_8UC3);
			imshow("Imagen original", original);

			/* Cambia la piel encontrada en la imagen */
			alienized = alienize(original, color);
			imshow("Imagen alien", alienized);

			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}
	}
}
