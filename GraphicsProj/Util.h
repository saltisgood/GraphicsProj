#ifndef UTIL_H_
#define UTIL_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Colour.h"

typedef unsigned int uint;

namespace proj 
{
	struct switches
	{
		switches() : mIsVideo(false), mIsCamera(false), mFilename(), mCameraId(0), mDebugDisplay(false) {}

		bool mIsVideo;
		bool mIsCamera;
		std::string mFilename;
		int mCameraId;
		bool mDebugDisplay;
	};

	const uint MAX_HANDS = 2;

	const uchar MAX_DIFF = 60;

	void chromaKey(cv::Mat& img, Colour& colour);

	void blur(cv::Mat& img, uint passes = 1);

	cv::vector<cv::Rect> sortRect(cv::vector<cv::Rect>&, int);

	cv::Point centreRect(cv::Rect&);

	int distDiff(cv::Point, cv::Point);

	void drawText(cv::Mat& img, const std::string& text, const cv::Point& org, double fontScale, const cv::Scalar& colour, bool isCentred);

#ifdef _DEBUG
	void debugDisplayImage(cv::Mat&);
#endif
}

#endif
