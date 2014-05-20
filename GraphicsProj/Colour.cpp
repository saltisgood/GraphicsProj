#include "Colour.h"

using namespace proj;
using namespace cv;

Colour::~Colour()
{
	if (mScalar)
	{
		delete mScalar;
		mScalar = NULL;
	}
}

Scalar Colour::toScalar() const
{
	if (!mScalar)
	{
		mScalar = new Scalar(mB, mG, mR);
	}

	return *mScalar;
}