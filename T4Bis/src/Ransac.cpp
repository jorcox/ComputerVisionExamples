#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>
#include <unistd.h>
#include <math.h>

using namespace std;
using namespace cv;

Mat Ransac(vector<Point2f> picCoreAdd, vector<Point2f> picAddCore, double eps, bool adaptive) {
	Mat H;
	srand(time(NULL));

	vector<Point2f> random_choice_core;
	vector<Point2f> random_choice_add;
	vector<Point2f> consensus_core;
	vector<Point2f> consensus_add;
	vector<Point2f> max_consensus_core;
	vector<Point2f> max_consensus_add;
	int umbral_consenso = 50;
	int conjunto_consenso = 0;
	int max_consenso = 0;
	int i = 0;

	int num_intentos;
	double p = 0.5;
	double P = 0.99;
	int k = 4;
	num_intentos = log(1-P) / log(1-pow(p,k));
	cout << "Intentos: " << num_intentos << endl;

	while (conjunto_consenso < umbral_consenso && i < num_intentos) {
		random_choice_core.clear();
		random_choice_add.clear();
		consensus_core.clear();
		consensus_add.clear();
		conjunto_consenso = 0;

		/* Elige 4 matchings al azar */
		for (int j=0; j<4; j++) {
			int index = rand() % picCoreAdd.size();
			random_choice_core.push_back(picCoreAdd.at(index));
			random_choice_add.push_back(picAddCore.at(index));
		}

		/* Calcula el modelo con esos 4 puntos */
		H = findHomography(random_choice_core, random_choice_add, 0);

		/* Aplica ese modelo a todos los puntos */
		vector<Point2f> result(picCoreAdd.size());
		perspectiveTransform(picCoreAdd, result, H);

		/* Calcula cuantos matchings son compatibles con este modelo */
		for (int j=0; j<(int)picAddCore.size(); j++) {
			Point2f real = picAddCore.at(j);
			Point2f obtained = result.at(j);

			/* Calcula distancia entre el matching real y el obtenido */
			double dist = sqrt(pow(real.x - obtained.x, 2) + (pow(real.y - obtained.y, 2)));
			if (dist < eps) {
				conjunto_consenso++;
				consensus_core.push_back(picCoreAdd.at(j));
				consensus_add.push_back(picAddCore.at(j));
			}
		}

		if (conjunto_consenso > max_consenso) {
			max_consensus_core = vector<Point2f>(consensus_core);
			max_consensus_add = vector<Point2f>(consensus_add);
			max_consenso = conjunto_consenso;
		}

		if (adaptive) {
			p = conjunto_consenso / (double) picCoreAdd.size();
			if (p > 0) {
				num_intentos = log(1-P) / log(1-pow(p,k));
				cout << "Intentos: " << num_intentos << endl;
			}
		}

		i++;
	}
	cout << "Consenso: " << max_consenso << endl;
	cout << endl;

	/* Calcula la homografia sobre el conjunto consenso */
	H = findHomography(max_consensus_core, max_consensus_add, 0);
	//max_consensus_core.clear();
	//max_consensus_add.clear();
	return H;

}
