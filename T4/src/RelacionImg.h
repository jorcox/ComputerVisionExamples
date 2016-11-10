/*
 * RelacionImg.h
 *
 *  Created on: 17 de abr. de 2016
 *      Author: Jorge
 */

#include <opencv/highgui.h>
#include "ImagePan.h"

#ifndef SRC_RELACIONIMG_H_
#define SRC_RELACIONIMG_H_

using namespace std;
using namespace cv;

class RelacionImg {
public:
	RelacionImg(ImagePan, ImagePan);
	ImagePan getFirst();
	ImagePan getSecond();
	vector<vector<DMatch> > getMatches();
	void setMatches(vector<vector<DMatch> >);
	vector<DMatch> getGoodMatches();
	void setGoodMatches(vector<DMatch>);
	Mat getImgMatches();
	void setImgMatches(Mat);
	vector<Point2f> relKeyFirstSecond;
	vector<Point2f> relKeySecondFirst;
private:
	ImagePan first;
	ImagePan second;
	vector<vector<DMatch> > matches;
	vector<DMatch> goodMatches;
	Mat imgMatches;
};

#endif /* SRC_RELACIONIMG_H_ */
