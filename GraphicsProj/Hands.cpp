#include "Hands.h"

using namespace proj;
using namespace cv;

const int MAX_HAND_DIFF = 60;
const int MAX_AREA_DIFF = 10000;

void Hands::calibrateHand(int hand, bool open, Rect& area)
{
	assert(hand == 0 || hand == 1);

	if (open)
	{
		mHands[hand].setOpenRect(area);
		mLines[hand].addPoint(centreRect(area));
	}
	else
	{
		mHands[hand].setClosedRect(area);
		mLines[hand].addPoint(centreRect(area));
	}
}

bool Hands::checkHandDistance(int hand, Rect& rect)
{
	assert(hand == 0 || hand == 1);

	Rect prev = mHands[hand].getPrevRect();
	
	int dist = distDiff(centreRect(prev), centreRect(rect));
	return dist <= MAX_HAND_DIFF;

	//return abs(mHands[hand].getPrevRect().x - x) <= MAX_HAND_DIFF;
}

bool Hands::checkSize(Rect& hand, int handNo)
{
	assert(handNo == 0 || handNo == 1);

	int area = hand.area();
	int openArea = mHands[handNo].getOpenRect().area();
	int closeArea = mHands[handNo].getClosedRect().area();

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

bool Hands::updateHands(vector<Rect>& hands)
{
	int size = (int)hands.size();

	int index = -1;
	bool found2 = false;

	for (int i = 0; i < size; i++)
	{
		if (checkHandDistance(0, hands[i]) && checkSize(hands[i], 0))
		{
			mHands[0].setPrevRect(hands[i]);
			index = i;
			mLines[0].addPoint(centreRect(hands[i]));
			break;
		}
	}

	for (int i = 0; i < size; i++)
	{
		if (i != index && checkHandDistance(1, hands[i]) && checkSize(hands[i], 1))
		{
			mHands[1].setPrevRect(hands[i]);
			found2 = true;
			mLines[1].addPoint(centreRect(hands[i]));
			break;
		}
	}

	if (index == -1 || !found2)
	{
		return false;
	}
	else
	{
		return true;
	}
}