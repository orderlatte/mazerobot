#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <map>

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
	bool findGoalArea(cv::Mat& CameraImage, bool  bDebug=false);
	bool findCrossArea(cv::Mat& CameraImage,bool bDebug=false);
	float FindLineInImageAndComputeOffset(cv::Mat& CameraImage, bool bDebug = false);
	float FindLineInImageAndComputeOffsetAndWidth(cv::Mat& CameraImage, int& selectedWidth, bool bDebug = false);

	void SetParameter(map<string, float> mParam);
private:

	float m_thresBlueAreaOfROI;
	float m_thresCrossAreaOfROI;
	int m_thresBinary;
	int m_lowerBoundBlue;
	int m_upperBoundBlue;
};

