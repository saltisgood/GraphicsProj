#include "Sprite.h"
#include "Vertices.h"

using namespace sGL;
using namespace cv;
using namespace cv::ogl;

const GLsizei INDICES_COUNT = 6;
const GLsizei VERTICES_COUNT = 4;

Sprite::Sprite(uint width, uint height) : mProgram(), mWidth(width), mHeight(height), mTex(new Texture2D()),
	mVertices(new Vertices(*this, VERTICES_COUNT, INDICES_COUNT, GL_TRIANGLES))
{
	setupIndices();
	setupTexRegions();
	setupVertices();
}

Sprite::~Sprite()
{
#ifndef __CPP11
	if (mVertices)
	{
		delete mVertices;
		mVertices = NULL;
	}

	if (mTex)
	{
		mTex->release();
		mTex = NULL;
	}
#endif
}

void Sprite::setTexture(const Mat& img)
{
	mTex->copyFrom(img);
}

#ifdef __CPP11
void Sprite::setTexture(std::shared_ptr<Texture2D> tex)
#else
void Sprite::setTexture(Texture2D *tex)
#endif
{
	if (!tex || mTex == tex)
	{
		return;
	}

#ifndef __CPP11
	if (mTex)
	{
		delete mTex;
	}
#endif

	mTex = tex;
}

void Sprite::setupIndices()
{
#ifdef __CPP11
	std::unique_ptr<ushort[]> indices(new ushort[INDICES_COUNT]);
#else
	ushort * indices = new ushort[INDICES_COUNT];
#endif

	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 2;
	
#ifdef __CPP11
	mVertices->setIndicesBuffer(std::move(indices));
#else
	mVertices->setIndicesBuffer(indices);
	delete[] indices;
#endif
}

void Sprite::setupTexCoords(TextureRegion& region)
{
#ifdef __CPP11
	std::unique_ptr<float[]> coords(new float[VERTICES_COUNT * TEXCOORD_CNT]);
#else
	float * coords = new float[VERTICES_COUNT * TEXCOORD_CNT];
#endif

	coords[0] = region.u1;        // Add U for Vertex 0
    coords[1] = region.v1;        // Add V for Vertex 0
    coords[2] = region.u2;        // Add U for Vertex 1
    coords[3] = region.v1;        // Add V for Vertex 1
    coords[4] = region.u2;        // Add U for Vertex 2
    coords[5] = region.v2;        // Add V for Vertex 2
    coords[6] = region.u1;        // Add U for Vertex 3
    coords[7] = region.v2; 

#ifdef __CPP11
	mVertices->setTexCoords(std::move(coords));
#else
	mVertices->setTexCoords(coords);
	delete[] coords;
#endif
}

void Sprite::setupVertices()
{
#ifdef __CPP11
	std::unique_ptr<float[]> vertexMatrix(new float[VERTICES_COUNT * POSITION_CNT_2D]);
#else
	float * vertexMatrix = new float[VERTICES_COUNT * POSITION_CNT_2D];
#endif

	vertexMatrix[0] = mWidth / -2.0f;
	vertexMatrix[1] = mHeight / 2.0f;

	vertexMatrix[2] = mWidth / 2.0f;
	vertexMatrix[3] = mHeight / 2.0f;

	vertexMatrix[4] = mWidth / 2.0f;
	vertexMatrix[5] = mHeight / -2.0f;

	vertexMatrix[6] = mWidth / -2.0f;
	vertexMatrix[7] = mHeight / -2.0f;

#ifdef __CPP11
	mVertices->setVertices(std::move(vertexMatrix));
#else
	mVertices->setVertices(vertexMatrix);
	delete[] vertexMatrix;
#endif
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