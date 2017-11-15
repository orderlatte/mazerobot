#include "Recognizer.h"

Recognizer::Recognizer()
{
	if (readRefImages(symbols) == -1) {
		fprintf(stderr, "Error reading reference symbols\n");
		exit(0);
	}

	m_bDebug = false;

	//createTrackbar("Min Threshold:", "A", &lowThreshold, 100, CannyThreshold);
}


Recognizer::~Recognizer()
{
}

int Recognizer::readRefImages(Symbol *symbols)
{
	symbols[0].img = imread("arrowGo.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[0].img.data)
		return -1;
	threshold(symbols[0].img, symbols[0].img, 100, 255, 0);
	symbols[0].name = "Go";

	symbols[1].img = imread("arrowStop.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[1].img.data)
		return -1;
	threshold(symbols[1].img, symbols[1].img, 100, 255, 0);
	symbols[1].name = "Stop";

	symbols[2].img = imread("arrowS.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[2].img.data)
		return -1;
	threshold(symbols[2].img, symbols[2].img, 100, 255, 0);
	symbols[2].name = "Up";

	symbols[3].img = imread("arrowB.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[3].img.data)
		return -1;
	threshold(symbols[3].img, symbols[3].img, 100, 255, 0);
	symbols[3].name = "Ball";

	return 0;
}

int Recognizer::initialize()
{
	return -1;
}

int Recognizer::getRecognitionResult()
{
	return -1;
}

void Recognizer::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
}

