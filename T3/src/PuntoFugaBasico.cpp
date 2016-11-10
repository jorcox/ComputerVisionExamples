#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>
#include <math.h>

using namespace std;
using namespace cv;

void PuntoFugaBasico(Mat img, Mat mag, Mat ori, int* px, int* py, int* v, int umbral) {
	int votos[img.cols];
	for (int i=0; i<img.cols; i++) {
		votos[i] = 0;
	}

	for (int i=0; i<img.rows; i++) {
		for (int j=0; j<img.cols; j++) {
			double th = ((double) ori.at<uchar>(i,j) / 128 * M_PI)-M_PI;
			//cout<<th<<endl;
			/*if (mag.at<uchar>(i,j) > umbral &&
					((th > ((M_PI/2) + ang)) ||	(th < ((M_PI/2) - ang))) &&
					((th > ((3*(M_PI/2)) + ang)) ||	(th < ((3*(M_PI/2)) - ang))) &&
					((th > (((M_PI)) + ang)) ||	(th < (((M_PI)) - ang))) &&
					((th > ((0) + ang)) ||	(th < (6.18)))
					) {*/
			/*if (mag.at<uchar>(i,j) > umbral &&
					((th > ((0) + ang)) ||	(th < (5.5))) &&
					((th > (((M_PI)) + ang)) ||	(th < (((M_PI)) - ang)))) {*/
			if (mag.at<uchar>(i,j) > umbral){
				double x = j - img.cols / 2;
				double y = img.rows / 2 - i;
				//double th = (double) ori.at<uchar>(i,j) / 128 * M_PI;
				double rho = x*cos(th) + y*sin(th);

				int pos = floor(rho / cos(th));
				if (!(pos > 250 || pos < -250)) {
					votos[pos + 250]++;
				}
			}
		}
	}

	int max = -1;
	int iMax = -1;
	for (int i=0; i<img.cols; i++) {
		if (votos[i] > max) {
			max = votos[i];
			iMax = i;
		}
	}

	*px = iMax;
	*py = 256;
	*v = max;
}
