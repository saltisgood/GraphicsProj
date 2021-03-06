#include "BGround.h"
#include "Colour.h"
#include "Threading.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace proj;
using namespace cv;

BackGround::BackGround() : 
	mBg(),
	mEntropy(),
	mReplacement()
{
	mReplacement = imread("rainbow.jpg");
}

void BackGround::forceBackground(const Mat& newBg)
{
	mBg = newBg.clone();
	mEntropy = Mat::zeros(mBg.rows, mBg.cols, CV_8UC1);
}

#define MIN_DIFF 30
#define XBLOCK_SIZE 3
#define YBLOCK_SIZE 3
#define MAX_ENTROPY 30
#define MAX_DIFF 100
#define ENTROPY_CHANNELS 1

const int DIFF_INTERVAL = MAX_DIFF - MIN_DIFF;

void segUpdateBG WORKER_ARGS(threadNo, threadNums, pbg, pimg, pentropy,)
{
	Mat * bg = (Mat *)pbg;
	const Mat * img = (Mat *)pimg;
	Mat * entropy = (Mat *)pentropy;

	static const int rows = img->rows;
	static const int channels = img->channels();
	static const int cols = img->cols * channels;
	static const int ecols = entropy->cols * ENTROPY_CHANNELS;
	static const int ediff = ENTROPY_CHANNELS - channels;
	uchar *b;
	const uchar *im;
	uchar *e;

	int tcount = rows / threadNums;
	int start = (int)threadNo * tcount;
	tcount += start;

	if (bg->isContinuous() && img->isContinuous() && entropy->isContinuous())
	{
		b = bg->data + (start * cols);
		im = img->data + (start * cols);
		e = entropy->data + (start * ecols);

		uchar * btmp, * brow;
		const uchar * imtmp, * imrow;
		uchar * etmp, * erow;

		for (int row = start; row < tcount; row += YBLOCK_SIZE, b += YBLOCK_SIZE * cols, im += YBLOCK_SIZE * cols, e += YBLOCK_SIZE * ecols)
		{
			brow = b;
			imrow = im;
			erow = e;

			for (int col = 0; (channels * col) < cols; 
				col += XBLOCK_SIZE, brow += XBLOCK_SIZE * channels, imrow += XBLOCK_SIZE * channels, erow += XBLOCK_SIZE * ENTROPY_CHANNELS)
			{
				int ave = 0;
				uint ind = 0;

				for (int y = 0; (y < YBLOCK_SIZE) && (row + y < rows); y++)
				{
					btmp = brow + (y * cols);
					imtmp = imrow + (y * cols);

					for (int x = 0; (x < XBLOCK_SIZE) && ((channels * (col + x)) < cols); x++, btmp += channels, imtmp += channels)
					{
						updateRollingAve(ave, ind, Colour::diff(btmp, imtmp));
					}
				}

				if (ave <= MIN_DIFF)
				{
					for (int y = 0; (y < YBLOCK_SIZE) && (row + y < rows); y++)
					{
						btmp = brow + (y * cols);
						imtmp = imrow + (y * cols);
						etmp = erow + (y * ecols);

						for (int x = 0; (x < XBLOCK_SIZE) && ((channels * (col + x)) < cols); x++, ++etmp)
						{
							for (int c = 0; c < channels; ++c, ++btmp, ++imtmp)
							{
								updateWeightedAve<uchar>(*btmp, 7, *imtmp);
							}

							*etmp = 0;
						}
					}
				}
				else if (ave >= MAX_DIFF)
				{
					for (int y = 0; (y < YBLOCK_SIZE) && (row + y < rows); y++)
					{
						etmp = erow + (y * ecols);

						for (int x = 0; (x < XBLOCK_SIZE) && ((channels * (col + x)) < cols); ++x, ++etmp)
						{
							*etmp = 255;
						}
					}
				}
				else
				{
					//const uchar avediff = ave - MIN_DIFF;
					const uchar avediff = (255 * (ave - MIN_DIFF)) / DIFF_INTERVAL;

					for (int y = 0; (y < YBLOCK_SIZE) && (row + y < rows); y++)
					{
						etmp = erow + (y * ecols);

						for (int x = 0; (x < XBLOCK_SIZE) && ((channels * (col + x)) < cols); x++,  ++etmp)
						{
							*etmp = avediff;
						}
					}
				}
			}
		}
	}
}

void BackGround::extractForeground(const Mat& img)
{
	//Mat tmp;
	//resize(img, tmp, Size(), 0.5, 0.5, CV_INTER_AREA);

	if (mBg.empty())
	{
		forceBackground(img);
		//forceBackground(tmp);
	}

	CV_DbgAssert(mBg.isContinuous());
	CV_DbgAssert(img.isContinuous());
	CV_DbgAssert(mEntropy.isContinuous());

	perf::ThreadPool::doWork(&segUpdateBG, &mBg, (void *)&img, &mEntropy);

	static const Mat kernel = getStructuringElement(MORPH_RECT, Size(15, 15));
	static const Point pt(-1, -1);
	morphologyEx(mEntropy, mEntropy, MORPH_CLOSE, kernel, pt, 3);
}

void segComposite WORKER_ARGS(threadNo, threadNums, pimg, preplacement, pentropy,)
{
	const Mat * replacement = (Mat *)preplacement;
	Mat * img = (Mat *)pimg;
	const Mat * entropy = (Mat *)pentropy;

	static const int rows = img->rows;
	static const int channels = img->channels();
	static const int cols = img->cols * channels;
	static const int ecols = entropy->cols * ENTROPY_CHANNELS;
	static const int ediff = ENTROPY_CHANNELS - channels;
	const uchar *r;
	uchar *i;
	const uchar *e;

	int tcount = rows / threadNums;
	int start = (int)threadNo * tcount;
	tcount += start;

	// Assume for now they're all continuous
	r = replacement->data + (start * cols);
	i = img->data + (start * cols);
	e = entropy->data + (start * ecols);

	start *= img->cols;
	tcount *= img->cols;

	for (; start < tcount; start++, ++e)
	{
		for (int j = 0; j < channels; ++j, ++i, ++r)
		{
			*i =  ((*i * *e) / UCHAR_MAX) + ((*r * (UCHAR_MAX - *e)) / UCHAR_MAX);
		}
	}
}

void BackGround::composite(Mat& img)
{
	CV_DbgAssert(img.isContinuous());
	CV_DbgAssert(mReplacement.isContinuous());
	CV_DbgAssert(mEntropy.isContinuous());

	perf::ThreadPool::doWork(&segComposite, &img, &mReplacement, &mEntropy);
}

void segApplyMask WORKER_ARGS(threadNo, numThreads, pimg, pmask,,)
{
	Mat * img = (Mat *)pimg;
	const Mat * mask = (Mat *)pmask;

	static const int rows = img->rows;
	static const int channels = img->channels();
	static const int cols = img->cols * channels;
	static const int ecols = mask->cols * ENTROPY_CHANNELS;
	uchar *i;
	const uchar *m;

	int tcount = rows / numThreads;
	int start = (int)threadNo * tcount;
	tcount += start;

	// Assume for now they're all continuous
	i = img->data + (start * cols);
	m = mask->data + (start * ecols);

	start *= img->cols;
	tcount *= img->cols;

	for (; start < tcount; start++, ++m)
	{
		for (int j = 0; j < channels; ++j, ++i)
		{
			*i =  ((*i * *m) / UCHAR_MAX);
		}
	}
}

void BackGround::applyMask(cv::Mat& img)
{
	CV_DbgAssert(img.isContinuous());
	CV_DbgAssert(mEntropy.isContinuous());

	perf::ThreadPool::doWork(&segApplyMask, &img, &mEntropy);
}