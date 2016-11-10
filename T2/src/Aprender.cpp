#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>
#include "Figura.h"

using namespace std;
using namespace cv;

RNG rng(12345);
int morph_size = 1;

/*
 * Actualiza la media y varianza de un descriptor concreto
 */
void updateDescriptor(Figura *f, double descriptor, double oldMean, double oldSSE, double oldVar,
		double* newMean, double* newSSE, double* newVar) {
	*newMean = (((oldMean*f->n)+descriptor))/(f->n+1);
	*newSSE = pow(descriptor, 2) + f->n*(*newMean);
	double alfa = 0.000001;
	double prioriVar = pow(*newMean*alfa,2);
	if(f->n==0){
		*newVar = prioriVar;
	} else {
		double normalVar = (((f->n-1)*oldVar)+((descriptor-oldMean)*(descriptor-(*newMean))))/(f->n);
		*newVar = (prioriVar/(f->n+1))+(((f->n)/(f->n+1))*normalVar);
		//*newVar = normalVar;
	}
}

/*
 * Actualiza los descriptores de un tipo de figura a partir de los descriptores
 * del nuevo elemento.
 */
void updateFeature(Figura* f, double area, double perimeter, double inv1, double inv2) {
	double newMean, newSSE, newVar;
	updateDescriptor(f, area, f->area_media, f->area_sse, f->area_varianza, &newMean, &newSSE, &newVar);
	f->area_media = newMean;
	f->area_sse += newSSE;
	f->area_varianza = newVar;

	updateDescriptor(f, perimeter, f->perimetro_media, f->perimetro_sse, f->perimetro_varianza, &newMean, &newSSE, &newVar);
	f->perimetro_media = newMean;
	f->perimetro_sse += newSSE;
	f->perimetro_varianza = newVar;

	updateDescriptor(f, inv1, f->m1_media, f->m1_sse, f->m1_varianza, &newMean, &newSSE, &newVar);
	f->m1_media = newMean;
	f->m1_sse += newSSE;
	f->m1_varianza = newVar;

	updateDescriptor(f, inv2, f->m2_media, f->m2_sse, f->m2_varianza, &newMean, &newSSE, &newVar);
	f->m2_media = newMean;
	f->m2_sse += newSSE;
	f->m2_varianza = newVar;

	f->n += 1;
}

void aprender(String nomFich, String nomObj) {
	Mat img, adaptive, otsu, canny, blobs;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

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

	/* Carga de la imagen */
	img = imread(nomFich, CV_LOAD_IMAGE_GRAYSCALE);
	adaptive = Mat::zeros(img.size(), CV_8UC1);

	/* Hace umbralizacion por el metodo de Otsu */
	//adaptiveThreshold(img, adaptive, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
	//        THRESH_BINARY_INV, 5, 2);
	threshold(img, otsu, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);
	//Mat kernel = Mat::ones(5,5,CV_8UC1);
	//dilate(otsu,kernel, 1);

	Mat element = getStructuringElement(MORPH_RECT, Size(2*morph_size + 1, 2*morph_size+1),
			Point(morph_size, morph_size));
	morphologyEx(otsu, otsu, MORPH_CLOSE, element);

	findContours(otsu, contours, hierarchy, CV_RETR_EXTERNAL,
			CV_CHAIN_APPROX_NONE);

	/* Pinta los blobs para distinguirlos */
	blobs = Mat::zeros(otsu.size(), CV_8UC3);
	for (int i = 0; i < (int) contours.size(); i++) {
		Scalar color(rng.uniform(0, 255), rng.uniform(0, 255),
				rng.uniform(0, 255));
		drawContours(blobs, contours, i, color, CV_FILLED, 8, hierarchy, 0,
				Point());
	}

	/* Calcula los descriptores de imagen */
	double perimeter = arcLength(contours.at(0), true);

	Moments m = moments(contours[0], true);
	double inv[7];
	HuMoments(m, inv);
	double area = m.m00;
	double inv1 = inv[1];
	double inv2 = inv[2];

	/* Actualizacion de los datos del fichero */
	if (string(nomObj) == "circulo") {
		updateFeature(&circulo, area, perimeter, inv1, inv2);
	} else if (string(nomObj) == "rectangulo"){
		updateFeature(&rectangulo, area, perimeter, inv1, inv2);
	} else if (string(nomObj) == "triangulo"){
		updateFeature(&triangulo, area, perimeter, inv1, inv2);
	} else if (string(nomObj) == "rueda"){
		updateFeature(&rueda, area, perimeter, inv1, inv2);
	} else if (string(nomObj) == "vagon"){
		updateFeature(&vagon, area, perimeter, inv1, inv2);
	}

	/* Escribe los datos actualizados en el fichero */
	FileStorage fs2("objetos.yml", FileStorage::WRITE);
	circulo.write(fs2);
	rectangulo.write(fs2);
	rueda.write(fs2);
	triangulo.write(fs2);
	vagon.write(fs2);
	fs2.release();

	//while (1) {
		//imshow("Original", img);
		//imshow("Adaptive", adaptive);
		//imshow("Contours", otsu);
		//imshow("Blobs", blobs);

		//if (waitKey(1) == 27)
		//  break;
	//}
}
