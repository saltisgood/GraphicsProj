#ifndef SPRITE_H_
#define SPRITE_H_

#include "Program.h"
#include "TextureRegion.h"
#include "Matrix.h"

#include <opencv2/core/opengl_interop.hpp>

namespace sGL
{
	class Vertices;

	class Sprite
	{
	public:
		Sprite(UINT width, UINT height);
		~Sprite();

		void setTexture(cv::Mat& img);
		void setTexture(cv::ogl::Texture2D *tex);
		const Program& getProgram() const { return mProgram; } 
		GLuint getTextureId() const { return mTex->texId(); }

		void draw(Matrix& vpMatrix) const;

	private:
		void setupIndices();
		void setupTexCoords(TextureRegion& region);
		void setupVertices();
		void setupTexRegions();

		Program const mProgram;
		cv::ogl::Texture2D *mTex;
		Vertices *mVertices;
		UINT mWidth;
		UINT mHeight;
	};
}

#include "Vertices.h"

#endif