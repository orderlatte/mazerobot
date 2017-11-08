#pragma once
#include "Recognizer.h"

class TemplateMatch :
	public Recognizer
{
public:
	TemplateMatch();
	~TemplateMatch();

	//int lowThreshold;

	int Recognize(Mat& img);
private:
	void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center);
};

