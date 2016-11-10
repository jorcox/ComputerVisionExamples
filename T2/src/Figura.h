#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>

#ifndef FIGURA_H_
#define FIGURA_H_

using namespace std;
using namespace cv;

class Figura {
public:
    int n;
    String nombre;
    float area_media;
    float area_sse;
    float area_varianza;
    float perimetro_media;
    float perimetro_sse;
    float perimetro_varianza;
    float m1_media;
    float m1_sse;
    float m1_varianza;
    float m2_media;
    float m2_sse;
    float m2_varianza;

    Figura(String nom);

    ~Figura();

    void write(FileStorage& fs);

    void read(const FileNode& node);

};

#endif /* FIGURA_H_ */
