#ifndef COLOUR_H_
#define COLOUR_H_

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

	protected:
		uchar R, G, B;
	};
}

#endif