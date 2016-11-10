/*
 * T4Bis.cpp
 *
 *  Created on: 17 de abr. de 2016
 *      Author: Jorge
 */
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

extern Mat Ransac(vector<Point2f> picCoreAdd, vector<Point2f> picAddCore,
		double eps, bool adaptive);

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

int main(int argc, char *argv[]) {
	vector<Mat> imgsBN;
	vector<Mat> imgsCL;
	int mode = 0;				// 1 Plano 2 Curvo
	int nImagenes = 0;
	int imagenCentral = 0;
	Mat coreCL;					// Firts core image
	int detectorExtractor = 0;
	// 1 Orb 2 Surf 3 Sift
	int matcher = 0;           	// 1 Bruta 2 FLANN
	int ransac = 0;				// 1 Casero 2 NoCasero

	if (string(argv[2]) == "plano") {
		mode = 1;
	} else if (string(argv[2]) == "curvo") {
		mode = 2;
	} else {
		cout << "Modo no reconocido" << endl;
	}

	if (string(argv[1]) == "disco") {
		float factorReescala = atof(argv[4]);
		detectorExtractor = atoi(argv[5]);
		matcher = atoi(argv[6]);
		ransac = atoi(argv[7]);
		for (int i = 8; i < argc; i++) {
			Mat img = imread(string(argv[i]), CV_LOAD_IMAGE_COLOR);
			resize(img, img,
					Size(img.cols * factorReescala, img.rows * factorReescala));
			imgsCL.push_back(img);
			Mat imgBN;
			cvtColor(img, imgBN, CV_RGB2GRAY);
			imgsBN.push_back(imgBN);
		}
		if (mode == 1) {
			nImagenes = atoi(argv[3]);
			coreCL = imgsCL.at(0);
			imgsCL.erase(imgsCL.begin());
			imgsBN.erase(imgsBN.begin());
		} else if (mode == 2) {
			imagenCentral = atoi(argv[3]);
			coreCL = imgsCL.at(imagenCentral - 1);
			imgsCL.erase(imgsCL.begin() + (imagenCentral - 1));
			imgsBN.erase(imgsBN.begin() + (imagenCentral - 1));
		}

	} else if (string(argv[1]) == "live") {
		int imagenesLive = atoi(argv[3]);
		int segundos = atoi(argv[4]) * 1000;
		imagenCentral = atoi(argv[5]);
		detectorExtractor = atoi(argv[6]);
		matcher = atoi(argv[7]);
		ransac = atoi(argv[8]);
		int imagesTaken = 0;
		while (imagesTaken < imagenesLive) {
			Mat img;
			cout << "Waiting for image, press SPACE" << endl;
			VideoCapture cap(0); // open the default camera
			cap >> img;
			imshow("Capturing", img);
			int keyP = waitKey(segundos);
			imgsCL.push_back(img);
			Mat imgG;
			cvtColor(img, imgG, CV_RGB2GRAY);
			imgsBN.push_back(imgG);
			cout << "Image taken." << endl;
			imagesTaken++;
		}
		if (mode == 1) {
			coreCL = imgsCL.at(0);
			imgsCL.erase(imgsCL.begin());
			imgsBN.erase(imgsBN.begin());
		} else if (mode == 2) {
			coreCL = imgsCL.at(imagenCentral - 1);
			imgsCL.erase(imgsCL.begin() + (imagenCentral - 1));
			imgsBN.erase(imgsBN.begin() + (imagenCentral - 1));
		}
	}

	for (int ind = 0; ind < imgsBN.size(); ++ind) {
		/* Pre-process */
		Mat core;
		Mat add = imgsBN.at(ind);
		Mat addCL = imgsCL.at(ind);
		cvtColor(coreCL, core, CV_RGB2GRAY);

		/* Pre Time */
		double pre = (double)getTickCount();

		/*
		 * Detects the keypoints with a feature detector and
		 * Extracts the descriptors from the features
		 */
		vector<KeyPoint> keypointsCore, keypointsAdd;
		Mat descriptorsCore, descriptorsAdd;
		if (detectorExtractor == 1) {
			cout<<"Using Orb"<<endl;
			OrbFeatureDetector detector(2000);// Usar NORM_HAMMING en BFMatcher
			detector.detect(core, keypointsCore);
			detector.detect(add, keypointsAdd);
			OrbDescriptorExtractor extractor;
			extractor.compute(core, keypointsCore, descriptorsCore);
			extractor.compute(add, keypointsAdd, descriptorsAdd);
		} else if (detectorExtractor == 2) {
			cout<<"Using Surf"<<endl;
			SurfFeatureDetector detector(200);	// Usar NORM_L2 en BFMatcher
			detector.detect(core, keypointsCore);
			detector.detect(add, keypointsAdd);
			SurfDescriptorExtractor extractor;
			extractor.compute(core, keypointsCore, descriptorsCore);
			extractor.compute(add, keypointsAdd, descriptorsAdd);
		} else {
			cout<<"Using Sift"<<endl;
			SiftFeatureDetector detector(400);	// Usar NORM_L2 en BFMatcher
			detector.detect(core, keypointsCore);
			detector.detect(add, keypointsAdd);
			SiftDescriptorExtractor extractor;
			extractor.compute(core, keypointsCore, descriptorsCore);
			extractor.compute(add, keypointsAdd, descriptorsAdd);
		}



		/* Finds matches between both images */
		vector<vector<DMatch> > matches;
		if(matcher == 1){
			cout<<"Using BFMatcher"<<endl;
			if(detectorExtractor == 1){
				BFMatcher matcher(NORM_HAMMING);
				matcher.knnMatch(descriptorsAdd, descriptorsCore, matches, 2);
			}else{
				BFMatcher matcher(NORM_L2);
				matcher.knnMatch(descriptorsAdd, descriptorsCore, matches, 2);
			}
		} else{
			cout<<"Using FLANN"<<endl;
			FlannBasedMatcher matcher;	// Mas rapido que fuerza bruta
			matcher.knnMatch(descriptorsAdd, descriptorsCore, matches, 2);
		}

		/* Only good matches are preserved (2nd neighbor ratio) */
		vector<DMatch> good_matches;
		for (int i = 0; i < (int) matches.size(); i++) {
			float d1 = matches.at(i).at(0).distance;
			float d2 = matches.at(i).at(1).distance;
			if (d1 < d2 * 0.7) {	// Ratio 2 vecino (ir variando)
				good_matches.push_back(matches.at(i).at(0));
			}
		}

		Mat img_matches;
		drawMatches(add, keypointsAdd, core, keypointsCore, good_matches,
				img_matches);

		/* Localize the object */
		vector<Point2f> picCoreAdd;
		vector<Point2f> picAddCore;
		vector<Point2f> pic23;
		vector<Point2f> pic32;
		for (int i = 0; i < (int) good_matches.size(); i++) {
			/* Get the keypoints from the good matches */
			picCoreAdd.push_back(keypointsAdd[good_matches[i].queryIdx].pt);
			picAddCore.push_back(keypointsCore[good_matches[i].trainIdx].pt);
		}

		if (good_matches.size() >= 4) {
			Mat H;
			if(ransac == 1){
				cout<<"Using OpenCv RANSAC"<<endl;
				H = findHomography(picCoreAdd, picAddCore, CV_RANSAC);
			} else if(ransac == 2){
				cout<<"Using Homemade RANSAC"<<endl;
				H = Ransac(picCoreAdd, picAddCore, 2, false);
			} else {
				cout<<"Using Homemade RANSAC Adaptative"<<endl;
				H = Ransac(picCoreAdd, picAddCore, 2, true);

			}

			/* Get the corners from the image_1 ( the object to be "detected" ) */
			vector<Point2f> obj_corners12(4);
			vector<Point2f> core_corners(4);
			obj_corners12[0] = cvPoint(0, 0);
			obj_corners12[1] = cvPoint(add.cols, 0);
			obj_corners12[2] = cvPoint(add.cols, add.rows);
			obj_corners12[3] = cvPoint(0, add.rows);

			core_corners[0] = cvPoint(0, 0);
			core_corners[1] = cvPoint(core.cols, 0);
			core_corners[2] = cvPoint(core.cols, core.rows);
			core_corners[3] = cvPoint(0, core.rows);

			vector<Point2f> scene_corners12(4);
			perspectiveTransform(obj_corners12, scene_corners12, H);

			/* Draw lines between the corners (the mapped object in the scene - image_2 ) */
			line(img_matches, scene_corners12[0] + Point2f(add.cols, 0),
					scene_corners12[1] + Point2f(add.cols, 0),
					Scalar(0, 255, 0), 4);
			line(img_matches, scene_corners12[1] + Point2f(add.cols, 0),
					scene_corners12[2] + Point2f(add.cols, 0),
					Scalar(0, 255, 0), 4);
			line(img_matches, scene_corners12[2] + Point2f(add.cols, 0),
					scene_corners12[3] + Point2f(add.cols, 0),
					Scalar(0, 255, 0), 4);
			line(img_matches, scene_corners12[3] + Point2f(add.cols, 0),
					scene_corners12[0] + Point2f(add.cols, 0),
					Scalar(0, 255, 0), 4);

			vector<vector<Point2f> > scenes; /* Numero random xD en dinámico habra que hacerlo dinámico */
			scenes.push_back(scene_corners12);
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

			/* Use the Homography Matrix to warp the images */
			Mat result;
			result = Mat(Size(width, height), CV_32F);
			warpPerspective(coreCL, result, T, result.size(), INTER_LINEAR,
					BORDER_TRANSPARENT);
			warpPerspective(addCL, result, T * H, result.size(), INTER_LINEAR,
					BORDER_TRANSPARENT);
			//warpPerspective(img2,result,T,result.size(), INTER_LINEAR, BORDER_TRANSPARENT);
			//Mat half(result,cv::Rect(0,0,img2.cols,img2.rows));
			//warpPerspective(img2,result2,H1,Size(img2.cols,img2.rows));
			//result2.copyTo(half);

			Mat coreShow, addShow;
			resize(core, coreShow, Size(core.cols * 0.5, core.rows * 0.5));
			resize(add, addShow, Size(add.cols * 0.5, add.rows * 0.5));
			imshow("Core", coreShow);
			imshow("Add", addShow);

			Mat show = result;
			resize(show, show, Size(show.cols * 0.5, show.rows * 0.5));
			imshow("Result", show);
			coreCL = result;

			/* Show detected matches */
			Mat matchesShow;
			resize(img_matches, matchesShow,
					Size(img_matches.cols * 0.5, img_matches.rows * 0.5));
			imshow("Good Matches & Object detection 1->2", matchesShow);

			/* Post Time */
			pre = ((double)getTickCount() - pre)/getTickFrequency();
			cout<<pre<<" seconds"<<endl;

			waitKey(0);
			pre = 0;
		} else {
			cout << "Not enought good matches" << endl;
		}
	}

}

