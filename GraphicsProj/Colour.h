#ifndef COLOUR_H_
#define COLOUR_H_

#include <opencv2/core/core.hpp>

namespace proj
{
	class Colour
	{
	public:
		Colour(uchar r, uchar g, uchar b) : R(r), G(g), B(b)
		{
			mScalar = NULL;
		}

		virtual ~Colour();

		uchar getRed() const { return R; }
		uchar getBlue() const { return B; }
		uchar getGreen() const { return G; }

		cv::Scalar toScalar();

	protected:
		uchar R, G, B;
		cv::Scalar *mScalar;
	};
}

#endif
