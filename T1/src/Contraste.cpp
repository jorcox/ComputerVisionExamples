#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>

using namespace cv;

void contraste() {
	VideoCapture cap(0); /* Camara */

	double alfa; /* Contraste */
	int beta; /* Brillo */

	/*
	 * Lectura de valores alfa y beta
	 */
	std::cout << "* Inserta el valor de alfa : ";
	std::cin >> alfa;
	std::cout << "* Inserta el valor de beta : ";
	std::cin >> beta;

	if (cap.isOpened()) {
		while (1) {
			Mat original, equalized, ycrcb_original, ycrcb_final;
			Mat contr = Mat::zeros(original.size(), original.type());
			cap >> original;

			if (original.empty())
				break;

			imshow("Imagen original", original);

			original.convertTo(contr, -1, alfa, beta);
			imshow("Ajuste de brillo y contraste", contr);

			cvtColor(original, ycrcb_original, CV_BGR2YCrCb);
			vector<Mat> channels;
			split(ycrcb_original, channels);
			equalizeHist(channels[0], channels[0]);
			merge(channels, ycrcb_final);
			cvtColor(ycrcb_final, equalized, CV_YCrCb2BGR);
			imshow("Histograma equalizado", equalized);

			if (waitKey(1) == 27)
				break; /* Acaba cuando se puls ESC */
		}
	}
}
