#include "Hand.h"

using namespace proj;
using namespace cv;

void Hand::calibrate(Rect& rect, bool open)
{
	if (open)
	{
		mOpenRect = rect;
		mPrevRect = rect;
	}
	else
	{
		mCloseRect = rect;
		mPrevRect = rect;
	}
}

bool Hand::checkDistance(Rect& rect)
{
	int dist = distDiff(centreRect(mPrevRect), centreRect(rect));
	return dist <= MAX_HAND_DIFF;
}

bool Hand::checkSize(Rect& hand)
{
	int area = hand.area();
	int openArea = mOpenRect.area();
	int closeArea = mCloseRect.area();

	if (abs(area - closeArea) <= MAX_AREA_DIFF || (abs(area - openArea) <= MAX_AREA_DIFF))
	{
		return true;
	}
	else if (area >= closeArea && area <= openArea)
	{
		return true;
	}
	else
	{
		return false;
	}
}