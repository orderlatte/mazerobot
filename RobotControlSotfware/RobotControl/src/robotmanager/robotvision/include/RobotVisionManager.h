#pragma once


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
//#include "ImageProcessing.h"
#include "PI3OpencvCompat.h"

#include "Recognizer.h"
#include "Detector.h"
#include "TemplateMatch.h"

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

	bool Initialize();
	//void Initialize(VisionParameter param);
	bool FindRedDot();
	bool FindGoalArea();
	bool FindStartArea();
	bool FindCrossArea();
	float FindLineInImageAndComputeOffset();
	float FindLineInImageAndComputeOffsetAndWidth(int& selectedWidth);

	bool FindRedDot(cv::Mat& cameraimg);
	bool FindGoalArea(cv::Mat& cameraimg);
	bool FindStartArea(cv::Mat& cameraimg);
	bool FindCrossArea(cv::Mat& cameraimg);
	float FindLineInImageAndComputeOffset(cv::Mat& cameraimg);
	float FindLineInImageAndComputeOffsetAndWidth(cv::Mat& camimage, int& selectedWidth, int binParamNum=1);
	float FindLineInImageAndComputeOffsetAndWidth_OTSU(cv::Mat& camimage, int& selectedWidth);

	bool GetCamImage(cv::Mat& capimage);

	void SetDebug(bool bDebug);

	int RecognizeImage(cv::Mat& cameraimg);

	int GetRecogResultMax();

private:
	int readParameter();

	//VisionParameter m_sParameter;
	Recognizer* m_recognizer;
	Detector* m_detector;

	map<string, float> m_mParam;

	bool m_bDebug;

	VideoCapture cap;
};

