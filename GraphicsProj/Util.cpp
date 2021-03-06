#include <cmath>

#include "Util.h"
#include "Threading.h"

using namespace proj;
using namespace cv;
using namespace std;

#define max(x, y) ((x > y) ? x : y)

template<typename T>
void segBlueDiff WORKER_ARGS(threadNo, numThreads, pimg, pmod,,)
{
	const Mat * img = (const Mat *)pimg;
	Mat * mod = (Mat *)pmod;

	static const int channels = img->channels();
	static const int rows = img->rows;
	static const int cols = img->cols * channels;

	int tcount = rows / numThreads;
	int start = (int)threadNo * tcount;
	tcount += start;

	const T* i;
	T* m;

	if (img->isContinuous() && mod->isContinuous())
	{
		start *= img->cols;
		tcount *= img->cols;

		i = img->data + (start * channels);
		m = mod->data + (start * channels);

		for (; start < tcount; ++start, i += channels, m += channels)
		{
			if (*i > *(i + 1))
			{
				*m = *(m + 1);
			}
		}
	}
	else
	{
		for (; start < tcount; ++start)
		{
			i = img->ptr<T>(start);
			m = mod->ptr<T>(start);

			for (int c = 0; c < cols; c += channels)
			{
				if (i[c] > i[c + 1])
				{
					m[c] = m[c + 1];
				}
			}
		}
	}
}

template<typename T>
void blueDiff(const Mat& img, Mat& mod)
{
	CV_DbgAssert(img.channels() >= 3);
	CV_DbgAssert(img.depth() == mod.depth());
	CV_DbgAssert(img.channels() == mod.channels());
	CV_DbgAssert(img.cols == mod.cols && img.rows == mod.rows);

	perf::ThreadPool::doWork(&segBlueDiff<uchar>, (void *)&img, &mod);

	/* int channels = img.channels();
	int rows = img.rows;
	int cols = img.cols * channels;

	for (int i = 0; i < rows; i++)
	{
		const T* p = img.ptr<T>(i);
		T* q = mod.ptr<T>(i);

		for (int j = 0; j < cols; j += channels)
		{
			if (p[j] > p[j + 1])
			{
				q[j] = p[j + 1];
			}
		}
	} */
}

template<typename T>
void greenDiff(const Mat& img, Mat& mod)
{
	int channels = img.channels();
	int rows = img.rows;
	int cols = img.cols * channels;

	CV_DbgAssert(channels >= 3);
	CV_DbgAssert(img.depth() == mod.depth());
	CV_DbgAssert(img.channels() == mod.channels());
	CV_DbgAssert(img.cols == mod.cols && img.rows == mod.rows);

	for (int i = 0; i < rows; i++)
	{
		const T* p = img.ptr<T>(i);
		T* q = mod.ptr<T>(i);

		for (int j = 0; j < cols; j += channels)
		{
			if (p[j + 1] > p[j])
			{
				q[j + 1] = p[j];
			}
		}
	}
}

template<typename T>
void segBlueMask WORKER_ARGS(threadNo, numThreads, pimg, pmask,,)
{
	const Mat * img = (const Mat *)pimg;
	Mat * mask = (Mat *)pmask;

	static const int channels = img->channels();
	static const int rows = img->rows;
	static const int cols = img->cols * channels;

	int tcount = rows / numThreads;
	int start = (int)threadNo * tcount;
	tcount += start;

	T* m;
	const T* i;

	if (img->isContinuous() && mask->isContinuous())
	{
		start *= img->cols;
		tcount *= img->cols;

		m = mask->data + start;
		i = img->data + (start * channels);

		for (; start < tcount; ++start, i += channels, ++m)
		{
			T mx = max(*(i + 1), *(i + 2));
			*m = (mx > *i) ? 0 : *i - mx;
		}
	}
	else
	{
		for (int r = start; r < tcount; r++)
		{
			m = mask->ptr<T>(r);
			i = img->ptr<T>(r);

			for (int c = 0, k = 0; c < cols; c += channels, ++k)
			{
				T mx = max(i[c + 1], i[c + 2]);
				m[k] = (mx > i[c]) ? 0 : i[c] - mx;
			}
		}
	}
}

template<typename T>
void blueMask(const Mat& img, Mat& mask)
{
	CV_DbgAssert(img.channels() >= 3);
	CV_DbgAssert(img.rows == mask.rows && img.cols == mask.cols);
	CV_DbgAssert(mask.channels() == 1);
	
	perf::ThreadPool::doWork(&segBlueMask<T>, (void *)&img, &mask);
}

template<typename T>
void greenMask(const Mat& img, Mat& mask)
{
	int channels = img.channels();
	int rows = img.rows;
	int cols = img.cols * channels;

	CV_DbgAssert(channels >= 3);
	CV_DbgAssert(rows == mask.rows && img.cols == mask.cols);
	CV_DbgAssert(mask.channels() == 1);
	CV_DbgAssert(img.depth() == mask.depth());

	for (int i = 0; i < rows; i++)
	{
		const T* p = img.ptr<T>(i);
		T* q = mask.ptr<T>(i);

		for (int j = 0, k = 0; j < cols; j += channels, k++)
		{
			T mx = max(p[j], p[j + 2]);
			q[k] = (mx > p[j + 1]) ? 0 : p[j + 1] - mx;
		}
	}
}

void proj::rgbKey(Mat& img, int32_t colour)
{
	if (!img.data)
	{
		return;
	}

	Mat msk;
	rgbKey(img, msk, colour);
}

void proj::rgbKey(const Mat& img, Mat& _mask, int32_t colour, bool invert)
{
	Mat mod = img;
	rgbKey(mod, _mask, colour, invert);
}

void proj::rgbKey(Mat& img, Mat& _mask, int32_t colour, bool _invert, bool _DoMask)
{
	if (!img.data)
	{
		return;
	}

	//Mat imgmod = img.clone();
	_mask.create(img.rows, img.cols, CV_8UC1);

	switch (colour)
	{
	case Colour::BLUE:
		//blueDiff<uchar>(img, imgmod);
		blueMask<uchar>(img, _mask);
		break;
	case Colour::GREEN:
		//greenDiff<uchar>(img, imgmod);
		greenMask<uchar>(img, _mask);
		break;
	default:
		return;
	}

	if (_invert)
	{
		invert<uchar>(_mask, 255);
	}
	if (_DoMask)
	{
		mask<uchar>(img, img, _mask, 255);
	}
}

void proj::chromaKey(Mat& img, const Colour& colour)
{
	if (!img.data)
	{
		return;
	}

	int channels = img.channels();

    int nRows = img.rows;
    int nCols = img.cols * channels;

    if (img.isContinuous())
    {
        nCols *= nRows;
        nRows = 1;

		uchar* p = img.data;
		for (int i = 0; i < nCols; i += channels, p += channels)
		{
			if (abs(p[0] - colour.getBlue()) > MAX_DIFF || abs(p[1] - colour.getGreen()) > MAX_DIFF || abs(p[2] - colour.getRed()) > MAX_DIFF)
			{
				p[0] = 0;
				p[1] = 0;
				p[2] = 0;
			}
			else
			{
				p[0] = 255;
				p[1] = 255;
				p[2] = 255;
			}
		}
    }
	else
	{

    int i,j;
    uchar* p;
    for(i = 0; i < nRows; i++)
    {
        p = img.ptr<uchar>(i);
        for (j = 0; j < nCols; j += channels)
        {
			if (abs(p[j] - colour.getBlue()) > MAX_DIFF || abs(p[j + 1] - colour.getGreen()) > MAX_DIFF || abs(p[j + 2] - colour.getRed()) > MAX_DIFF)
			{
				p[j] = 0;
				p[j + 1] = 0;
				p[j + 2] = 0;
			}
			else
			{
				p[j] = 255;
				p[j + 1] = 255;
				p[j + 2] = 255;
			}
        }
    }
	}
}
/*
void proj::blur(Mat& img, uint passes)
{
	cv::blur(img, img, Size(3, 3), Point(-1, -1));

	if (passes > 1)
	{
		blur(img, passes - 1);
	}
}
*/

template<typename T>
void proj::invert(Mat& img, T max)
{
	int channels = img.channels();

	int nRows = img.rows;
	int nCols = img.cols * channels;

	if (img.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}

	for (int i = 0; i < nRows; i++)
	{
		T *p = img.ptr<T>(i);

		for (int j = 0; j < nCols; j += channels)
		{
			p[j] = max - p[j];
		}
	}
}

template<typename T>
void segMask WORKER_ARGS(threadNo, numThreads, pimg, pout, pmask, pmax)
{
	T max = *(T *)pmax;
	const Mat * img = (const Mat *)pimg;
	Mat * out = (Mat *)pout;
	const Mat * mask = (const Mat *)pmask;

	static const int cols = img->cols;
	static const int rows = img->rows;
	static const int channels = img->channels();

	int tcount = numThreads / rows;
	int start = (int)threadNo * tcount;
	tcount += start;

	const T * i;
	T * o;
	const T * m;

	if (img->isContinuous() && out->isContinuous() && mask->isContinuous())
	{
		start *= cols;
		tcount *= cols;

		i = img->data + (start * channels);
		o = out->data + (start * channels);
		m = mask->data + start;

		for (; start < tcount; ++start, ++m)
		{
			for (int k = 0; k < channels; ++k, ++o, ++i)
			{
				*o = *i * *m / max;
			}
		}
	}
	else
	{
		for (int r = 0; r < rows; r++)
		{
			i = img->ptr<T>(r);
			m = mask->ptr<T>(r);
			o = out->ptr<T>(r);

			for (int k = 0; k < cols; k++)
			{
				for (int j = 0; j < channels; j++)
				{
					int index = (k * channels) + j;
					int val = i[index] * m[k] / max;
					o[index] = (T)val;
				}
			}
		}
	}
}

template<typename T>
void proj::mask(const Mat& img, Mat& out, const Mat& mask, T max)
{
	CV_DbgAssert(img.depth() == mask.depth() && out.depth() == img.depth());
	CV_DbgAssert(img.channels() == out.channels());
	CV_DbgAssert(mask.channels() == 1);
	CV_DbgAssert(img.cols == mask.cols && img.cols == out.cols && img.rows == mask.rows && img.rows == out.rows);

	static const uchar * mx = new uchar(255);

	perf::ThreadPool::doWork(&segMask<uchar>, (void *)&img, &out, (void *)&mask, (void *)mx);

	/* int nCols = img.cols;
	int nRows = img.rows;

	int mkCols = mask.cols;
	int mkRows = mask.rows;

	CV_DbgAssert(nCols == mkCols && nRows == mkRows && nCols == out.cols && nRows == out.rows);

	int channels = img.channels();

	//nCols *= channels;

	for (int i = 0; i < nRows; i++)
	{
		const T *p = img.ptr<T>(i);
		const T *q = mask.ptr<T>(i);
		T *o = out.ptr<T>(i);

		for (int k = 0; k < nCols; k++)
		{
			for (int j = 0; j < channels; j++)
			{
				int index = (k * channels) + j;
				int val = p[index] * q[k] / max;
				o[(k * channels) + j] = (T)val;
			}
		}
	} */
}

vector<Rect> proj::sortRect(const vector<Rect>& arr, int maxsort)
{
	int arrSize = (int)arr.size();
	if (arrSize < maxsort)
	{
		maxsort = arrSize;
	}

	vector<int> areas(arrSize);
	for (int i = 0; i < arrSize; i++)
	{
		areas[i] = arr[i].area();
	}

	vector<Rect> out(maxsort);
	vector<int> indices(maxsort);

	
	for (int i = 0; i < maxsort; i++)
	{
		int index = 0;
		int maxArea = 0;
		for (int j = 0; j < arrSize; j++)
		{
			bool found = false;
			for (int k = 0; k < i; k++)
			{
				if (indices[k] == j)
				{
					found = true;
					break;
				}
			}

			if (!found && areas[j] > maxArea)
			{
				index = j;
				maxArea = areas[j];
			}
		}

		out[i] = arr[index];
		indices[i] = index;
	}

	return out;
}

Point proj::centreRect(const Rect& rect)
{
	return Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
}

int proj::distDiff(const Point& x, const Point& y)
{
	return (int)sqrt(pow(abs(y.x - x.x), 2) + pow(abs(x.y - y.y), 2));
}

void proj::drawText(cv::Mat& img, const std::string& text, const cv::Point& org, double fontScale, const cv::Scalar& colour, bool isCentred)
{
	int baseline = 0;
	Size textSize = getTextSize(text, cv::FONT_HERSHEY_COMPLEX, fontScale, 3, &baseline);

	Point textOrg;
	if (isCentred)
	{
		textOrg = Point(org.x - (textSize.width / 2), org.y + (textSize.height / 2));
	}
	else
	{
		textOrg = Point(org.x, org.y + textSize.height);
	}

	putText(img, text, textOrg, cv::FONT_HERSHEY_COMPLEX, fontScale, colour, 3);
}

void proj::backgroundDiff(const Mat& bg, Mat& img)
{
	img = img - bg;
}

#ifdef _DEBUG

bool displayed = false;
void debugDisplayImage(Mat& img)
{
	if (!displayed)
	{
		namedWindow("Debug");
		displayed = true;
	}

	imshow("Debug", img);
	waitKey(0);
}

#endif
