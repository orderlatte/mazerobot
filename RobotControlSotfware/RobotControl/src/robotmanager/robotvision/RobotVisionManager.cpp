#include "RobotVisionManager.h"


//static int            AWidth;
//static int            AHeight;
//static int            Pan;
//static int            Tilt;
//static CvCapture *    capture = NULL;
//static bool           Run = false;

#define WIDTH 640
#define HEIGHT 480

static CvCapture *    capture=NULL;


RobotVisionManager::RobotVisionManager()
{
	m_recognizer = new Recognizer();
	m_detector = new Detector();

	m_bDebug = false;

	m_sParameter.thresBlueAreaOfROI = (float)0.2;
	m_sParameter.thresCrossAreaOfROI = (float)0.7;

	//cout << "camera open try...!!!" << endl;

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

//	if (capture != NULL)
//	{
//		cvReleaseCapture(&capture); // Close camera
//		capture = NULL;
//	}
}

bool RobotVisionManager::Initialize()
{
	if (cap.isOpened())
	{
		cap.release();
	}

	if (!cap.open(0))
	{
		cap.set(CAP_PROP_FRAME_WIDTH, WIDTH);
		cap.set(CAP_PROP_FRAME_HEIGHT, HEIGHT);

		fprintf(stderr, "Camera Failed to Initialize\n");
		return false;
	}

	cout << "camera open success!!!" << endl;

	cap.set(CAP_PROP_FRAME_WIDTH, WIDTH);
	cap.set(CAP_PROP_FRAME_HEIGHT, HEIGHT);

	//return true;

//	capture = cvCreateCameraCapture(0);   // Open default Camera
//	if (!capture)
//	{
//		printf("Camera Not Initialized\n");
//		//CleanUp();
//		return 0;
//	}
//
//	if (cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, WIDTH) == 0) // Set camera width
//	{
//		printf("cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,WIDTH) Failed)\n");
//	}
//
//	if (cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, HEIGHT) == 0) // Set camera height
//	{
//		printf("cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,HEIGHT) Failed)\n");
//	}

	//AWidth = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	//AHeight = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	return true;
}

void RobotVisionManager::Initialize(VisionParameter param)
{
	m_sParameter = param;
	Initialize();
}

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
	return m_detector->findGoalArea(cameraimg, m_sParameter.thresBlueAreaOfROI, m_bDebug);
}

bool RobotVisionManager::FindCrossArea()
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
	return m_detector->findGoalArea(cameraimg, m_sParameter.thresBlueAreaOfROI, m_bDebug);
}

bool RobotVisionManager::FindCrossArea(cv::Mat& camimage)
{
	Mat cameraimg;
	cap >> cameraimg;
	return m_detector->findCrossArea(cameraimg, m_sParameter.thresCrossAreaOfROI, m_bDebug);
}

float RobotVisionManager::FindLineInImageAndComputeOffset(cv::Mat& cameraimg)
{
	return m_detector->FindLineInImageAndComputeOffset(cameraimg, m_bDebug);
}

float RobotVisionManager::FindLineInImageAndComputeOffsetAndWidth(cv::Mat& cameraimg, int& selectedWidth)
{
	return m_detector->FindLineInImageAndComputeOffsetAndWidth(cameraimg, selectedWidth, m_bDebug);
}

void RobotVisionManager::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
}

bool RobotVisionManager::GetCamImage(cv::Mat& capimage)
{
	cap >> capimage;
	return true;
}
