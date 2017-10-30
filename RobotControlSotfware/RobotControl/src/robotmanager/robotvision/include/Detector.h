#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
//#include <opencv2/nonfree/features2d.hpp>
//#include <opencv2/legacy/legacy.hpp>
#include "opencv2/video/tracking.hpp"

using namespace cv;
using namespace std;

#define FUDGE_BIAS 0.01 // Bias robot to the left or right

class Detector
{
public:
	Detector();
	~Detector();

	bool findRedDot(cv::Mat& CameraImage,bool bDebug=false);
	bool findGoalArea(cv::Mat& CameraImage, const float thresBlueAreaOfROI, bool  bDebug=false);
	bool findCrossArea(cv::Mat& CameraImage, const float thresCrossAreaOfROI, bool bDebug=false);
	float FindLineInImageAndComputeOffset(cv::Mat& CameraImage, bool bDebug = false);
};
