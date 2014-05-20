#include "Vertices.h"

using namespace sGL;
using namespace std;

const int MVP_MATRIX_INDEX_CNT = 1;

Vertices::Vertices(const Sprite& sprite, GLsizei numVertices, GLsizei numIndices, GLenum primitives) : 
	mShape(sprite),
	mProgram(sprite.getProgram()),

	mUsesColour(mProgram.usesColour()),
	mUsesTexture(mProgram.usesTexture()),
	mUsesTexCoords(mProgram.usesTexCoords()),
	mUsesMVPIndex(mProgram.usesMVPIndex()),
	mUsesAlpha(mProgram.usesAlpha()),

	mPositionCount(POSITION_CNT_2D),
	mVertexStride(mPositionCount + (mUsesMVPIndex ? MVP_MATRIX_INDEX_CNT : 0) + (mUsesTexCoords ? TEXCOORD_CNT : 0)),
	mVertexSize(mVertexStride * sizeof(float)),

	mPrimitiveType(primitives),
	mIndicesCount(numIndices),
	mVerticesCount(numVertices),

	mPositionHandle(mProgram.getPositionHandle()), 
	mTexCoordHandle(mProgram.getTexCoordHandle()),

	mVertexBuffer(new float[mPositionCount * mVerticesCount]),
	mTexCoordsBuffer(mUsesTexCoords ? new float[TEXCOORD_CNT * mVerticesCount] :
#ifdef __CPP11
	nullptr
#else
	NULL
#endif
	),
	mIndicesBuffer((mIndicesCount > 0) ? new ushort[numIndices] :
#ifdef __CPP11
	nullptr
#else
	NULL
#endif
	)
{

}

Vertices::~Vertices()
{
#ifndef __CPP11
	if (mVertexBuffer)
	{
		delete[] mVertexBuffer;
		mVertexBuffer = NULL;
	}

	if (mIndicesBuffer)
	{
		delete[] mIndicesBuffer;
		mIndicesBuffer = NULL;
	}

	if (mTexCoordsBuffer)
	{
		delete[] mTexCoordsBuffer;
		mTexCoordsBuffer = NULL;
	}
#endif
}

void Vertices::bind(Matrix& mvpMatrix)
{
	glUseProgram(mProgram.getHandle());

	GLint handle = glGetUniformLocation(mProgram.getHandle(), "u_MVPMatrix");
	glUniformMatrix4fv(handle, 1, GL_FALSE, mvpMatrix.getPointer());
	checkGLError("MVPMatrix"); 

	glVertexAttribPointer(mPositionHandle, mPositionCount, GL_FLOAT, GL_FALSE, 0,
		mVertexBuffer
#ifdef __CPP11
		.get()
#endif
		);
	glEnableVertexAttribArray(mPositionHandle);
	checkGLError("Vertices");

	if (mUsesColour)
	{
		// Not used at the moment
	}

	if (mUsesTexture)
	{
		handle = glGetUniformLocation(mProgram.getHandle(), "u_Texture");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mShape.getTextureId());
		glUniform1i(handle, 0);
		checkGLError("Textures");
	}

	if (mUsesTexCoords)
	{
		glVertexAttribPointer(mTexCoordHandle, TEXCOORD_CNT, GL_FLOAT, false, 0, 
			mTexCoordsBuffer
#ifdef __CPP11
			.get()
#endif
			);
		glEnableVertexAttribArray(mTexCoordHandle);
		checkGLError("Tex Coords");
	}

	if (mUsesAlpha)
	{
		// Not used at the moment
		handle = glGetUniformLocation(mProgram.getHandle(), "u_Alpha");
		glUniform1f(handle, 1.0f);
	}

	if (mUsesMVPIndex)
	{
		// Not used at the moment
	}
}

void Vertices::draw(Matrix& mvpMatrix)
{
	bind(mvpMatrix);

	if (mIndicesBuffer)
	{
		glDrawElements(mPrimitiveType, mIndicesCount, GL_UNSIGNED_SHORT, 
			mIndicesBuffer
#ifdef __CPP11
			.get()
#endif
			);
		checkGLError("Draw");
	}
	else
	{
		glDrawArrays(mPrimitiveType, 0, mVerticesCount);
	}
}

void Vertices::setIndicesBuffer(
#ifdef __CPP11
	unique_ptr<ushort[]>
#else 
	ushort* 
#endif
	buff)
{
	if (!mIndicesBuffer || !buff)
	{
		return;
	}

#ifdef __CPP11
	mIndicesBuffer = move(buff);
#else
	for (GLsizei i = 0; i < mIndicesCount; i++)
	{
		mIndicesBuffer[i] = buff[i];
	}
#endif
}

void Vertices::setTexCoords(
#ifdef __CPP11
	unique_ptr<float[]>
#else
	float *
#endif
	buff)
{
	if (!mUsesTexCoords || !buff)
	{
		return;
	}

#ifdef __CPP11
	mTexCoordsBuffer = move(buff);
#else
	for (int i = 0; i < TEXCOORD_CNT * mVerticesCount; i++)
	{
		mTexCoordsBuffer[i] = buff[i];
	}
#endif
}

void Vertices::setVertices(
#ifdef __CPP11
	unique_ptr<float[]>
#else
	float *
#endif
	buff)
{
	if (!buff)
	{
		return;
	}

#ifdef __CPP11
	mVertexBuffer = move(buff);
#else
	for (int i = 0; i < mPositionCount * mVerticesCount; i++)
	{
		mVertexBuffer[i] = buff[i];
	}
#endif
}