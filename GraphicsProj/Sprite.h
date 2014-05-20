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
		Sprite(uint width, uint height);
		~Sprite();

		const Program& getProgram() const { return mProgram; } 
		GLuint getTextureId() const { return mTex->texId(); }
		void draw(Matrix& vpMatrix) const;

		void setTexture(const cv::Mat& img);

#ifdef __CPP11
		void setTexture(std::shared_ptr<cv::ogl::Texture2D> tex);
#else
		void setTexture(cv::ogl::Texture2D *tex);
#endif

	private:
		void setupIndices();
		void setupTexCoords(TextureRegion& region);
		void setupVertices();
		void setupTexRegions();

		Program const mProgram;
		uint mWidth;
		uint mHeight;

#ifdef __CPP11
		std::shared_ptr<cv::ogl::Texture2D> mTex;
		std::unique_ptr<Vertices> mVertices;
#else
		cv::ogl::Texture2D *mTex;
		Vertices *mVertices;
#endif
	};
}

#endif