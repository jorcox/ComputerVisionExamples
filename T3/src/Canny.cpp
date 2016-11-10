/*
 * Canny.cpp
 *
 *  Created on: 3 de abr. de 2016
 *      Author: Jorge
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>
#include <math.h>

using namespace std;
using namespace cv;

float Gausiana(int i, float sigma) {
	return (1 / (sqrt(2 * M_PI) * sigma))
			* exp((-pow(i, 2)) / (2 * pow(sigma, 2)))*(1/0.39);
}

float DerGausiana(int i, float sigma) {
	return ((-i) / pow(sigma, 2)) * exp((-pow(i, 2)) / (2 * pow(sigma, 2)));
}

void Canny(Mat img, Mat* gx_draw, Mat* gy_draw, Mat* mag, Mat* ori, int mas,
		double sigma) {
	Mat masGasX, masDerX, masGasY, masDerY, gx, gy;

	masGasX = Mat::zeros(1, mas, CV_32F);
	masDerX = Mat::zeros(1, mas, CV_32F);
	masGasY = Mat::zeros(mas, 1, CV_32F);
	masDerY = Mat::zeros(mas, 1, CV_32F);
	gx = Mat::zeros(img.size(), CV_32F);
	gy = Mat::zeros(img.size(), CV_32F);

	//Mat gau = getGaussianKernel(5, -1, CV_32F);

	for (int i = 0; i < mas; i++) {
		/*
		 * Creación de la máscaras de Canny
		 */

		int div = -mas/2;
		int index = div + i;
		masGasX.at<float>(0, i) = Gausiana(index, sigma);
		//cout << masGasX.at<float>(0, i)<<endl;
		masDerX.at<float>(0, i) = DerGausiana(index, sigma);
		//cout << masDerX.at<float>(0, i)<<endl;
		masGasY.at<float>(i, 0) = Gausiana(index, sigma);
		//cout << masGasY.at<float>(i, 0)<<endl;
		masDerY.at<float>(i, 0) = DerGausiana(index, sigma);
		//cout << masDerY.at<float>(i, 0)<<endl;
	}

	//masGasX.at<float>(0, 0) = 0.1353 ; masGasX.at<float>(0, 1) = 0.6065; masGasX.at<float>(0, 2) = 1; masGasX.at<float>(0, 3) = 0.6065; masGasX.at<float>(0, 4) = 0.1353;
	//masDerX.at<float>(0, 0) = -0.2707; masDerX.at<float>(0, 1) = -0.6065; masDerX.at<float>(0, 2) = 0; masDerX.at<float>(0, 3) = 0.6065; masDerX.at<float>(0, 4) = 0.2707;
	//masGasY.at<float>(0, 0) = 0.1353; masGasY.at<float>(1, 0) = 0.6065; masGasY.at<float>(2, 0) = 1; masGasY.at<float>(3, 0) = 0.6065; masGasY.at<float>(4, 0) = 0.1353;
	//masDerY.at<float>(0, 0) = 0.2707; masDerY.at<float>(1, 0) = 0.6065; masDerY.at<float>(2, 0) = 0; masDerY.at<float>(3, 0) = -0.6065; masDerY.at<float>(0, 0) = -0.2707;

	//sepFilter2D(img, gx, CV_32F, gau.t(), gau);
	//sepFilter2D(img, gy, CV_32F, gau.t(), gau);
	sepFilter2D(img, gx, CV_16S, -masDerX, masGasY);
	sepFilter2D(img, gy, CV_16S, masGasY.t(), masDerX.t());

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {

			/* Pasa los gradientes de [-255..255] a [0..255] */
			gx_draw->at < uchar > (i, j) = gx.at<short>(i, j) / 2 + 128;
			gy_draw->at < uchar > (i, j) = gy.at<short>(i, j) / 2 + 128;

			/* mod = sqrt(gx^2 + gy^2) */
			mag->at<uchar>(i, j) = sqrt(
					(double) (gx.at<short>(i, j) * gx.at<short>(i, j)
							+ gy.at<short>(i, j) * gy.at<short>(i, j)));

			/* theta = atan2(gy, gx) */
			double theta = atan2(gy.at<short>(i, j), gx.at<short>(i, j));
			//theta = fmod(theta, 2 * M_PI);

			theta = theta + M_PI;
			if(theta >=0 && theta < M_PI){
				theta = theta + M_PI;
			} else{
				theta = theta - M_PI;
			}

			theta = theta / M_PI * 128;
			//cout<<theta<<endl;
			ori->at<uchar>(i, j) = theta;
		}
	}

}

