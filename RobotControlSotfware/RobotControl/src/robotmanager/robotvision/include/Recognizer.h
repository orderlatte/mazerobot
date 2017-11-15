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

using namespace std;
using namespace cv;

//class Symbol {
//
//public:
//	Mat img;
//	string name;
//
//};

#define NumOfSigns 4

class Symbol {

public:
	Mat img;
	string name;
};

class Recognizer
{
public:
	Recognizer();
	virtual ~Recognizer();

	virtual int Recognize(Mat& img)=0;
	void SetDebug(bool bDebug);

	virtual int initialize();
	virtual int getRecognitionResult();

protected:
	Symbol symbols[NumOfSigns];
	virtual int readRefImages(Symbol *symbols);

	bool m_bDebug;
	
};

