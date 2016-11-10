#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
//#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <string>
#include "ImagePan.h"
#include "RelacionImg.h"

using namespace std;
using namespace cv;

int calcMinMax(vector<vector<Point2f> > objs, vector<Point2f> core,
		vector<float>* sal) {
	float minX = numeric_limits<int>::max();
	float minY = numeric_limits<int>::max();
	float maxX = -numeric_limits<int>::max();
	float maxY = -numeric_limits<int>::max();
	for (int j = 0; j < objs.size(); ++j) {
		vector<Point2f> obj = objs.at(j);
		for (int i = 0; i < 4; ++i) {
			float objX = obj[i].x;
			float objY = obj[i].y;
			float coreX = core[i].x;
			float coreY = core[i].y;
			(objX < minX) ? (minX = objX) : (minX);
			(objX > maxX) ? (maxX = objX) : (maxX);
			(objY < minY) ? (minY = objY) : (minY);
			(objY > maxY) ? (maxY = objY) : (maxY);
			(coreX < minX) ? (minX = coreX) : (minX);
			(coreX > maxX) ? (maxX = coreX) : (maxX);
			(coreY < minY) ? (minY = coreY) : (minY);
			(coreY > maxY) ? (maxY = coreY) : (maxY);
		}
	}

	sal->at(0) = minX;
	sal->at(1) = maxX;
	sal->at(2) = minY;
	sal->at(3) = maxY;
}

int main() {
	vector<RelacionImg> relations;
	vector<ImagePan> imgs;
	/* Reads the images */
	Mat img1 = imread("imagenes/Torre1.jpg", CV_LOAD_IMAGE_COLOR);
	Mat img2 = imread("imagenes/Torre2.jpg", CV_LOAD_IMAGE_COLOR);
	Mat img3 = imread("imagenes/Torre3.jpg", CV_LOAD_IMAGE_COLOR);
	resize(img1, img1, Size(img1.cols, img1.rows));
	resize(img2, img2, Size(img2.cols*0.8, img2.rows*0.8));
	resize(img3, img3, Size(img3.cols, img3.rows));
	imgs.push_back(ImagePan(img1));
	imgs.push_back(ImagePan(img2));
	imgs.push_back(ImagePan(img3));

	/* Detects the keypoints with a feature detector */
	SurfFeatureDetector detector(300);	// Usar NORM_L2 en BFMatcher
	//OrbFeatureDetector detector(100);	// Usar NORM_HAMMING en BFMatcher
	//SiftFeatureDetector detector(300);	// Usar NORM_L2 en BFMatcher
	//vector<vector<KeyPoint> > keyPoints;
	for (int i = 0; i < imgs.size(); ++i) {
		vector<KeyPoint> keypoints;
		detector.detect(imgs[i].getImg(), keypoints);
		imgs[i].setKeyPoints(keypoints);
		//keyPoints.push_back(keypoints);
	}



	/* Extracts the descriptors from the features */
	SurfDescriptorExtractor extractor;
	//OrbDescriptorExtractor extractor;
	//SiftDescriptorExtractor extractor;
	//vector<Mat> descriptors;
	for (int i = 0; i < imgs.size(); ++i) {
		Mat descriptorsImg;
		vector<KeyPoint> keyPoints = imgs[i].getKeyPoints();
		extractor.compute(imgs[i].getImg(), keyPoints, descriptorsImg);
		imgs[i].setDescriptors(descriptorsImg);
		//descriptors.push_back(descriptorsImg);
	}

	/* Finds matches between both images */
	//BFMatcher matcher(NORM_L2);	// Usar NORM correspondiente a detector
	FlannBasedMatcher matcher;	// Mas rapido que fuerza bruta

	for (int i = 0; i < imgs.size(); ++i) {
		for (int j = 0; j < imgs.size(); ++j) {
			vector<vector<DMatch> > matches;
			if (i != j) {
				matcher.knnMatch(imgs[i].getDescriptors(),
						imgs[j].getDescriptors(), matches, 2);
				RelacionImg relation = RelacionImg(imgs[i], imgs[j]);
				relation.setMatches(matches);
				relations.push_back(relation);
			}
		}
	}

	for (int i = 0; i < (int) relations.size(); ++i) {
		vector<vector<DMatch> > matches = relations.at(i).getMatches();
		vector<DMatch> good_matches;
		for (int k = 0; k < (int) matches.size(); ++k) {
			float d1 = matches.at(k).at(0).distance;
			float d2 = matches.at(k).at(1).distance;
			if (d1 < d2 * 0.7) {	// Ratio 2 vecino (ir variando)
				good_matches.push_back(matches.at(k).at(0));
			}
		}
		relations.at(i).setGoodMatches(good_matches);
	}

	for (int i = 0; i < imgs.size(); ++i) {
		std::ostringstream oss;
		oss << "Img " << i;
		imshow(oss.str(), imgs[i].getImg());
	}

	for (int i = 0; i < relations.size(); ++i) {
		Mat img_matches1, img_matches2;
		vector<DMatch> good_matches;
		Mat img_matches;
		drawMatches(relations.at(i).getFirst().getImg(),
				relations.at(i).getFirst().getKeyPoints(),
				relations.at(i).getSecond().getImg(),
				relations.at(i).getSecond().getKeyPoints(),
				relations.at(i).getGoodMatches(), img_matches);
		relations.at(i).setImgMatches(img_matches);
	}

	/* Localize the object */
	vector<Point2f> picFristSecond;
	vector<Point2f> picSecondFirst;
	for (int i = 0; i < (int) relations.size(); ++i) {
		for (int j = 0; j < (int) relations.at(i).getGoodMatches().size();
				j++) {
			/* Get the keypoints from the good matches */
			picFristSecond.push_back(
					relations.at(i).getFirst().getKeyPoints()[relations.at(i).getGoodMatches()[j].queryIdx].pt);
			picSecondFirst.push_back(
					relations.at(i).getSecond().getKeyPoints()[relations.at(i).getGoodMatches()[j].trainIdx].pt);
		}
		if (relations.at(i).getGoodMatches().size() < 4) {
			relations.erase(relations.begin() + i);
		} else {
			relations.at(i).relKeyFirstSecond = picFristSecond;
			relations.at(i).relKeySecondFirst = picSecondFirst;
			Mat H = cv::findHomography(picFristSecond, picSecondFirst , CV_RANSAC);
			vector<Point2f> obj_corners(4);
			vector<Point2f> core_corners(4);
			obj_corners[0] = cvPoint(0, 0);
			obj_corners[1] = cvPoint(relations.at(i).getFirst().getImg().cols, 0);
			obj_corners[2] = cvPoint(relations.at(i).getFirst().getImg().cols, relations.at(i).getFirst().getImg().rows);
			obj_corners[3] = cvPoint(0, relations.at(i).getFirst().getImg().rows);

			core_corners[0] = cvPoint(0, 0);
			core_corners[1] = cvPoint(relations.at(i).getSecond().getImg().cols, 0);
			core_corners[2] = cvPoint(relations.at(i).getSecond().getImg().cols, relations.at(i).getSecond().getImg().rows);
			core_corners[3] = cvPoint(0, relations.at(i).getSecond().getImg().rows);

			vector<Point2f> scene_corners(4);
			perspectiveTransform(obj_corners, scene_corners, H);
				/* Draw lines between the corners (the mapped object in the scene - image_2 ) */
			Mat imgM = relations.at(i).getImgMatches();
			line(imgM, scene_corners[0] + Point2f(img1.cols, 0),
					scene_corners[1] + Point2f(img1.cols, 0), Scalar(0, 255, 0),
					4);
			line(imgM, scene_corners[1] + Point2f(img1.cols, 0),
					scene_corners[2] + Point2f(img1.cols, 0), Scalar(0, 255, 0),
					4);
			line(imgM, scene_corners[2] + Point2f(img1.cols, 0),
					scene_corners[3] + Point2f(img1.cols, 0), Scalar(0, 255, 0),
					4);
			line(imgM, scene_corners[3] + Point2f(img1.cols, 0),
					scene_corners[0] + Point2f(img1.cols, 0), Scalar(0, 255, 0),
					4);
			relations.at(i).setImgMatches(imgM);

			vector<vector<Point2f> > scenes;
			scenes.push_back(scene_corners);
			vector<float> minMax(4);
			calcMinMax(scenes, core_corners, &minMax);
			int width = minMax[1] - minMax[0];
			int height = minMax[3] - minMax[2];

			Mat T = Mat::eye(3, 3, CV_64FC1);
			if (minMax[0] < 0) {
				T.at<double>(0, 2) = -minMax[0];
			}
			if (minMax[2] < 0) {
				T.at<double>(1, 2) = -minMax[2];
			}

			Mat result = Mat(Size(width, height), CV_32F);
			Mat im1 = relations.at(i).getSecond().getImg();
			Mat im2 = relations.at(i).getFirst().getImg();
			warpPerspective(im1, result, T, result.size(), INTER_LINEAR,
					BORDER_TRANSPARENT);
			warpPerspective(im2, result, T * H, result.size(), INTER_LINEAR,
					BORDER_TRANSPARENT);

			std::ostringstream outR;
			outR << "Result " << i;
			resize(result, result, Size(result.cols * 0.8, result.rows * 0.8));
			imshow(outR.str(), result);

			std::ostringstream outM;
			outM << "ImgM " << i;
			imshow(outM.str(), relations.at(i).getImgMatches());
		}
	}
	waitKey(0);
}

