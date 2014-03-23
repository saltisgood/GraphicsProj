#ifndef VERTICES_H_
#define VERTICES_H_

#include <opencv2/core/opengl_interop.hpp>

#include "Matrix.h"
#include "Program.h"

namespace sGL
{
	static const int TEXCOORD_CNT = 2; 
	static const GLint POSITION_CNT_2D = 2;

	class Sprite;

	class Vertices
	{
	public:
		Vertices(Sprite* sprite, GLsizei numVertices, GLsizei numIndices, GLenum primitives);
		~Vertices();

		void draw(Matrix& mvpMatrix);
		void setIndicesBuffer(unsigned short* buff);
		void setTexCoords(float* buff);
		void setVertices(float *buff);
	private:
		void bind(Matrix& mvpMatrix);

		Sprite * const mShape;
		Program const& mProgram;

		const bool mUsesColour;
		const bool mUsesTexture;
		const bool mUsesTexCoords;
		const bool mUsesMVPIndex;
		const bool mUsesAlpha;

		GLuint mPositionHandle;
		GLuint mTexCoordHandle;
		
		const GLint mPositionCount;
		const GLsizei mVertexSize;
		const GLint mVertexStride;

		const GLenum mPrimitiveType;
		const GLsizei mIndicesCount;
		const GLsizei mVerticesCount;

		float* mVertexBuffer;
		float *mTexCoordsBuffer;
		unsigned short *mIndicesBuffer;
	};
}

#include "Sprite.h"

#endif