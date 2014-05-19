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

Scalar Colour::toScalar()
{
	if (!mScalar)
	{
		mScalar = new Scalar(B, G, R);
	}

	return *mScalar;
}