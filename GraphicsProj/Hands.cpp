#include "Hands.h"
#include "Looper.h"

using namespace proj;
using namespace cv;

const int MAX_HAND_DIFF = 60;
const int MAX_AREA_DIFF = 10000;

Hands::Hands(disp::Looper& looper) :
	mLooper(looper),
	mHand(),
	mLine(),
	mCalibrated(false),
	mInCalibrationArea(false),
	mCalibrationRectOpen(),
	mCalibrationRectClosed()
{
	int screenWidth = mLooper.getDisplay().getWidth();
	int screenHeight = mLooper.getDisplay().getHeight();

	mCalibrationRectClosed.width = screenWidth / 4;
	mCalibrationRectClosed.height = screenWidth / 4;
	mCalibrationRectClosed.x = 3 * screenWidth / 8;
	mCalibrationRectClosed.y = 3 * screenHeight / 8;

	mCalibrationRectOpen.width = screenWidth / 4;
	mCalibrationRectOpen.height = screenWidth / 2;
	mCalibrationRectOpen.x = mCalibrationRectClosed.x;
	mCalibrationRectOpen.y = mCalibrationRectClosed.y;
}

#define DIST_TOLERANCE 10
#define AREA_TOLERANCE 500
#define WAIT_TIME 50

bool Hands::updateHands(vector<Rect>& hands)
{
	int size = (int)hands.size();
	bool found = false;

	if (mCalibrated)
	{
		bool found = false;
		for (int i = 0; i < size; i++)
		{
			if (mHand.checkDistance(hands[i]) && mHand.checkSize(hands[i]))
			{
				mHand.setPrevRect(hands[i]);
				mLine.addPoint(centreRect(hands[i]));
				found = true;
				break;
			}
		}
	}
	else
	{
		static const Point calibrationCentreOpen = centreRect(mCalibrationRectOpen);
		static const int openArea = mCalibrationRectOpen.area();
		static const Point calibrationCentreClosed = centreRect(mCalibrationRectClosed);
		static const int closedArea = mCalibrationRectClosed.area();

		mInCalibrationArea = false;
		bool finishedCalib = false;

		if (mCalibratingClosed)
		{
			for (int i = 0; i < size; ++i)
			{
				int dist = distDiff(centreRect(hands[i]), calibrationCentreClosed);
				if (dist <= DIST_TOLERANCE)
				{
					int area = abs(closedArea - hands[i].area());
					if (area <= AREA_TOLERANCE)
					{
						mInCalibrationArea = true;
						finishedCalib = mHand.calibrate(area, false);
						found = true;
						break;
					}
				}
			}

			if (finishedCalib)
			{
				mCalibratingClosed = false;
			}
		}
		else
		{
			static int wait = WAIT_TIME;
			if (wait > 0)
			{
				wait--;
			}
			else
			{
				for (int i = 0; i < size; ++i)
				{
					int dist = distDiff(centreRect(hands[i]), calibrationCentreOpen);
					if (dist <= DIST_TOLERANCE)
					{
						int area = abs(closedArea - hands[i].area());
						if (area <= AREA_TOLERANCE)
						{
							mInCalibrationArea = true;
							finishedCalib = mHand.calibrate(area, true);
							found = true;
							break;
						}
					}
				}

				if (finishedCalib)
				{
					mCalibrated = true;
				}
			}
		}
	}

	return found;
}

const Scalar_<uchar> happyColour(255, 255); // :)
const Scalar_<uchar> sadColour(0, 0, 255); // :(
void Hands::draw(Mat& img, bool debugDisplay) const
{
	if (!mCalibrated)
	{
		const Scalar& colour = (mInCalibrationArea) ? happyColour : sadColour;
		const Rect& rect = (mCalibratingClosed) ? mCalibrationRectClosed : mCalibrationRectOpen;

		rectangle(img, rect, colour);
	}
}