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
		Vertices(const Sprite& sprite, GLsizei numVertices, GLsizei numIndices, GLenum primitives);
		~Vertices();

		void draw(Matrix& mvpMatrix);

		void setIndicesBuffer(
#ifdef __CPP11
			std::unique_ptr<ushort[]>
#else
			ushort *
#endif
			);
		
		void setTexCoords(
#ifdef __CPP11
			std::unique_ptr<float[]>
#else
			float *
#endif
			);

		void setVertices(
#ifdef __CPP11
			std::unique_ptr<float[]>
#else
			float *
#endif
			buff);

	private:
		void bind(Matrix& mvpMatrix);

		const Sprite& mShape;
		const Program& mProgram;

		const bool mUsesColour;
		const bool mUsesTexture;
		const bool mUsesTexCoords;
		const bool mUsesMVPIndex;
		const bool mUsesAlpha;

		const GLint mPositionCount;
		const GLint mVertexStride;
		const GLsizei mVertexSize;

		const GLenum mPrimitiveType;
		const GLsizei mIndicesCount;
		const GLsizei mVerticesCount;

		GLuint mPositionHandle;
		GLuint mTexCoordHandle;

#ifdef __CPP11
		std::unique_ptr<float[]>
#else
		float *
#endif
		mVertexBuffer;

#ifdef __CPP11
		std::unique_ptr<float[]>
#else
		float *
#endif
		mTexCoordsBuffer;

#ifdef __CPP11
		std::unique_ptr<ushort[]>
#else
		ushort *
#endif
		mIndicesBuffer;
	};
}

#include "Sprite.h"

#endif