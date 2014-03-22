#ifndef HAND_H_
#define HAND_H_

#include <opencv2/core/core.hpp>

#include "Util.h"

namespace proj
{
	class Hand
	{
	public:
		Hand() : mPrevX(0), mPrevY(0), mIsLeft(false), mPrevRect(), mOpenRect(), mCloseRect() {}
		Hand(bool isLeft) : mPrevX(0), mPrevY(0), mIsLeft(isLeft), mPrevRect(), mOpenRect(), mCloseRect() {}
		virtual ~Hand() {}

		void calibrate(cv::Rect& rect, bool open);
		bool checkDistance(cv::Rect& rect);
		bool checkSize(cv::Rect&);

		cv::Rect& getClosedRect() { return mCloseRect; }
		cv::Rect& getOpenRect() { return mOpenRect; }
		cv::Rect& getPrevRect() { return mPrevRect; }
		void setClosedRect(cv::Rect& closedRect) { mCloseRect = closedRect; }
		void setOpenRect(cv::Rect& openRect) { mOpenRect = openRect; }
		void setPrevRect(cv::Rect& prevRect) { mPrevRect = prevRect; }

		static const int LEFT = 0;
		static const int RIGHT = 1;

	private:
		int mPrevX;
		int mPrevY;
		bool mIsLeft;
		cv::Rect mPrevRect;
		cv::Rect mOpenRect;
		cv::Rect mCloseRect;

		static const int MAX_HAND_DIFF = 60;
		static const int MAX_AREA_DIFF = 10000;
	};
}

#endif
