#include "BGround.h"
#include "Colour.h"
#include "Threading.h"
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

void threadTest(uchar threadNo, uchar threadNums, void *pbg, void *ptmp, void *, void *)
{
	Mat * bg = (Mat *)pbg;
	Mat * tmp = (Mat *)ptmp;

	uchar *p;
	uchar *q;
	static int rows = tmp->rows;
	static int channels = tmp->channels();
	static int cols = tmp->cols * channels;

	int tcount = rows / threadNums;
	int start = (int)threadNo * tcount;
	tcount += start;

	if (bg->isContinuous() && tmp->isContinuous())
	{
		tcount *= cols;
		start *= cols;

		p = tmp->data + start;
		q = bg->data + start;

		for (int i = start; i < tcount; i += channels, p += channels, q += channels)
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
		for (int i = start; i < tcount; i++)
		{
			p = tmp->ptr<uchar>(i);
			q = bg->ptr<uchar>(i);

			for (int j = 0; j < cols; j += channels, p += channels, q += channels)
			{
				if (Colour::diff(p, q) <= MIN_DIFF)
				{
					// WARNING: Assumes channels == 3
					p[0] = 0;
					p[1] = 0;
					p[2] = 0;
				}
			}
		}
	}
}

void BackGround::extractForeground(Mat& img)
{
	//Mat tmp;
	//resize(img, tmp, Size(), 0.5, 0.5, CV_INTER_AREA);

	if (mBg.empty())
	{
		forceBackground(img);
		//forceBackground(tmp);
	}

	perf::ThreadPool::doWork(&threadTest, &mBg, &img);
}