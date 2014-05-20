#ifndef HANDS_H_
#define HANDS_H_

#include "Util.h"
#include "Hand.h"
#include "Line.h"

namespace proj
{
	class Hands
	{
	public:
		Hands()
		{
			mHands[0] = Hand(true);
			mHands[1] = Hand();
			mLines[0] = Line();
			mLines[1] = Line();
		}

		virtual ~Hands() {}

		Hand& getHand(int hand) { return mHands[hand]; }

		void setHandRect(int hand, cv::Rect& pos) { mHands[hand].setPrevRect(pos); }
		bool updateHands(cv::vector<cv::Rect>& hands);
		const cv::Rect& getPrevRect(int hand) const { return mHands[hand].getPrevRect(); }
		const Line& getLine(int hand) const { return mLines[hand]; }

	private:
		Hand mHands[2];
		Line mLines[2];
	};
}

#endif