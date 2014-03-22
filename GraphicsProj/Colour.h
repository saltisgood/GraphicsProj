#ifndef COLOUR_H_
#define COLOUR_H_

#include <opencv2/core/core.hpp>

#include "Util.h"

namespace proj
{
	class Colour
	{
	public:
		Colour(uchar r, uchar g, uchar b)
		{
			R = r;
			G = g;
			B = b;
		}

		virtual ~Colour() {}

		uchar getRed() { return R; }
		uchar getBlue() { return B; }
		uchar getGreen() { return G; }

		cv::Scalar toScalar() { return cv::Scalar(B, G, R); }

	protected:
		uchar R, G, B;
	};
}

#endif
