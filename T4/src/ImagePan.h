/*
 * ImagePan.h
 *
 *  Created on: 17 de abr. de 2016
 *      Author: Jorge
 */

#include <opencv/highgui.h>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#ifndef SRC_IMAGEPAN_H_
#define SRC_IMAGEPAN_H_


using namespace std;
using namespace cv;

class ImagePan {
public:
	ImagePan(Mat img);
	ImagePan();
	vector<Mat> getRelated();
	void setRelated(vector<Mat>);
	Mat getImg();
	vector<KeyPoint> getKeyPoints();
	void setKeyPoints(vector<KeyPoint>);
	void setDescriptors(Mat);
	Mat getDescriptors();

private:
	Mat img;
	vector<Mat> related;
	vector<KeyPoint> keyPoints;
	Mat descriptors;
};

#endif /* SRC_IMAGEPAN_H_ */
