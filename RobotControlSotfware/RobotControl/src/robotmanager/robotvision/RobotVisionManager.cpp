#include "RobotVisionManager.h"
#include <fstream>


//static int            AWidth;
//static int            AHeight;
//static int            Pan;
//static int            Tilt;
//static CvCapture *    capture = NULL;
//static bool           Run = false;

#define WIDTH 640
#define HEIGHT 480
#define MAX_SIZE 1000

char inputString[MAX_SIZE];


RobotVisionManager::RobotVisionManager()
{
	m_recognizer = new TemplateMatch();
	m_detector = new Detector();

	m_bDebug = false;

	//m_sParameter.thresBlueAreaOfROI = (float)0.2;
	//m_sParameter.thresCrossAreaOfROI = (float)0.7;

	if(!Initialize()) cout<< "[RobotVisionManager] Initialization fail !!!!!!" << endl;
}


RobotVisionManager::~RobotVisionManager()
{
	delete m_recognizer;
	delete m_detector;

	if (cap.isOpened())
	{
		cap.release();
	}

	//if (capture != NULL)
	//{
	//	cvReleaseCapture(&capture); // Close camera
	//	capture = NULL;
	//}
}

bool RobotVisionManager::Initialize()
{
	if (!cap.open(0))
	{
		fprintf(stderr, "Camera Failed to Initialize\n");
		return false;
	}
	cap.set(CAP_PROP_FRAME_WIDTH, WIDTH);
	cap.set(CAP_PROP_FRAME_HEIGHT, HEIGHT);

	readParameter();
	m_detector->SetParameter(m_mParam);

	return true;

	//capture = cvCreateCameraCapture(0);   // Open default Camera
	//if (!capture)
	//{
	//	printf("Camera Not Initialized\n");
	//	//CleanUp();
	//	return 0;
	//}

	//if (cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, WIDTH) == 0) // Set camera width 
	//{
	//	printf("cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,WIDTH) Failed)\n");
	//}

	//if (cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, HEIGHT) == 0) // Set camera height
	//{
	//	printf("cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,HEIGHT) Failed)\n");
	//}

	//AWidth = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	//AHeight = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
}

//void RobotVisionManager::Initialize(VisionParameter param)
//{
//	//m_sParameter = param;
//	Initialize();
//}

bool RobotVisionManager::FindRedDot()
{
	Mat cameraimg;
	cap >> cameraimg;
	return m_detector->findRedDot(cameraimg,m_bDebug);
}

bool RobotVisionManager::FindGoalArea()
{
	Mat cameraimg;
	cap >> cameraimg;
	//return m_detector->findGoalArea(cameraimg, m_sParameter.thresBlueAreaOfROI, m_bDebug);
	return m_detector->findGoalArea(cameraimg, m_bDebug);
}

bool RobotVisionManager::FindStartArea()
{
	Mat cameraimg;
	cap >> cameraimg;
	//return m_detector->findGoalArea(cameraimg, m_sParameter.thresBlueAreaOfROI, m_bDebug);
	return m_detector->findStartArea(cameraimg, m_bDebug);
}

bool RobotVisionManager::FindCrossArea()
{
	Mat cameraimg;
	cap >> cameraimg;
	return m_detector->findCrossArea(cameraimg, m_bDebug);
}

float RobotVisionManager::FindLineInImageAndComputeOffset()
{
	Mat cameraimg;
	cap >> cameraimg;
	return m_detector->FindLineInImageAndComputeOffset(cameraimg, m_bDebug);
}

float RobotVisionManager::FindLineInImageAndComputeOffsetAndWidth(int& selectedWidth)
{
	Mat cameraimg;
	cap >> cameraimg;
	return m_detector->FindLineInImageAndComputeOffsetAndWidth(cameraimg, selectedWidth, m_bDebug);
}

bool RobotVisionManager::FindRedDot(cv::Mat& cameraimg)
{
	return m_detector->findRedDot(cameraimg, m_bDebug);
}

bool RobotVisionManager::FindGoalArea(cv::Mat& cameraimg)
{
	return m_detector->findGoalArea(cameraimg,  m_bDebug);
}

bool RobotVisionManager::FindStartArea(cv::Mat& cameraimg)
{
	return m_detector->findStartArea(cameraimg, m_bDebug);
}


bool RobotVisionManager::FindCrossArea(cv::Mat& camimage)
{
	Mat cameraimg;
	cap >> cameraimg;
	return m_detector->findCrossArea(cameraimg, m_bDebug);
}

float RobotVisionManager::FindLineInImageAndComputeOffset(cv::Mat& cameraimg)
{
	return m_detector->FindLineInImageAndComputeOffset(cameraimg, m_bDebug);
}

float RobotVisionManager::FindLineInImageAndComputeOffsetAndWidth(cv::Mat& cameraimg, int& selectedWidth, int binParamNum)
{
	return m_detector->FindLineInImageAndComputeOffsetAndWidth(cameraimg, selectedWidth, binParamNum, m_bDebug);
}

float RobotVisionManager::FindLineInImageAndComputeOffsetAndWidth_OTSU(cv::Mat& cameraimg, int& selectedWidth)
{
	return m_detector->FindLineInImageAndComputeOffsetAndWidth_OTSU(cameraimg, selectedWidth,m_bDebug);
}


void RobotVisionManager::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
	m_recognizer->SetDebug(bDebug);
}

bool RobotVisionManager::GetCamImage(cv::Mat& capimage)
{
	cap >> capimage;
	return true;
}

int RobotVisionManager::RecognizeImage(cv::Mat& cameraimg)
{
	return m_recognizer->Recognize(cameraimg);
}

int RobotVisionManager::readParameter()
{
	string tmp;
	float tmpFloat;

	ifstream inFile("VisionParameter.txt");

	if (inFile.is_open())
	{
		while (!inFile.eof()) {
			inFile >> tmp;
			inFile >> tmpFloat;
			m_mParam[tmp] = tmpFloat;
		}
		cout << "VisionParameter reading complete!!!" << endl;
	}
	else
	{
		cerr << "Can't open!!! : VisionParameter.txt" << endl;
		return 0;
	}

	inFile.close();

	return m_mParam.size();
}
