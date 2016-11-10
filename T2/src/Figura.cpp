#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>
#include "Figura.h"

using namespace std;
using namespace cv;

Figura::Figura(String nom) {
	nombre = nom;
	n = 0;
	area_media = 0;
    area_sse = 0;
	area_varianza = 0;
	perimetro_media = 0;
	perimetro_sse = 0;
	perimetro_varianza = 0;
	m1_media = 0;
	m1_sse = 0;
	m1_varianza = 0;
	m2_media = 0;
	m2_sse = 0;
	m2_varianza = 0;
}

Figura::~Figura() {
	/*
	delete[] &n;
	delete[] &area_media;
	delete[] &area_sse;
	delete[] &area_varianza;
	delete[] &perimetro_media;
	delete[] &perimetro_sse;
	delete[] &perimetro_varianza;
	delete[] &m1_media;
	delete[] &m1_sse;
	delete[] &m1_varianza;
	delete[] &m2_media;
	delete[] &m2_sse;
	delete[] &m2_varianza;
	*/
}

void Figura::write(FileStorage& fs) {
	fs << nombre + "_n" << n << nombre + "_area_media" << area_media
			<< nombre + "_area_sse" << area_sse
			<< nombre + "_area_varianza" << area_varianza
			<< nombre + "_perimetro_media" << perimetro_media
			<< nombre + "_perimetro_sse" << perimetro_sse
			<< nombre + "_perimetro_varianza" << perimetro_varianza
			<< nombre + "_m1_media" << m1_media
			<< nombre + "_m1_sse" << m1_sse
			<< nombre + "_m1_varianza" << m1_varianza
			<< nombre + "_m2_media" << m2_media
			<< nombre + "_m2_sse" << m2_sse
			<< nombre + "_m2_varianza"<< m2_varianza;
}

void Figura::read(const FileNode& node) {
	n = (int) node[nombre + "_n"];
	area_media = (float) node[nombre + "_area_media"];
	area_sse = (float) node[nombre + "_area_sse"];
	area_varianza = (float) node[nombre + "_area_varianza"];
	perimetro_media = (float) node[nombre + "_perimetro_media"];
	perimetro_sse = (float) node[nombre + "_perimetro_sse"];
	perimetro_varianza = (float) node[nombre + "_perimetro_varianza"];
	m1_media = (float) node[nombre + "_m1_media"];
	m1_sse = (float) node[nombre + "_m1_sse"];
	m1_varianza = (float) node[nombre + "_m1_varianza"];
	m2_media = (float) node[nombre + "_m2_media"];
	m2_sse = (float) node[nombre + "_m2_sse"];
	m2_varianza = (float) node[nombre + "_m2_varianza"];
}
