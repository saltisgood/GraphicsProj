#ifndef HAND_H_
#define HAND_H_

#include <opencv2/core/core.hpp>
#include "Util.h"

namespace proj
{
	class Hand
	{
		friend class Hands;

	public:
		Hand() : mPrevX(0), mPrevY(0), mPrevRect(), mOpenRect(), mCloseRect() {}
		virtual ~Hand() {}

		void calibrate(cv::Rect& rect, bool open);
		bool checkDistance(const cv::Rect& rect) const;
		bool checkSize(const cv::Rect&) const;

		// Calibrate the Hand with an area for a known state. Returns a value that says whether the hand has
		// reached required calibration.
		bool calibrate(int area, bool open);

		const cv::Rect& getClosedRect() const { return mCloseRect; }
		const cv::Rect& getOpenRect() const { return mOpenRect; }
		const cv::Rect& getPrevRect() const { return mPrevRect; }
		void setClosedRect(cv::Rect& closedRect) { mCloseRect = closedRect; }
		void setOpenRect(cv::Rect& openRect) { mOpenRect = openRect; }
		void setPrevRect(cv::Rect& prevRect) { mPrevRect = prevRect; }

	private:
		int mPrevX;
		int mPrevY;
		cv::Rect mPrevRect;
		cv::Rect mOpenRect;
		cv::Rect mCloseRect;
		int mCloseArea;
		uint mCloseAreaCalibCount;
		int mOpenArea;
		uint mOpenAreaCalibCount;

		static const int MAX_HAND_DIFF = 60;
		static const int MAX_AREA_DIFF = 10000;
	};
}

#endif
