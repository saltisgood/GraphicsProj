#include "BGround.h"
#include "Colour.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace proj;
using namespace cv;

const ushort MIN_DIFF = 10;

BackGround::BackGround() : 
	mBg(),
	mFramesSinceBG(0),
	mEntropy(0)
{

}

void BackGround::forceBackground(const Mat& newBg)
{
	mBg = newBg.clone();
	mFramesSinceBG = 0;
	mEntropy = 0;
}

void BackGround::extractForeground(Mat& img)
{
	Mat tmp;
	resize(img, tmp, Size(), 0.5, 0.5, CV_INTER_AREA);

	if (mBg.empty())
	{
		//forceBackground(img);
		forceBackground(tmp);
	}
	//img -= mBg;

	uchar *p;
	const uchar *q;
	int rows = tmp.rows;
	int channels = tmp.channels();
	int cols = tmp.cols * channels;

	if (mBg.isContinuous() && tmp.isContinuous())
	{
		cols *= rows;
		rows = 1;

		p = tmp.data;
		q = mBg.data;
		for (int i = 0; i < cols; i += channels, p += channels, q += channels)
		{
			if (Colour::diff(p, q) <= MIN_DIFF)
			{
				p[0] = 0;
				p[1] = 0;
				p[2] = 0;
			}
		}
	}
	else
	{
	for (int i = 0; i < rows; i++)
	{
		p = tmp.ptr<uchar>(i);
		q = mBg.ptr<uchar>(i);

		for (int j = 0; j < cols; j += channels)
		{
			if (Colour::diff(p[j], q[j]) <= MIN_DIFF)
			{
				// WARNING: Assumes channels == 3
				p[j] = 0;
				p[j + 1] = 0;
				p[j + 2] = 0;
			}
		}
	}
	}
}