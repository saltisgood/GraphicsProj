#ifndef TEXTURE_REGION_H_
#define TEXTURE_REGION_H_

namespace sGL
{
	class TextureRegion
	{
	public:
		TextureRegion(float texWidth, float texHeight, float x, float y, float width, float height);

		const float u1;
		const float u2;
		const float v1;
		const float v2;
	};
}

#endif