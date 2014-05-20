#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <GL/GLee.h>

#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/GL.h>

#include <iostream>
#include <opencv2/core/core.hpp>

namespace sGL
{
	class Program
	{
	public:
		Program();
		~Program();

		GLuint getHandle() const { return mProgramHandle; }

		virtual bool usesColour() const { return false; }
		virtual bool usesTexture() const { return true; }
		virtual bool usesTexCoords() const { return true; }
		virtual bool usesMVPIndex() const { return false; }
		virtual bool usesAlpha() const { return false; }
		virtual GLuint getPositionHandle() const { return 1; }
		virtual GLuint getTexCoordHandle() const { return 2; }

	private:
		GLuint mProgramHandle;
		GLuint mVertexShaderHandle;
		GLuint mFragShaderHandle;
	};

	GLuint createProgram(GLuint vertexShader, GLuint fragShader, GLuint count = 0, const GLchar** names = 
#ifdef __CPP11
		nullptr
#else
		NULL
#endif
		);
	GLuint loadShader(GLenum type, const GLchar* code);
	void checkGLError(const std::string& function);
}

#endif