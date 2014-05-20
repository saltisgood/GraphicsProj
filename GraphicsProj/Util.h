#ifndef UTIL_H_
#define UTIL_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Colour.h"

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;

namespace proj 
{
	struct switches
	{
		switches() : isVideo(false), isCamera(false), filename(), cameraId(0), debugDisplay(false) {}

		bool isVideo;
		bool isCamera;
		std::string filename;
		int cameraId;
		bool debugDisplay;
	};

	const uint MAX_HANDS = 2;

	const uchar MAX_DIFF = 60;

	void chromaKey(cv::Mat& img, const Colour& colour);

	void blur(cv::Mat& img, uint passes = 1);

	cv::vector<cv::Rect> sortRect(const cv::vector<cv::Rect>&, int);

	cv::Point centreRect(const cv::Rect&);

	int distDiff(const cv::Point&, const cv::Point&);

	void drawText(cv::Mat& img, const std::string& text, const cv::Point& org, double fontScale, const cv::Scalar& colour, bool isCentred);

#ifdef _DEBUG
	void debugDisplayImage(cv::Mat&);
#endif
}

#endif
