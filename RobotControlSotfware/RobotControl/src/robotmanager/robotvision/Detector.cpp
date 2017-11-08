#include "Detector.h"
#include "PI3OpencvCompat.h"

static const cv::Scalar ROI_COLOR(0.0, 255.0, 0.0);
static const cv::Scalar ROI_COLOR2(255.0, 0.0, 255.0); // for find reddot
static const cv::Scalar ROI_COLOR3(255.0, 255.0, 0.0); // for find goal area
static const cv::Scalar TRACK_COLOR(0.0, 0.0, 255.0);
static const cv::Scalar SELECT_COLOR(255.0, 255.0, 255.0);
static const cv::Scalar NAV_COLOR(0.0, 255.0, 255.0);

Detector::Detector()
{
}


Detector::~Detector()
{
}

bool Detector::findRedDot(cv::Mat& CameraImage,bool bDebug)
{
	
	bool ret = false;

	Rect RoiRec(10, 2 * CameraImage.rows / 3, CameraImage.cols - 20, CameraImage.rows / 10); //Define region of interest rectangle

	Mat roi(CameraImage, RoiRec); // clip image to region of interest 

	Mat HSV;
	Mat threshold, threshold2;
	Mat result;

	cvtColor(roi, HSV, CV_BGR2HSV);
	inRange(HSV, Scalar(160, 120, 150), Scalar(179, 255, 255), threshold);
	inRange(HSV, Scalar(0, 120, 150), Scalar(10, 255, 255), threshold2);

	bitwise_and(roi, roi, result, threshold);
	bitwise_and(roi, roi, result, threshold2);


	Mat result2;
	cv::addWeighted(threshold, 1.0, threshold2, 1.0, 0.0, result2);

	cv::GaussianBlur(result2, result2, cv::Size(9, 9), 2, 2);

	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(result2, circles, CV_HOUGH_GRADIENT, 1, result2.rows / 8, 100, 20, 0, 0);

	if (circles.size() > 0) ret = true;

	// Loop over all detected circles and outline them on the original image
	//if (circles.size() == 0) std::exit(-1);
	if (bDebug)
	{
		rectangle(CameraImage, RoiRec, ROI_COLOR2, 3); // draw region of interest on camera image
		if (!IsPi3) imshow("threshold_redDot", threshold);
		if (!IsPi3) imshow("result_redDot", result);

		for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle)
		{
			cv::Point center(round(circles[current_circle][0]), round(circles[current_circle][1]));
			int radius = round(circles[current_circle][2]);
			cv::circle(CameraImage, cvPoint(center.x + RoiRec.x, center.y + RoiRec.y), radius, cv::Scalar(0, 255, 0), 5);
		}
		if (!IsPi3) imshow("camera_reddot", CameraImage);
	}

	return ret;
}

bool Detector::findGoalArea(cv::Mat& CameraImage, const float thresBlueAreaOfROI, bool bDebug)
{
	bool ret = false;

	//const float thresBlueAreaOfROI = 0.2;  // blue point ���� ���� // need tuning

	Rect RoiRec(10, 3 * CameraImage.rows / 4, CameraImage.cols - 20, CameraImage.rows / 12); //Define region of interest rectangle

	Mat roi(CameraImage, RoiRec); // clip image to region of interest 
	Mat HSV;
	Mat threshold;
	Mat result;

	cvtColor(roi, HSV, CV_BGR2HSV);
	inRange(HSV, Scalar(110, 0, 0), Scalar(130, 255, 255), threshold);

	bitwise_and(roi, roi, result, threshold);


	IplImage tmp = threshold;
	int cnt = cvCountNonZero(&tmp);

	if (bDebug)
	{
		rectangle(CameraImage, RoiRec, ROI_COLOR3, 3); // draw region of interest on camera image
		if (!IsPi3) imshow("threshold_goalArea", threshold);
		if (!IsPi3) imshow("result_goalArea", result);

		cout << "area" << RoiRec.area() << " bluepoint:" << cnt << endl;
		if (!IsPi3) imshow("camera_goal", CameraImage);
	}

	if (cnt > (RoiRec.area()*thresBlueAreaOfROI))
	{
		ret = true;
	}

	return ret;
}

bool Detector::findCrossArea(cv::Mat& CameraImage, const float thresCrossAreaOfROI, bool bDebug)
{
	//const float thresCrossAreaOfROI = 0.5; // ROI���� �����ϴ� Cross Area�� ����
	bool ret = false;

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat mono, blur, thresh, erodeImg, dilateImg;
	Rect RoiRec(10, 2 * CameraImage.rows / 3, CameraImage.cols - 20, CameraImage.rows / 12); //Define region of interest rectangle

	Mat roi(CameraImage, RoiRec); // clip image to region of interest 
	cvtColor(roi, mono, COLOR_BGR2GRAY);  // coovert image to grayscale 

										  //std::cout<<"Variance: "<<stddev.val[0]<<std::endl;
	GaussianBlur(mono, blur, Size(9, 9), 2, 2); // blur image to remove small irregularities
	threshold(blur, thresh, 0, 255, THRESH_BINARY_INV | THRESH_OTSU); //Color thresholding makes image more blacka nd white
	Mat erodeElmt = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat dilateElmt = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(thresh, erodeImg, erodeElmt);       // reduces noise Extract edges 
	dilate(erodeImg, dilateImg, dilateElmt);

	findContours(dilateImg, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);// Find the contours of the frame


	//////////////////////////////////
	//	find corner
	//Mat dst, dst_norm, dst_norm_scaled;
	//dst = Mat::zeros(dst.size(), CV_32FC1);

	//// Detecting corners
	//cornerHarris(mono, dst, 7, 5, 0.05, BORDER_DEFAULT);

	//// Normalizing
	//normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	//convertScaleAbs(dst_norm, dst_norm_scaled);

	//int cnt = 0;
	//// Drawing a circle around corners
	//for (int j = 0; j < dst_norm.rows; j++)
	//{
	//	for (int i = 0; i < dst_norm.cols; i++)
	//	{
	//		if ((int)dst_norm.at<float>(j, i) > 200)
	//		{
	//			//if(bDebug) circle(CameraImage, Point(i+RoiRec.x, j+RoiRec.y), 5, Scalar(0, 255, 0), 2, 8, 0);
	//			cnt++;
	//		}
	//	}
	//}
	//////////////////////////////////////

	double minMaxCx = -DBL_MAX;
	Rect selected_edge(0, 0, 0, 0); //Edge beging followed
	Rect nav_point(0, 0, 0, 0);
	double area, width;

	for (unsigned int i = 0; i<contours.size(); i++)  //Find the biggest contour 
	{
		//Moments mu = moments(contours[i]);
		Rect r = boundingRect(contours[i]);
		area = r.area();

		//if (mu.m00 > minMaxCx) minMaxCx = mu.m00;
		if (area > minMaxCx) {
			minMaxCx = area;
			width = r.width;
		}

		//if (mu.m00 > (RoiRec.area()*thresCrossAreaOfROI))
		if (area > (RoiRec.area()*thresCrossAreaOfROI))
		//if(area > (RoiRec.area()*thresCrossAreaOfROI) && cnt >= 2)   // corner+
		{
			ret = true;
		}
	}

	if (bDebug)
	{
		rectangle(CameraImage, RoiRec, ROI_COLOR, 3); // draw region of interest on camera image
		if (!IsPi3) imshow("camera_cross", CameraImage);
		cout << "ROI_area" << RoiRec.area() << " cross_area:" << minMaxCx << " width: " << width <<  endl;
		//cout << cnt << endl;
	}
	

	return ret;
}

float Detector::FindLineInImageAndComputeOffset(cv::Mat& CameraImage, bool bDebug)
{
	char text[1024];
	float offsetfromcenter;
	cv::Scalar mean, stddev;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat mono, blur, thresh, erodeImg, dilateImg;
	Rect RoiRec(10, 2 * CameraImage.rows / 3, CameraImage.cols - 20, CameraImage.rows / 12); //Define region of interest rectangle

	Mat roi(CameraImage, RoiRec); // clip image to region of interest 
	cvtColor(roi, mono, COLOR_BGR2GRAY);  // coovert image to grayscale 
	meanStdDev(mono, mean, stddev);         // Comppute image mean and stddev -- be to used later if needed

											//std::cout<<"Variance: "<<stddev.val[0]<<std::endl;
	GaussianBlur(mono, blur, Size(9, 9), 2, 2); // blur image to remove small irregularities
	threshold(blur, thresh, 0, 255, THRESH_BINARY_INV | THRESH_OTSU); //Color thresholding makes image more blacka nd white
	Mat erodeElmt = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat dilateElmt = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(thresh, erodeImg, erodeElmt);       // reduces noise Extract edges 
	dilate(erodeImg, dilateImg, dilateElmt);

	findContours(dilateImg, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);// Find the contours of the frame

	if(bDebug) rectangle(CameraImage, RoiRec, ROI_COLOR, 3); // draw region of interest on camera image

	double minMaxCx = -DBL_MAX;
	Rect selected_edge(0, 0, 0, 0); //Edge beging followed
	Rect nav_point(0, 0, 0, 0);
	for (unsigned int i = 0; i<contours.size(); i++)  //Find the biggest contour 
	{
		Moments mu = moments(contours[i]);

		if (mu.m00 > 100.0) // area threadhold
		{
			Rect r = boundingRect(contours[i]);
			Rect show(r.x + RoiRec.x, r.y + RoiRec.y, r.width, r.height);
			if(bDebug) rectangle(CameraImage, show, TRACK_COLOR, 3); // Draw contours found

			selected_edge.width = 20;
			nav_point.width = 20;
			double cx;
			cx = r.x + r.width / 2;
			//line( CameraImage,Point(cx+10,0),Point(cx+10,CameraImage.rows),Scalar( 0, 255, 0 ),2,8); 
			if (cx > minMaxCx)
			{
				minMaxCx = cx;
				selected_edge = show;
				selected_edge.x = show.x + r.width - 10;
				selected_edge.width = 20;
				nav_point = show;
				nav_point.x = 10 + minMaxCx - 10;
				nav_point.width = 20;
			}
		}
	}

	if (fabs(minMaxCx) == DBL_MAX) minMaxCx = roi.cols / 2;


	offsetfromcenter = 1.0f - 2.0f*(float)minMaxCx / roi.cols;

	if (bDebug) 
	{
		rectangle(CameraImage, selected_edge, SELECT_COLOR, 3);
		rectangle(CameraImage, nav_point, NAV_COLOR, 3);
		line(CameraImage, Point(CameraImage.cols / 2, 0), Point(CameraImage.cols / 2, CameraImage.rows), Scalar(0, 0, 0), 2, 8);
		//if (!IsPi3) imshow( "processed", dilateImg );

		sprintf(text, "Nav Value %f", offsetfromcenter);
		cv::putText(CameraImage, text, cv::Point(10, 50), CV_FONT_HERSHEY_PLAIN, 2, CV_RGB(0, 0, 0), 3);

		if (!IsPi3) imshow("FindLine", CameraImage);
	}

	return offsetfromcenter + FUDGE_BIAS;
}

float Detector::FindLineInImageAndComputeOffsetAndWidth(cv::Mat& CameraImage, int& selectedWidth, bool bDebug)
{
	char text[1024];
	float offsetfromcenter;
	cv::Scalar mean, stddev;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat mono, blur, thresh, erodeImg, dilateImg;

	//selectedWidth = 0; 

	Rect RoiRec(10, 2 * CameraImage.rows / 3, CameraImage.cols - 20, CameraImage.rows / 12); //Define region of interest rectangle

	Mat roi(CameraImage, RoiRec); // clip image to region of interest 
	cvtColor(roi, mono, COLOR_BGR2GRAY);  // coovert image to grayscale 
	meanStdDev(mono, mean, stddev);         // Comppute image mean and stddev -- be to used later if needed

											//std::cout<<"Variance: "<<stddev.val[0]<<std::endl;
	GaussianBlur(mono, blur, Size(9, 9), 2, 2); // blur image to remove small irregularities
	threshold(blur, thresh, 0, 255, THRESH_BINARY_INV | THRESH_OTSU); //Color thresholding makes image more blacka nd white
	Mat erodeElmt = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat dilateElmt = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(thresh, erodeImg, erodeElmt);       // reduces noise Extract edges 
	dilate(erodeImg, dilateImg, dilateElmt);

	findContours(dilateImg, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);// Find the contours of the frame

	if (bDebug) rectangle(CameraImage, RoiRec, ROI_COLOR, 3); // draw region of interest on camera image

	double minMaxCx = -DBL_MAX;
	Rect selected_edge(0, 0, 0, 0); //Edge beging followed
	Rect nav_point(0, 0, 0, 0);
	for (unsigned int i = 0; i<contours.size(); i++)  //Find the biggest contour 
	{
		Moments mu = moments(contours[i]);

		if (mu.m00 > 100.0) // area threadhold
		{
			Rect r = boundingRect(contours[i]);
			Rect show(r.x + RoiRec.x, r.y + RoiRec.y, r.width, r.height);
			if (bDebug) rectangle(CameraImage, show, TRACK_COLOR, 3); // Draw contours found

			selected_edge.width = 20;
			nav_point.width = 20;
			double cx;
			cx = r.x + r.width / 2;
			//line( CameraImage,Point(cx+10,0),Point(cx+10,CameraImage.rows),Scalar( 0, 255, 0 ),2,8); 
			if (cx > minMaxCx)
			{
				minMaxCx = cx;
				selected_edge = show;
				selected_edge.x = show.x + r.width - 10;
				selected_edge.width = 20;
				nav_point = show;
				nav_point.x = 10 + minMaxCx - 10;
				nav_point.width = 20;

				selectedWidth = r.width;
			}
		}
	}

	if (fabs(minMaxCx) == DBL_MAX) minMaxCx = roi.cols / 2;

	offsetfromcenter = 1.0f - 2.0f*(float)minMaxCx / roi.cols;

	if (bDebug)
	{
		rectangle(CameraImage, selected_edge, SELECT_COLOR, 3);
		rectangle(CameraImage, nav_point, NAV_COLOR, 3);
		line(CameraImage, Point(CameraImage.cols / 2, 0), Point(CameraImage.cols / 2, CameraImage.rows), Scalar(0, 0, 0), 2, 8);
		//if (!IsPi3) imshow( "processed", dilateImg );

		sprintf(text, "Nav Value %f", offsetfromcenter);
		cv::putText(CameraImage, text, cv::Point(10, 50), CV_FONT_HERSHEY_PLAIN, 2, CV_RGB(0, 0, 0), 3);

		if (!IsPi3) imshow("FindLine", CameraImage);
	}

	return offsetfromcenter + FUDGE_BIAS;
}
