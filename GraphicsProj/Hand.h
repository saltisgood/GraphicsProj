#ifndef HAND_H_
#define HAND_H_

#include <opencv2\core\core.hpp>

namespace proj
{
	class Hand
	{
	public:
		Hand() : mPrevX(0), mPrevY(0), mIsLeft(false), mPrevRect(), mOpenRect(), mCloseRect() {}
		Hand(bool isLeft) : mPrevX(0), mPrevY(0), mIsLeft(isLeft), mPrevRect(), mOpenRect(), mCloseRect() {}
		virtual ~Hand() {}

		cv::Rect getClosedRect() { return mCloseRect; }
		cv::Rect getOpenRect() { return mOpenRect; }
		cv::Rect getPrevRect() { return mPrevRect; }
		void setClosedRect(cv::Rect& closedRect) { mCloseRect = closedRect; }
		void setOpenRect(cv::Rect& openRect) { mOpenRect = openRect; }
		void setPrevRect(cv::Rect& prevRect) { mPrevRect = prevRect; }

	private:
		int mPrevX;
		int mPrevY;
		bool mIsLeft;
		cv::Rect mPrevRect;
		cv::Rect mOpenRect;
		cv::Rect mCloseRect;
	};
}

#endif