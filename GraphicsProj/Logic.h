#ifndef LOGIC_H_
#define LOGIC_H_

#include <opencv2/core/core.hpp>

#include "Display.h"
#include "Util.h"
#include "Colour.h"

namespace proj
{
	class Logic
	{
	public:
		Logic(disp::Display* display);

		bool isCalibrated() const { return mIsCalibrated; }

		void drawAnything(cv::Mat& img);
	private:
		bool mIsCalibrated;
		disp::Display *mDisplay;
		Colour mTextColour;
	};
}

#endif