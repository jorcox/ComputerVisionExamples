#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>
#include "Figura.h"

using namespace std;
using namespace cv;

extern double mahalanobis(vector<Point> x, Figura f);
extern int morph_size;
extern RNG rng;

int chi_4 = 9.488;

void reconocer(String nomFich) {
	Mat img, otsu, morph, blobs;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/* Carga de la imagen de reconocimiento */
	img = imread(nomFich, CV_LOAD_IMAGE_GRAYSCALE);

	/* Hace umbralizacion por el metodo de Otsu */
	threshold(img, otsu, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);

	Mat element = getStructuringElement(MORPH_RECT, Size(2*morph_size + 1, 2*morph_size+1),
				Point(morph_size, morph_size));
	morphologyEx(otsu, morph, MORPH_CLOSE, element);

	findContours(morph, contours, hierarchy, CV_RETR_EXTERNAL,
			CV_CHAIN_APPROX_NONE);

	for (int i=0; i<(int)contours.size(); i++) {
		if (contourArea(contours.at(i))<5) {
			contours.erase(contours.begin() + i);
		}
	}

	blobs = Mat::zeros(morph.size(), CV_8UC3);
	for (int i = 0; i < (int) contours.size(); i++) {
		Scalar color(rng.uniform(0, 255), rng.uniform(0, 255),
				rng.uniform(0, 255));
		drawContours(blobs, contours, i, color, CV_FILLED, 8, hierarchy, 0,
				Point());
		cout<<"Figura "<<i<<": "<<color(0)<<" "<<color(1)<<" "<<color(2)<<endl;
	}
	cout<<endl;

	/* Lectura de los objetos almacenados en el fichero */
	FileStorage fs("objetos.yml", FileStorage::READ);
	FileNode n = fs["strings"];

	Figura circulo = Figura("circulo");
	circulo.read(n);

	Figura rectangulo = Figura("rectangulo");
	rectangulo.read(n);

	Figura rueda = Figura("rueda");
	rueda.read(n);

	Figura triangulo = Figura("triangulo");
	triangulo.read(n);

	Figura vagon = Figura("vagon");
	vagon.read(n);

	fs.release();
	Figura clases[] = {circulo, rectangulo, rueda, triangulo, vagon};

	cout<<contours.size()<<" contornos detectados"<<endl;
	/* Reconoce cada figura de la imagen */
	for (int i=0; i<(int)contours.size(); i++) {
		vector<string> resultados;

		/* Compara la figura con cada clase (circulo, rueda, etc) */
		for (int j=0; j<5; j++) {
			Figura f = clases[j];

			/* Si pasa el test de mahalanobis, pertenece a esa clase */
			double m = mahalanobis(contours.at(i), f);
			cout<<m<<endl;
			if (m < chi_4) {
				/* Insercion al final del vector con push_back */
				resultados.push_back(f.nombre);
			}
		}

		cout<<"Figura "<<i<<endl;
		for (int j=0; j<(int)resultados.size(); j++) {
			cout<<resultados.at(j)<<endl;
		}
		cout<<endl;

	}
	while (1) {
		imshow("Original", img);
		imshow("Otsu", otsu);
		imshow("Contours", morph);
		imshow("Blobs", blobs);

		if (waitKey(1) == 27)
			  break;
	}

}
