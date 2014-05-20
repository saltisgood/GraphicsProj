#ifndef COLOUR_H_
#define COLOUR_H_

#include <opencv2/core/core.hpp>

namespace proj
{
	class Colour
	{
	public:
		Colour(uchar r, uchar g, uchar b) : mR(r), mG(g), mB(b), mScalar(NULL) { }
		virtual ~Colour();

		uchar getRed() const { return mR; }
		uchar getBlue() const { return mB; }
		uchar getGreen() const { return mG; }

		cv::Scalar toScalar() const;

	protected:
		uchar mR, mG, mB;
		mutable cv::Scalar *mScalar;
	};
}

#endif
