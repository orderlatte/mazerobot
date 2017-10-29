//------------------------------------------------------------------------------------------------
// File: ImageProcessing.h
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Processes images from the Camera to locate various attributes
//------------------------------------------------------------------------------------------------
#ifndef ImageProcessingH
#define ImageProcessingH
#include <opencv2/core/core.hpp>

float FindLineInImageAndComputeOffset(cv::Mat& CameraImage);

#endif
//------------------------------------------------------------------------------------------------
//END of Include
//------------------------------------------------------------------------------------------------
