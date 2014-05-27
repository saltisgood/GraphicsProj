#include "Colour.h"
#include <cmath>

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
		mScalar = new Scalar_<uchar>(mB, mG, mR);
	}

	return *mScalar;
}

ushort Colour::diff(const Colour& c2) const
{
	return (ushort)sqrt<ushort>((ushort)(pow<uchar, uchar>(mR - c2.mR, 2) + pow<uchar, uchar>(mG - c2.mG, 2) + pow<uchar, uchar>(mB - c2.mB, 2)));
}

ushort Colour::diff(const Scalar_<uchar>& c1, const Scalar_<uchar>& c2)
{
	//return (ushort)sqrt<ushort>((ushort)(pow<uchar, uchar>(c1[0] - c2[0], 2) + pow<uchar, uchar>(c1[1] - c2[1], 2) + pow<uchar, uchar>(c1[2] - c2[2], 2)));
	//return (ushort)sqrt<ushort>((ushort)(((c1[0] - c2[0]) * (c1[0] - c2[0])) + ((c1[1] - c2[1]) * (c1[1] - c2[1])) + ((c1[2] - c2[2]) * (c1[2] - c2[2]))));
	//return (ushort)sqrt<ushort>((ushort)(((c1[0] - c2[0]) << 2) + ((c1[1] - c2[1]) << 2) + ((c1[2] - c2[2]) << 2)));
	return (ushort)((ushort)(((c1[0] - c2[0]) << 2) + ((c1[1] - c2[1]) << 2) + ((c1[2] - c2[2]) << 2)));
}

ushort Colour::diff(const uchar* c1, const uchar* c2)
{
	return (ushort)sqrt<ushort>((ushort)(pow<short, uchar>(c1[0] - c2[0], 2) + pow<short, uchar>(c1[1] - c2[1], 2) + pow<short, uchar>(c1[2] - c2[2], 2)));
}