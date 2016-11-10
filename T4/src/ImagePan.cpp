/*
 * ImagePan.cpp
 *
 *  Created on: 17 de abr. de 2016
 *      Author: Jorge
 */

#include <opencv/highgui.h>
#include "ImagePan.h"

using namespace std;
using namespace cv;

ImagePan::ImagePan(Mat img) {
	this->img = img;
}

ImagePan::ImagePan() {
}

vector<Mat> ImagePan::getRelated(){
	return related;
}

void ImagePan::setRelated(vector<Mat> related){
	this->related = related;
}

vector<KeyPoint> ImagePan::getKeyPoints(){
	return keyPoints;
}

void ImagePan::setKeyPoints(vector<KeyPoint> keyPoints){
	this->keyPoints = keyPoints;
}

Mat ImagePan::getDescriptors(){
	return descriptors;
}

void ImagePan::setDescriptors(Mat descriptors){
	this->descriptors=descriptors;
}

Mat ImagePan::getImg(){
	return img;
}

