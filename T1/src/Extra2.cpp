#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>

using namespace cv;

int extra2() {

	VideoCapture cap(0); /* Camara */
	/* Tamaño de los circulos */
	int bsize ;
	/* Inicializacion de los valores */
	std::cout << "* Inserta el tamaño de los circulos: ";
	std::cin >> bsize;

	if (cap.isOpened()) {
		while (1) {
			Mat original;
			cap >> original;
			if (original.empty())
				break; // end of video stream

			imshow("Imagen origial", original);
			/* Unsigned 8 bits y tres canales */
			Mat final = Mat::zeros(original.size(), CV_8UC3);
			/* Unsigned 8 bits y un canal */
			Mat cir = Mat::zeros(original.size(), CV_8UC1);

			for (int i = 0; i < original.rows; i += bsize) {
				for (int j = 0; j < original.cols; j += bsize) {
					/* Interseccion del rectangulo concreto con el grande para asegurarnos
					 * de que el rectangulo no sale de la imagen original
					 */
					Rect rect = Rect(j, i, bsize, bsize)
							& Rect(0, 0, original.cols, original.rows);
					/* Cuadro concreto */
					Mat cuadro(final, rect);
					/* Se le asigna la media del color */
					cuadro.setTo(mean(original(rect)));

					/* Se calcula un circulo en la capa superior */
					circle(cir, Point(j + bsize / 2, i + bsize / 2),
							bsize / 2 - 1, CV_RGB(255, 255, 255), -1, CV_AA);
				}
			}

			Mat cir_32f;
			/* Conversion a formato de floats */
			cir.convertTo(cir_32f, CV_32F);
			/*
			 * Normalizacion de la mascara de circulos
			 * Se invierten los circulos para dejar solo el marco.
			 */
			normalize(cir_32f, cir_32f, 0, 1, NORM_MINMAX);

			Mat final_32f;
			/* Conversion a formato de floats */
			final.convertTo(final_32f, CV_32F);

			std::vector<Mat> channels;
			/* Division por canales */
			split(final_32f, channels);
			/* Por cada canal de color */
			for (int i = 0; i < channels.size(); ++i)
				/* Multiplica la imagen original por la mascara */
				channels[i] = channels[i].mul(cir_32f);

			/* Se vuelve a juntar los canales */
			merge(channels, final_32f);
			/* Se pasa a formato de 8bits sin signo */
			final_32f.convertTo(final, CV_8U);

			imshow("Imagen final", final);
			if (waitKey(1) == 27)
				break; /* Se para al presionar ESC */
		}
	}
	return 0;
}
