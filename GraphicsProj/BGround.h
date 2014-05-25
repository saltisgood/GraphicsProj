#ifndef BGROUND_H_
#define BGROUND_H_

#include <opencv2\core\core.hpp>

namespace proj
{
	class BackGround
	{
	public:
		BackGround();
		virtual ~BackGround() {}

		void forceBackground(const cv::Mat& newBg);
		void extractForeground(cv::Mat&);

	private:
		cv::Mat mBg;
		mutable int mFramesSinceBG;
		mutable int mEntropy;
	};
}

#endif