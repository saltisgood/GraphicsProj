#ifndef HAND_H_
#define HAND_H_

#include <opencv2/core/core.hpp>

namespace proj
{
	class Hand
	{
	public:
		Hand(bool isLeft = false) : mPrevX(0), mPrevY(0), mIsLeft(isLeft), mPrevRect(), mOpenRect(), mCloseRect() {}
		virtual ~Hand() {}

		void calibrate(cv::Rect& rect, bool open);
		bool checkDistance(const cv::Rect& rect) const;
		bool checkSize(const cv::Rect&) const;

		const cv::Rect& getClosedRect() const { return mCloseRect; }
		const cv::Rect& getOpenRect() const { return mOpenRect; }
		const cv::Rect& getPrevRect() const { return mPrevRect; }
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
