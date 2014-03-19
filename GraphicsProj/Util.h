#ifndef UTIL_H_
#define UTIL_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Colour.h"

#define uint unsigned int

namespace proj 
{
	struct switches
	{
		switches() : mIsVideo(false), mIsCamera(false), mFilename(), mCameraId(0) {}

		bool mIsVideo;
		bool mIsCamera;
		std::string mFilename;
		int mCameraId;
	};

	const uchar MAX_DIFF = 60;

	void chromaKey(cv::Mat& img, Colour& colour);

	void blur(cv::Mat& img, uint passes = 1);

	cv::vector<cv::Rect> sortRect(cv::vector<cv::Rect>&, int);

	cv::Point centreRect(cv::Rect&);

	int distDiff(cv::Point, cv::Point);
}

#endif