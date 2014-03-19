#include "Util.h"
#include <cmath>

using namespace proj;
using namespace cv;

void proj::chromaKey(Mat& img, Colour& colour)
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
    }

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

void proj::blur(Mat& img, uint passes)
{
	cv::blur(img, img, Size(3, 3), Point(-1, -1));

	if (passes > 1)
	{
		blur(img, passes - 1);
	}
}

vector<Rect> proj::sortRect(vector<Rect>& arr, int maxsort)
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

Point proj::centreRect(Rect& rect)
{
	return Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
}

int proj::distDiff(Point x, Point y)
{
	return (int)sqrt(pow(abs(y.x - x.x), 2) + pow(abs(x.y - y.y), 2));
}