#pragma once
#include "Recognizer.h"
#include <map>
#include <algorithm>

using namespace std;

struct LessBySecond
{
	template <typename Lhs, typename Rhs>
	bool operator()(const Lhs& lhs, const Rhs& rhs) const
	{
		return lhs.second < rhs.second;
	}
};

class TemplateMatch :
	public Recognizer
{
public:
	TemplateMatch();
	~TemplateMatch();

	//int lowThreshold;

	int Recognize(Mat& img);
	int initialize();
	int getRecognitionResult();
private:
	map<int, int > m_mRecogResult;
	void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center);
};

