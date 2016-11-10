#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>
#include <math.h>

using namespace std;
using namespace cv;

extern void Sobel(Mat img, Mat* gx_draw, Mat* gy_draw, Mat* mag, Mat* ori,
		int sigma);
extern void Canny(Mat img, Mat* gx_draw, Mat* gy_draw, Mat* mag, Mat* ori,
		int mas, double sigma);
extern void PuntoFugaBasico(Mat img, Mat mag, Mat ori, int* px, int* py,
		int* v, int umbral);
extern void PuntoFugaAvanzado(Mat* img, Mat* mag, Mat* ori, int* px, int* py,
		int* v, int umbral);

void PuntoFugaLive(int algorithm, int detector, double sigma, int umbral) {
	Mat img, img_bgr, gx, gx_draw, gy, gy_draw, mag, ori, calor;
	int px, py, votos;

	/* Lee de la camara de forma continua */
	VideoCapture cap(0); // open the default camera

	while (1) {
		cap >> img;
		cvtColor(img, img, CV_BGR2GRAY);
		gx_draw = Mat::zeros(img.size(), CV_8U);
		gy_draw = Mat::zeros(img.size(), CV_8U);
		mag = Mat::zeros(img.size(), CV_8U);
		ori = Mat::zeros(img.size(), CV_8U);

		if (detector == 0) {
			Sobel(img, &gx_draw, &gy_draw, &mag, &ori, sigma);
		} else {
			Canny(img, &gx_draw, &gy_draw, &mag, &ori, 5, sigma);
		}

		if (algorithm == 0) {
			PuntoFugaBasico(img, mag, ori, &px, &py, &votos, umbral);
		} else {
			PuntoFugaAvanzado(&img, &mag, &ori, &px, &py, &votos, umbral);
		}

		cvtColor(img, img_bgr, CV_GRAY2BGR);

		line(img_bgr, Point(px - 25, py - 25),
				Point(px + 25, py + 25), Scalar(0, 0, 255), 3);
		line(img_bgr, Point(px - 25, py + 25),
				Point(px + 25, py - 25), Scalar(0, 0, 255), 3);
		line(img_bgr, Point(0, py),
				Point(img.cols, py), Scalar(0, 0, 255));

		imshow("Original", img_bgr);
		imshow("gx", gx_draw);
		imshow("gy", gy_draw);
		imshow("Magnitud", mag);
		imshow("Orientacion", ori);

		if (waitKey(1) == 27)
			break;
	}
}
