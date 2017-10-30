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
//#include "ImageProcessing.h"

#include "Recognizer.h"
#include "Detector.h"

using namespace cv;
using namespace std;

typedef struct visionParameter
{
	float thresBlueAreaOfROI;
	float thresCrossAreaOfROI;
}VisionParameter;

class RobotVisionManager
{
public:
	RobotVisionManager();
	~RobotVisionManager();

	void initialize();
	void initialize(VisionParameter param);
	bool findRedDot();
	bool findGoalArea();
	bool findCrossArea();
	float FindLineInImageAndComputeOffset();
	void setDebug(bool bDebug);

private:
	VisionParameter m_sParameter;
	Recognizer* m_recognizer;
	Detector* m_detector;

	bool m_bDebug;

	VideoCapture cap;

	

};

