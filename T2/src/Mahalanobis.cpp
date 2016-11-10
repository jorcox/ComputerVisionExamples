#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>
#include "Figura.h"

using namespace std;
using namespace cv;

double mahalanobis(vector<Point> x, Figura f) {
	/* Calcula los descriptores de imagen */
	double perimeter = arcLength(x, true);

	Moments m = moments(x, true);
	double inv[7];
	HuMoments(m, inv);
	double area = m.m00;
	double inv1 = inv[1];
	double inv2 = inv[2];

	double d = pow((area - f.area_media), 2) / ((double) f.area_varianza);
	d += pow((perimeter - f.perimetro_media), 2)
			/ ((double) f.perimetro_varianza);
	d += pow((inv1 - f.m1_media), 2) / ((double) f.m1_varianza);
	d += pow((inv2 - f.m2_media), 2) / ((double) f.m2_varianza);

	return d;
}
