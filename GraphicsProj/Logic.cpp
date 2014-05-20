#include "Logic.h"

using namespace proj;
using namespace cv;
using namespace disp;

Logic::Logic(const Display& display) : mDisplay(display), mTextColour(255, 255, 255), mIsCalibrated(false)
{

}

void Logic::drawAnything(Mat& img)
{
	if (!mIsCalibrated)
	{
		drawText(img, "Please place your hands", Point(mDisplay.getWidth() / 2, 30), 2, mTextColour.toScalar(), true);
		drawText(img, "in the squares", Point(mDisplay.getWidth() / 2, 90), 2, mTextColour.toScalar(), true);
	}
}