#include "TextureRegion.h"

using namespace sGL;

TextureRegion::TextureRegion(float texWidth, float texHeight, float x, float y, float width, float height) :
	u1(x / texWidth), v1(y / texHeight), u2(u1 + (width / texWidth)), v2(v1 + (height / texHeight))
{

}