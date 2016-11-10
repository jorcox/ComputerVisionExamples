/*
 * RelacionImg.cpp
 *
 *  Created on: 17 de abr. de 2016
 *      Author: Jorge
 */

#include <opencv/highgui.h>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include "RelacionImg.h"
#include "ImagePan.h"

using namespace std;
using namespace cv;



RelacionImg::RelacionImg(ImagePan first, ImagePan second) {
	this->first=first;
	this->second=second;
}

ImagePan RelacionImg::getFirst(){
	return first;
}
ImagePan RelacionImg::getSecond(){
	return second;
}
vector<vector<DMatch> > RelacionImg::getMatches(){
	return matches;
}
void RelacionImg::setMatches(vector<vector<DMatch> > matches){
	this->matches = matches;
}
vector<DMatch> RelacionImg::getGoodMatches(){
	return goodMatches;
}
void RelacionImg::setGoodMatches(vector<DMatch> goodMatches){
	this->goodMatches = goodMatches;
}
Mat RelacionImg::getImgMatches(){
	return imgMatches;
}
void RelacionImg::setImgMatches(Mat imgMatches){
	this->imgMatches = imgMatches;
}

