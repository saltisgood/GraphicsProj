#ifndef HANDS_H_
#define HANDS_H_

#include "Util.h"
#include "Hand.h"
#include "Line.h"

namespace disp
{
	class Looper;
}

namespace proj
{
	class Hands
	{
	public:
		Hands(disp::Looper& looper);

		virtual ~Hands() {}

		Hand& getHand() { return mHand; }

		void setHandRect(cv::Rect& pos) { mHand.setPrevRect(pos); }
		bool updateHands(cv::vector<cv::Rect>& hands);
		const cv::Rect& getPrevRect() const { return mHand.getPrevRect(); }
		const Line& getLine() const { return mLine; }
		void draw(cv::Mat&, bool) const;

	private:
		disp::Looper& mLooper;
		Hand mHand;
		Line mLine;
		bool mCalibrated;
		bool mCalibratingClosed;
		bool mInCalibrationArea;
		cv::Rect mCalibrationRectOpen;
		cv::Rect mCalibrationRectClosed;
	};
}

#endif