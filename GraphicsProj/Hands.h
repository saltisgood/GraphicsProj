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

		void calibrateHand(int hand, bool open, cv::Rect& area);
		bool checkHandDistance(int hand, cv::Rect& rect);
		void setHandRect(int hand, cv::Rect& pos) { mHands[hand].setPrevRect(pos); }
		bool updateHands(cv::vector<cv::Rect>& hands);
		bool checkSize(cv::Rect& hand, int handNo);
		cv::Rect getPrevRect(int hand) { return mHands[hand].getPrevRect(); }
		Line getLine(int hand) { return mLines[hand]; }

	private:
		Hand mHands[2];
		Line mLines[2];
	};
}

#endif