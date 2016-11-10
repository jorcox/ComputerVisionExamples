#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>
using namespace cv;

/*
 * Estos array recogen los valores de K que deciden
 * el valor de distorsion de barril o cojin.
 */
double barril[3] = {0.00005, 0.0001, 0.0005};
double cojin[3] = {-0.000001, -0.0000015, -0.000002};
Rect bounds[3] = {Rect(170, 110, 300, 260),
		Rect(195, 130, 250, 220), Rect(245, 170, 150, 140)};

/*
 * Segun ha indicado el usuario, asigna los valores de K
 * para el eje X y para el eje Y, que decidiran la distorsion
 * de barril o cojin producida.
 */
void setK(String tipo, int nivel, double* kx, double* ky) {
	if (nivel<1 || nivel >3) {
		*kx = 0;
		*ky = 0;
	} else {
		if (tipo == "B") {
			*kx = barril[nivel-1];
			*ky = barril[nivel-1];
		} else if (tipo == "C"){
			*kx = cojin[nivel-1];
			*ky = cojin[nivel-1];
		} else {
			*kx = 0;
			*ky = 0;
		}
	}

}

/*
 * Capta la imagen de la camara y la distorsiona. Hace una distorsion
 * de barril o de cojin cuya intensidad es asignable por el usuario
 * en funcion de valores discretos (1, 2 o 3).
 */
void distorsion() {
	/* El usuario introduce el tipo e intensidad de distorsion */
	String tipo;
	int nivel;
	std::cout<<"Introduce B para distorsion de Barril, o C para Cojin: "; std::cin>>tipo;
	std::cout<<"Introduce el nivel de distorsion (1, 2 o 3): "; std::cin>>nivel;

	VideoCapture cap(0);
	Mat original, nueva, map_x, map_y;
	if (cap.isOpened()) {
		while (1) {
			cap >> original;
			nueva.create(original.size(), original.type());
			map_x.create(original.size(), CV_32F);
			map_y.create(original.size(), CV_32F);

			/* Asigna el centro de la imagen y los K */
			double cx = 320;
			double cy = 240;
			double kx;
			double ky;
			setK(tipo, nivel, &kx, &ky);

			for (int y = 0; y < original.rows; y++) {
				for (int x = 0; x < original.cols; x++) {
					/*
					 * Crea los mapas que asignan a cada posicion de X e Y
					 * su posicion en la imagen distorsionada.
					 */
					float u = cx +(x-cx)*(1+kx*((x-cx)*(x-cx)+(y-cy)*(y-cy)));
					map_x.at<float>(y,x) = u;

					float v = cy +(y-cy)*(1+ky*((x-cx)*(x-cx)+(y-cy)*(y-cy)));
					map_y.at<float>(y,x) = v;
				}
			}

			/* Realiza la transformacion a partir de los mapas creados */
			remap(original, nueva, map_x, map_y, CV_INTER_LINEAR,
					BORDER_CONSTANT, Scalar(0,0,0));

			if (tipo=="B" && nivel>0 && nivel<=3) {
				nueva = nueva(bounds[nivel-1]);
				Size size(640, 480);
				resize(nueva, nueva, size, 0, 0, INTER_LINEAR);
			}

			imshow("Original", original);
			imshow("Nueva", nueva);

			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}
	}
}
