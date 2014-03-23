#include "Vertices.h"

using namespace sGL;


const int MVP_MATRIX_INDEX_CNT = 1;


Vertices::Vertices(Sprite *sprite, GLsizei numVertices, GLsizei numIndices, GLenum primitives) : mShape(sprite), mProgram(sprite->getProgram()), mVerticesCount(numVertices), 
	mIndicesCount(numIndices), mPrimitiveType(primitives), mUsesColour(mProgram.usesColour()), mUsesTexture(mProgram.usesTexture()), mUsesTexCoords(mProgram.usesTexCoords()),
	mUsesMVPIndex(mProgram.usesMVPIndex()), mUsesAlpha(mProgram.usesAlpha()), mPositionCount(POSITION_CNT_2D),
	mVertexStride(mPositionCount + (mUsesMVPIndex ? MVP_MATRIX_INDEX_CNT : 0) + (mUsesTexCoords ? TEXCOORD_CNT : 0)),
	mVertexSize(mVertexStride * sizeof(float)), mPositionHandle(mProgram.getPositionHandle()), mTexCoordHandle(mProgram.getTexCoordHandle())
{
	/* mShape = sprite;
	mProgram = sprite->getProgram();

	mUsesColour = mProgram.usesColour();
	mUsesTexture = mProgram.usesTexture();
	mUsesTexCoords = mProgram.usesTexCoords();
	mUsesMVPIndex = mProgram.usesMVPIndex();
	mUsesAlpha = mProgram.usesAlpha();

	mPositionCount = POSITION_CNT_2D;

	mVertexStride = mPositionCount + 
		(mUsesMVPIndex ? MVP_MATRIX_INDEX_CNT : 0) +
		(mUsesTexCoords ? TEXCOORD_CNT : 0);

	mVertexBuffer = new float[mVerticesCount * mVertexStride];

	mVertexSize = mVertexStride * sizeof(float); */

	if (mIndicesCount > 0)
	{
		mIndicesBuffer = new unsigned short[numIndices];
	}
	else
	{
		mIndicesBuffer = NULL;
	}

	mVertexBuffer = new float[mPositionCount * mVerticesCount];

	if (mUsesTexCoords)
	{
		mTexCoordsBuffer = new float[TEXCOORD_CNT * mVerticesCount];
	}
	else
	{
		mTexCoordsBuffer = NULL;
	}
}

Vertices::~Vertices()
{
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
}

void Vertices::bind(Matrix& mvpMatrix)
{
	glUseProgram(mProgram.getHandle());

	GLint handle = glGetUniformLocation(mProgram.getHandle(), "u_MVPMatrix");
	glUniformMatrix4fv(handle, 1, GL_FALSE, mvpMatrix.getPointer());
	checkGLError("MVPMatrix"); 

	glVertexAttribPointer(mPositionHandle, mPositionCount, GL_FLOAT, GL_FALSE, 0, mVertexBuffer);
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
		glBindTexture(GL_TEXTURE_2D, mShape->getTextureId());
		glUniform1i(handle, 0);
		checkGLError("Textures");
	}

	if (mUsesTexCoords)
	{
		glVertexAttribPointer(mTexCoordHandle, TEXCOORD_CNT, GL_FLOAT, false, 0, mTexCoordsBuffer);
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
		glDrawElements(mPrimitiveType, mIndicesCount, GL_UNSIGNED_SHORT, mIndicesBuffer);
		checkGLError("Draw");
	}
	else
	{
		glDrawArrays(mPrimitiveType, 0, mVerticesCount);
	}
}

void Vertices::setIndicesBuffer(unsigned short* buff)
{
	if (!mIndicesBuffer)
	{
		return;
	}

	for (GLsizei i = 0; i < mIndicesCount; i++)
	{
		mIndicesBuffer[i] = buff[i];
	}
}

void Vertices::setTexCoords(float *buff)
{
	if (!mUsesTexCoords)
	{
		return;
	}

	for (int i = 0; i < TEXCOORD_CNT * mVerticesCount; i++)
	{
		mTexCoordsBuffer[i] = buff[i];
	}
}

void Vertices::setVertices(float *buff)
{
	for (int i = 0; i < mPositionCount * mVerticesCount; i++)
	{
		mVertexBuffer[i] = buff[i];
	}
}