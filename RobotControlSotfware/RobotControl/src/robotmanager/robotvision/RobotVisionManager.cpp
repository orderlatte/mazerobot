#include "include/RobotVisionManager.h"


RobotVisionManager::RobotVisionManager()
{
	m_recognizer = new Recognizer();
	m_detector = new Detector();

	m_bDebug = false;

	m_sParameter.thresBlueAreaOfROI = (float)0.2;
	m_sParameter.thresCrossAreaOfROI = (float)0.7;

	initialize();
}


RobotVisionManager::~RobotVisionManager()
{
	delete m_recognizer;
	delete m_detector;
}

void RobotVisionManager::initialize()
{
	cap.open(0);

	//namedWindow("A", CV_WINDOW_AUTOSIZE);
	//namedWindow("B", CV_WINDOW_AUTOSIZE);
	//namedWindow("C", CV_WINDOW_AUTOSIZE);

	/////////////////////////////////////////////
	//namedWindow("LineFollower", CV_WINDOW_AUTOSIZE);
	/////////////////////////////////////////////


	//Mat camera;
	//Mat greyImg;
	//Mat lineFollowingImg;
	//Mat crossImg, lineImg;

	////Symbol symbols[10];
	////if (readRefImages(symbols) == -1) {
	////	printf("Error reading reference symbols\n");
	////	return -1;
	////}

	////createTrackbar("Min Threshold:", "A", &lowThreshold, 100, CannyThreshold);

	//double t = (double)getTickCount();
	//// do something ...
	////t = ((double)getTickCount() - t)/getTickFrequency();
	////cout << "Times passed in seconds: " << t << endl;

	//cap >> camera;


	//Mat new_image;

	//while (1) {

	//	t = (double)getTickCount() - t;

	//	cap >> camera;
	//	cap >> lineFollowingImg;
	//	cap >> crossImg;
	//	cap >> lineImg;


	//	///////////////////////////////////
	//	//float offset = FindLineInImageAndComputeOffset(lineFollowingImg); // Process camera image / locat line and compute offset from line

	//	//FindCrossPoint(crossImg);
	//	//FindCornerPoint(crossImg);
	//	//FindLine(lineImg);
	//	//bool ret = FindGoalArea(camera);
	//	//if (ret) printf("Find GoalArea!!!!!\n");
	//	//FindRedDot(camera);
	//	//bool ret = FindCrossArea(camera);
	//	//if(ret) printf("Find CrossArea!!!!!\n");
	//	///////////////////////////////////
	//}
}

void RobotVisionManager::initialize(VisionParameter param)
{
	m_sParameter = param;
	initialize();
}

bool RobotVisionManager::findRedDot()
{
	Mat cameraimg;
	cap >> cameraimg;
	return m_detector->findRedDot(cameraimg,m_bDebug);
}

bool RobotVisionManager::findGoalArea()
{
	Mat cameraimg;
	cap >> cameraimg;
	return m_detector->findGoalArea(cameraimg, m_sParameter.thresBlueAreaOfROI, m_bDebug);
}

bool RobotVisionManager::findCrossArea()
{
	Mat cameraimg;
	cap >> cameraimg;
	return m_detector->findCrossArea(cameraimg, m_sParameter.thresCrossAreaOfROI, m_bDebug);
}

float RobotVisionManager::FindLineInImageAndComputeOffset()
{
	Mat cameraimg;
	cap >> cameraimg;
	return m_detector->FindLineInImageAndComputeOffset(cameraimg, m_bDebug);
}

void RobotVisionManager::setDebug(bool bDebug)
{
	m_bDebug = bDebug;
}