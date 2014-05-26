#ifndef UTIL_H_
#define UTIL_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Colour.h"

#define SAFE_DEC(x) x = (x == 0) ? 0 : x - 1;

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

	const char GREEN = 1;
	const char BLUE = 2;

	void chromaKey(cv::Mat& img, const Colour& colour);

	void greenKey(cv::Mat& img);

	void rgbKey(cv::Mat& img, char colour);
	void rgbKey(cv::Mat& img, cv::Mat& outMask, char colour, bool invert = true, bool mask = true);
	void rgbKey(const cv::Mat& img, cv::Mat& outMask, char colour, bool invert = true);

	template<typename T>
	void invert(cv::Mat&, T);

	template<typename T>
	void mask(const cv::Mat&, cv::Mat&, const cv::Mat&, T);

	void blur(cv::Mat& img, uint passes = 1);

	cv::vector<cv::Rect> sortRect(const cv::vector<cv::Rect>&, int);

	cv::Point centreRect(const cv::Rect&);

	int distDiff(const cv::Point&, const cv::Point&);

	void drawText(cv::Mat& img, const std::string& text, const cv::Point& org, double fontScale, const cv::Scalar& colour, bool isCentred);

	void backgroundDiff(const cv::Mat& bg, cv::Mat& img);

	inline void updateRollingAve(int& ave, uint& ind, int newVal)
	{
		ave = (newVal + (ind * ave)) / (ind + 1);
		ind++;
	}

#ifdef _DEBUG
	void debugDisplayImage(cv::Mat&);
#endif
}

#endif
