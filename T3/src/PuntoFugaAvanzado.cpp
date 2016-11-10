#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>
#include <math.h>

using namespace std;
using namespace cv;

bool valid(double th) {
	double ang = 0.2;
	return ((th > ((M_PI/2) + ang)) ||	(th < ((M_PI/2) - ang))) &&
			((th > ((3*(M_PI/2)) + ang)) ||	(th < ((3*(M_PI/2)) - ang))) &&
			((th > (((M_PI)) + ang)) ||	(th < (((M_PI)) - ang))) &&
			((th > ((0) + ang)) ||	(th < (6.18)));
}

void PuntoFugaAvanzado(Mat* img, Mat* mag, Mat* ori, int* px, int* py, int* v, int umbral) {
	/* Initializes votes matrix */
	int votos[img->rows][img->cols];
	for (int i=0; i<img->rows; i++) {
		for (int j=0; j<img->cols; j++) {
			votos[i][j] = 0;
		}
	}

	for (int i=0; i<img->rows; i++) {
		for (int j=0; j<img->cols; j++) {
			double th = ((double) ori->at<uchar>(i,j) / 128 * M_PI)-M_PI;

			if (mag->at<uchar>(i,j) > umbral && valid(th)){
				for (int k=0; k<img->rows; k++) {
					double x = j - img->cols / 2;
					double y = img->rows / 2 - i;
					double rho = x*cos(th) + y*sin(th);

					int posY = 	(img->rows/2 - k);
					int posX = floor((rho - posY * sin(th)) / cos(th));
					if (!(posX > img->cols/2 || posX < -img->cols/2 || posY > img->rows/2 || posY < -img->rows/2)) {
						(votos[img->rows/2 - posY][posX + img->cols/2])++;
					}
				}

			}
		}
	}

	int max = -1;
	int iMax = -1;
	int jMax = -1;
	for (int i=0; i<img->rows; i++) {
		for (int j=0; j<img->cols; j++) {
			if (votos[i][j] > max) {
				max = votos[i][j];
				iMax = i;
				jMax = j;
			}
		}
		cout << endl;
	}
	/*
	int maxRow = -1;
	for (int i=0; i<img->rows; i++) {
		int sumRow = 0;
		for (int j=0; j<img->cols; j++) {
			sumRow += votos[i][j];
		}

		if (sumRow > maxRow) {
			maxRow = sumRow;
			iMax = i;
		}
	}

	for (int j=0; j<img->cols; j++) {
		if (votos[iMax][j] > max) {
			max = votos[iMax][j];
			jMax = j;
		}
	}*/

	*px = jMax;
	*py = iMax;
	*v = max;
}
