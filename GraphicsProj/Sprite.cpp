#include "Sprite.h"

using namespace sGL;
using namespace cv;
using namespace cv::ogl;

const GLsizei INDICES_COUNT = 6;
const GLsizei VERTICES_COUNT = 4;

Sprite::Sprite(UINT width, UINT height) : mProgram(), mWidth(width), mHeight(height)
{
	mTex = new Texture2D();

	mVertices = new Vertices(this, VERTICES_COUNT, INDICES_COUNT, GL_TRIANGLES);

	setupIndices();
	setupTexRegions();
	setupVertices();
}

Sprite::~Sprite()
{
	if (mVertices)
	{
		delete mVertices;
		mVertices = NULL;
	}

	if (mTex)
	{
		delete mTex;
		mTex = NULL;
	}
}

void Sprite::setTexture(Mat& img)
{
	mTex->copyFrom(img);
}

void Sprite::setTexture(Texture2D *tex)
{
	if (!tex || mTex == tex)
	{
		return;
	}

	if (mTex)
	{
		delete mTex;
	}

	mTex = tex;
}

void Sprite::setupIndices()
{
	unsigned short *indices = new unsigned short[INDICES_COUNT];

	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 2;
	
	mVertices->setIndicesBuffer(indices);

	delete[] indices;
}

void Sprite::setupTexCoords(TextureRegion& region)
{
	float * coords = new float[VERTICES_COUNT * TEXCOORD_CNT];

	coords[0] = region.u1;        // Add U for Vertex 0
    coords[1] = region.v1;        // Add V for Vertex 0
    coords[2] = region.u2;        // Add U for Vertex 1
    coords[3] = region.v1;        // Add V for Vertex 1
    coords[4] = region.u2;        // Add U for Vertex 2
    coords[5] = region.v2;        // Add V for Vertex 2
    coords[6] = region.u1;        // Add U for Vertex 3
    coords[7] = region.v2; 

	mVertices->setTexCoords(coords);

	delete[] coords;
}

void Sprite::setupVertices()
{
	float *vertexMatrix = new float[VERTICES_COUNT * POSITION_CNT_2D];

	vertexMatrix[0] = mWidth / -2.0f;
	vertexMatrix[1] = mHeight / 2.0f;

	vertexMatrix[2] = mWidth / 2.0f;
	vertexMatrix[3] = mHeight / 2.0f;

	vertexMatrix[4] = mWidth / 2.0f;
	vertexMatrix[5] = mHeight / -2.0f;

	vertexMatrix[6] = mWidth / -2.0f;
	vertexMatrix[7] = mHeight / -2.0f;

	mVertices->setVertices(vertexMatrix);

	delete[] vertexMatrix;
}

void Sprite::setupTexRegions()
{
	TextureRegion region = TextureRegion((float)mWidth, (float)mHeight, 0.0f, 0.0f, (float)mWidth, (float)mHeight);

	setupTexCoords(region);
}

void Sprite::draw(Matrix& vpMatrix) const 
{
	// Ignore model matrix for the moment
	mVertices->draw(vpMatrix);
}