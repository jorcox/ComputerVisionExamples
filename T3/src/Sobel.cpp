#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>
#include <math.h>

using namespace std;
using namespace cv;

void Sobel(Mat img, Mat* gx_draw, Mat* gy_draw, Mat* mag, Mat* ori, int sigma) {
	Mat gx, gy;
	for ( int i = 1; i < sigma; i = i + 2 ) {
		GaussianBlur( img, img, Size( i, i ), 0, 0 );
	}

	Sobel(img, gx, CV_16S, 1, 0, 3);
	Sobel(img, gy, CV_16S, 0, 1, 3);

	for (int i=0; i<img.rows; i++) {
		for (int j=0; j<img.cols; j++) {

			/* Pasa los gradientes de [-255..255] a [0..255] */
			gx_draw->at<uchar>(i,j) = gx.at<short>(i,j) / 2 + 128;
			gy_draw->at<uchar>(i,j) = gy.at<short>(i,j) / 2 + 128;

			/* mod = sqrt(gx^2 + gy^2) */
			mag->at<uchar>(i,j) = sqrt((double) (gx.at<short>(i,j)*gx.at<short>(i,j)
					+ gy.at<short>(i,j)*gy.at<short>(i,j)));

			/* atan2 devuelve [-pi..pi], se pasa a [0..2pi] y luego a [0..255] */
			double theta = atan2(gy.at<short>(i,j), gx.at<short>(i,j));
			theta = theta + M_PI;
			if(theta >=0 && theta < M_PI){
				theta = theta + M_PI;
			} else{
				theta = theta - M_PI;
			}

			theta = theta / M_PI * 128;
			ori->at<uchar>(i,j) = theta;
		}
	}
}
