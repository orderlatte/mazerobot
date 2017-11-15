#include "TemplateMatch.h"
#include "PI3OpencvCompat.h"

int lowThreshold;

void CannyThreshold(int, void*) {
}


TemplateMatch::TemplateMatch()
{
	initialize();
}


TemplateMatch::~TemplateMatch()
{
}

void TemplateMatch::sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center) {
	std::vector<cv::Point2f> top, bot;

	for (unsigned int i = 0; i < corners.size(); i++) {
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}

	cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
	cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
	cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

	corners.clear();
	corners.push_back(tl);
	corners.push_back(tr);
	corners.push_back(br);
	corners.push_back(bl);
}


int TemplateMatch::Recognize(Mat& camera)
{
	if (!IsPi3 && m_bDebug) {
		namedWindow("A", CV_WINDOW_AUTOSIZE);
		namedWindow("B", CV_WINDOW_AUTOSIZE);
		namedWindow("C", CV_WINDOW_AUTOSIZE);

		createTrackbar("Min Threshold:", "A", &lowThreshold, 100, CannyThreshold);
	}

	int match=-1;

	Mat new_image,greyImg;

	cvtColor(camera, greyImg, CV_RGB2GRAY);

	//threshold(greyImg, greyImg, 100, 255, 0);

	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	GaussianBlur(greyImg, greyImg, Size(9, 9), 2, 2);

	/// Detect edges using canny
	Canny(greyImg, canny_output, lowThreshold, lowThreshold * 3, 3);

	//	imshow("B",canny_output);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE,
		CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<Point> approxRect;

	for (size_t i = 0; i < contours.size(); i++) {
		approxPolyDP(contours[i], approxRect,
			arcLength(Mat(contours[i]), true) * 0.01, true);
		if (approxRect.size() == 4) {
			float area = contourArea(contours[i]);

			if (area > 10000) {
				std::vector<cv::Point2f> corners;

				vector<Point>::iterator vertex;
				vertex = approxRect.begin();
				//vertex++;
				if (m_bDebug) circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);
				vertex++;
				if (m_bDebug) circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);
				vertex++;
				if (m_bDebug) circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);
				vertex++;
				if (m_bDebug) circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);

				Moments mu;
				mu = moments(contours[i], false);
				Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);

				sortCorners(corners, center);

				// Define the destination image
				Mat correctedImg = ::Mat::zeros(195, 271, CV_8UC3);

				// Corners of the destination image
				std::vector<cv::Point2f> quad_pts;
				quad_pts.push_back(Point2f(0, 0));
				quad_pts.push_back(Point2f(correctedImg.cols, 0));
				quad_pts.push_back(
					Point2f(correctedImg.cols, correctedImg.rows));
				quad_pts.push_back(Point2f(0, correctedImg.rows));

				// Get transformation matrix
				Mat transmtx = getPerspectiveTransform(corners, quad_pts);

				// Apply perspective transformation
				warpPerspective(camera, correctedImg, transmtx,
					correctedImg.size());

				Mat correctedImgBin;

				cvtColor(correctedImg, correctedImgBin, CV_RGB2GRAY);

				//equalizeHist(correctedImgBin, correctedImgBin);

				correctedImgBin.copyTo(new_image);

				threshold(correctedImgBin, correctedImgBin, 140, 255, 0);

				if (!IsPi3 && m_bDebug) imshow("B", correctedImgBin);

				double minVal, maxVal, medVal;


				minMaxLoc(new_image, &minVal, &maxVal);

				medVal = (maxVal - minVal) / 2;

				threshold(new_image, new_image, medVal, 255, 0);

				if (!IsPi3 && m_bDebug) imshow("C", new_image);


				Mat diffImg;

				int minDiff, diff;
				minDiff = 12000;
				match = -1;

				for (int i = 0; i < NumOfSigns; i++) {
					//diffImg = symbols[i].img-correctedImgBin;

					bitwise_xor(new_image, symbols[i].img, diffImg,
						noArray());

					diff = countNonZero(diffImg);
					if (diff < minDiff) {
						minDiff = diff;
						match = i;
					}

					if (i == 0) {
						//	imshow("B",diffImg);

					}
				}

				//imshow("B", correctedImg);

				if (match != -1) {
					putText(camera, symbols[match].name, Point(320, 30), 1,
						2, Scalar(0, 255, 0), 2);
					printf("Match %s\n", symbols[match].name.c_str());

				}
				else printf("No Match\n");

				//break;
			}

		}
	}

	if (!IsPi3 && m_bDebug) imshow("A", camera);

	m_mRecogResult[match]++;
	return match;
}

int TemplateMatch::initialize()
{
	m_mRecogResult[-1] = 0;	// no match
	for (int i = 0; i < NumOfSigns; i++)
	{
		m_mRecogResult[i] = 0;
	}

	return 0;
}

int TemplateMatch::getRecognitionResult()
{
	auto max = std::max_element(m_mRecogResult.begin(), m_mRecogResult.end(), LessBySecond());

	cout << "!!!!!!!!!!!!!!!!!!" << endl;
	//map<int,int >::iterator max = std::max_element(m_mRecogResult.begin(), m_mRecogResult.end(), m_mRecogResult.value_comp());

	for (int i = 0; i < m_mRecogResult.size()-1; i++)
	{
		cout << m_mRecogResult[i] << " ";
	}

	cout << m_mRecogResult[-1] << endl;

	initialize();

	return max->first;
}
