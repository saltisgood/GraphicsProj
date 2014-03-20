#include "Hands.h"

using namespace proj;
using namespace cv;

const int MAX_HAND_DIFF = 60;
const int MAX_AREA_DIFF = 10000;

bool Hands::updateHands(vector<Rect>& hands)
{
	int size = (int)hands.size();

	int index = -1;
	bool found2 = false;

	for (int i = 0; i < size; i++)
	{
		if (mHands[0].checkDistance(hands[i]) && mHands[0].checkSize(hands[i]))
		{
			mHands[0].setPrevRect(hands[i]);
			index = i;
			mLines[0].addPoint(centreRect(hands[i]));
			break;
		}
	}

	for (int i = 0; i < size; i++)
	{
		if (i != index && mHands[1].checkDistance(hands[i]) && mHands[1].checkSize(hands[i]))
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