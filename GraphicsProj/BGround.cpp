#include "BGround.h"
#include "Colour.h"
#include "Threading.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace proj;
using namespace cv;

BackGround::BackGround() : 
	mBg(),
	mFramesSinceBG(0),
	mEntropy(),
	mReplacement()
{
	mReplacement = imread("rainbow.jpg");
}

void BackGround::forceBackground(const Mat& newBg)
{
	mBg = newBg.clone();
	mFramesSinceBG = 0;
	mEntropy = Mat::zeros(mBg.rows, mBg.cols, CV_8UC4);
}

#ifdef __C3
	#define MIN_DIFF 30
#else
	#define MIN_DIFF 0
#endif

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

#define XBLOCK_SIZE 3
#define YBLOCK_SIZE 3
#define MAX_ENTROPY 30
#define MAX_DIFF 100
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
#ifdef __C3
						updateRollingAve(ave, ind, Colour::diff(ptmp, qtmp));

						/* updateWeightedAve<uchar>(ptmp[0], 10, qtmp[0]);
						updateWeightedAve<uchar>(ptmp[1], 10, qtmp[1]);
						updateWeightedAve<uchar>(ptmp[2], 10, qtmp[2]); */
#else
						updateRollingAve(ave, ind, abs(((int)(*ptmp)) - ((int)(*qtmp))));

						updateWeightedAve<uchar>(ptmp[0], 10, qtmp[0]);
#endif
					}
				}

				if (ave <= MIN_DIFF)
				{
					for (int y = 0; (y < YBLOCK_SIZE) && (row + y < rows); y++)
					{
						ptmp = prow + (y * cols);
						qtmp = qrow + (y * cols);
						rtmp = rrow + (y * cols);
						etmp = erow + (y * ecols);

						// Note: An optimisation can be made since entropy need only be as many blocks as there are. Saves on space plus repeated branching.
						for (int x = 0; (x < XBLOCK_SIZE) && ((channels * (col + x)) < cols); x++, ptmp += channels, qtmp += channels, rtmp += channels, etmp += ENTROPY_CHANNELS)
						{
							updateWeightedAve<uchar>(ptmp[0], 7, qtmp[0]);
							updateWeightedAve<uchar>(ptmp[1], 7, qtmp[1]);
							updateWeightedAve<uchar>(ptmp[2], 7, qtmp[2]);

							etmp[0] = 0;
							etmp[1] = 0;
							etmp[2] = 0;

							/*
							rtmp[0] = 0;
#ifdef __C3
							rtmp[1] = 0;
							rtmp[2] = 0;
#endif
							*/

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
							etmp[0] = 255;
								etmp[1] = 255;
								etmp[2] = 255;

							if (etmp[3] >= MAX_ENTROPY)
							{
								
								etmp[3] = 0;

								/* updateWeightedAve<uchar>(ptmp[0], 15, qtmp[0]);
								updateWeightedAve<uchar>(ptmp[1], 15, qtmp[1]);
								updateWeightedAve<uchar>(ptmp[2], 15, qtmp[2]); */

								//ptmp[0] = qtmp[0];
#ifdef __C3
								//ptmp[1] = qtmp[1];
								//ptmp[2] = qtmp[2];
#endif
							}
							else
							{
								etmp[3]++;

								/* updateWeightedAve<uchar>(ptmp[0], 15, qtmp[0]);
								updateWeightedAve<uchar>(ptmp[1], 15, qtmp[1]);
								updateWeightedAve<uchar>(ptmp[2], 15, qtmp[2]); */
							}
						}
					}
				}
				else
				{
					//const uchar avediff = ave - MIN_DIFF;
					const uchar avediff = (255 * (ave - MIN_DIFF)) / DIFF_INTERVAL;

					for (int y = 0; (y < YBLOCK_SIZE) && (row + y < rows); y++)
					{
						ptmp = prow + (y * cols);
						qtmp = qrow + (y * cols);
						rtmp = rrow + (y * cols);
						etmp = erow + (y * ecols);

						for (int x = 0; (x < XBLOCK_SIZE) && ((channels * (col + x)) < cols); x++, ptmp += channels, qtmp += channels, rtmp += channels, etmp += ENTROPY_CHANNELS)
						{
							etmp[0] = avediff;
							etmp[1] = avediff;
							etmp[2] = avediff;

							/*rtmp[0] = (qtmp[0] * avediff) / DIFF_INTERVAL;
#ifdef __C3
							rtmp[1] = (qtmp[1] * avediff) / DIFF_INTERVAL;
							rtmp[2] = (qtmp[2] * avediff) / DIFF_INTERVAL;
#endif
							*/

							/* updateWeightedAve<uchar>(ptmp[0], 30, qtmp[0]);
							updateWeightedAve<uchar>(ptmp[1], 30, qtmp[1]);
							updateWeightedAve<uchar>(ptmp[2], 30, qtmp[2]); */

							/* if (etmp[3] >= MAX_ENTROPY)
							{
								etmp[3] = 0;

								ptmp[0] = qtmp[0];
#ifdef __C3
								ptmp[1] = qtmp[1];
								ptmp[2] = qtmp[2];
#endif
							}
							else
							{
								etmp[3]++;
							} */
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

void BackGround::composite(Mat& img) const
{
	static int rows = img.rows;
	static int channels = img.channels();
	static int cols = channels * img.cols;

	uchar * dest;
	const uchar * ori;
	const uchar * ent;

	if (img.isContinuous() && mReplacement.isContinuous() && mEntropy.isContinuous())
	{
		static int dataSize = rows * cols;
		dest = img.data;
		ori = mReplacement.data;
		ent = mEntropy.data;

		for (int i = 0, k = 0; i < dataSize; i += channels, k += ENTROPY_CHANNELS)
		{
			dest[i] = (dest[i] * ent[k]) / 255;
			dest[i + 1] = (dest[i + 1] * ent[k + 1]) / 255;
			dest[i + 2] = (dest[i + 2] * ent[k + 2]) / 255;

			dest[i] += (ori[i] * (255 - ent[k])) / 255;
			dest[i + 1] += (ori[i + 1] * (255 - ent[k + 1])) / 255;
			dest[i + 2] += (ori[i + 2] * (255 - ent[k + 2])) / 255;
		}
	}
}