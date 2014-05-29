#include "BGround.h"
#include "Colour.h"
#include "Threading.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace proj;
using namespace cv;

BackGround::BackGround() : 
	mBg(),
	mFramesSinceBG(0),
	mEntropy()
{

}

void BackGround::forceBackground(const Mat& newBg)
{
	mBg = newBg.clone();
	mFramesSinceBG = 0;
	mEntropy = Mat::zeros(mBg.rows, mBg.cols, CV_8UC4);
}

#define MIN_DIFF 20

void segmentDiff(uchar threadNo, uchar threadNums, void *pbg, void *ptmp, void *, void *)
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

#define XBLOCK_SIZE 5
#define YBLOCK_SIZE 5
#define MAX_ENTROPY 10
#define MAX_DIFF 120
#define ENTROPY_CHANNELS 4

const int DIFF_INTERVAL = MAX_DIFF - MIN_DIFF;

void segUpdateBG(uchar threadNo, uchar threadNums, void *pbg, void *pimg, void *pdest, void *pentropy)
{
	Mat * bg = (Mat *)pbg;
	Mat * img = (Mat *)pimg;
	Mat * dest = (Mat *)pdest;
	Mat * entropy = (Mat *)pentropy;

	static int rows = img->rows;
	static int channels = img->channels();
	static int cols = img->cols * channels;
	static int ecols = entropy->cols * ENTROPY_CHANNELS;
	uchar *p;
	uchar *q;
	uchar *r;
	uchar *e;

	int tcount = rows / threadNums;
	int start = (int)threadNo * tcount;
	tcount += start;

	if (bg->isContinuous() && img->isContinuous() && dest->isContinuous() && entropy->isContinuous())
	{
		p = bg->data + (start * cols);
		q = img->data + (start * cols);
		r = dest->data + (start * cols);
		e = entropy->data + (start * ecols);

		uchar * ptmp, * prow;
		uchar * qtmp, * qrow;
		uchar * rtmp, * rrow;
		uchar * etmp, * erow;

		for (int row = start; row < tcount; row += YBLOCK_SIZE, p += YBLOCK_SIZE * cols, q += YBLOCK_SIZE * cols, r += YBLOCK_SIZE * cols, e += YBLOCK_SIZE * ecols)
		{
			prow = p;
			qrow = q;
			rrow = r;
			erow = e;

			for (int col = 0; (channels * col) < cols; 
				col += XBLOCK_SIZE, prow += XBLOCK_SIZE * channels, qrow += XBLOCK_SIZE * channels, rrow += XBLOCK_SIZE * channels, erow += XBLOCK_SIZE * ENTROPY_CHANNELS)
			{
				int ave = 0;
				uint ind = 0;

				for (int y = 0; (y < YBLOCK_SIZE) && (row + y < rows); y++)
				{
					ptmp = prow + (y * cols);
					qtmp = qrow + (y * cols);

					for (int x = 0; (x < XBLOCK_SIZE) && ((channels * (col + x)) < cols); x++, ptmp += channels, qtmp += channels)
					{
						updateRollingAve(ave, ind, Colour::diff(ptmp, qtmp));

						updateWeightedAve<uchar>(ptmp[0], 10, qtmp[0]);
						updateWeightedAve<uchar>(ptmp[1], 10, qtmp[1]);
						updateWeightedAve<uchar>(ptmp[2], 10, qtmp[2]);
					}
				}

				if (ave <= MIN_DIFF)
				{
					for (int y = 0; (y < YBLOCK_SIZE) && (row + y < rows); y++)
					{
						rtmp = rrow + (y * cols);
						etmp = erow + (y * ecols);

						// Note: An optimisation can be made since entropy need only be as many blocks as there are. Saves on space plus repeated branching.
						for (int x = 0; (x < XBLOCK_SIZE) && ((channels * (col + x)) < cols); x++, rtmp += channels, etmp += ENTROPY_CHANNELS)
						{
							rtmp[0] = 0;
							rtmp[1] = 0;
							rtmp[2] = 0;

							SAFE_DEC(etmp[3]);
						}
					}
				}
				else if (ave >= MAX_DIFF)
				{
					for (int y = 0; (y < YBLOCK_SIZE) && (row + y < rows); y++)
					{
						ptmp = prow + (y * cols);
						qtmp = qrow + (y * cols);
						etmp = erow + (y * ecols);

						for (int x = 0; (x < XBLOCK_SIZE) && ((channels * (col + x)) < cols); x++, ptmp += channels, qtmp += channels, etmp += ENTROPY_CHANNELS)
						{
							if (etmp[3] >= MAX_ENTROPY)
							{
								etmp[3] = 0;

								ptmp[0] = qtmp[0];
								ptmp[1] = qtmp[1];
								ptmp[2] = qtmp[2];
							}
							else
							{
								etmp[3] += 2;
							}
						}
					}
				}
				else
				{
					const uchar avediff = ave - MIN_DIFF;

					for (int y = 0; (y < YBLOCK_SIZE) && (row + y < rows); y++)
					{
						ptmp = prow + (y * cols);
						qtmp = qrow + (y * cols);
						rtmp = rrow + (y * cols);
						etmp = erow + (y * ecols);

						for (int x = 0; (x < XBLOCK_SIZE) && ((channels * (col + x)) < cols); x++, ptmp += channels, qtmp += channels, rtmp += channels, etmp += ENTROPY_CHANNELS)
						{
							rtmp[0] = (qtmp[0] * avediff) / DIFF_INTERVAL;
							rtmp[1] = (qtmp[1] * avediff) / DIFF_INTERVAL;
							rtmp[2] = (qtmp[2] * avediff) / DIFF_INTERVAL;

							if (etmp[3] >= MAX_ENTROPY)
							{
								etmp[3] = 0;

								ptmp[0] = qtmp[0];
								ptmp[1] = qtmp[1];
								ptmp[2] = qtmp[2];
							}
							else
							{
								etmp[3]++;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for (int i = start; i < tcount; i++)
		{
			p = img->ptr<uchar>(i);
			q = bg->ptr<uchar>(i);

			for (int j = 0; j < cols; j += channels, p += channels, q += channels)
			{
				// later
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

	Mat tmp = img.clone();

	//perf::ThreadPool::doWork(&segmentDiff, &mBg, &img);
	perf::ThreadPool::doWork(&segUpdateBG, &mBg, &tmp, &img, &mEntropy);
}