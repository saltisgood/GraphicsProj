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
		void composite(cv::Mat&);

	private:
		cv::Mat mBg;
		cv::Mat mEntropy;
		cv::Mat mReplacement;
	};
}

#endif