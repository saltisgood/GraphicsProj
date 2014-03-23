#include "Program.h"

using namespace sGL;
using namespace std;

const GLchar * defaultVertexShader = "#version 150 \nuniform mat4 u_MVPMatrix; attribute vec4 a_Position; attribute vec2 a_TexCoordinate; varying vec2 v_TexCoordinate; void main() { v_TexCoordinate = a_TexCoordinate; gl_Position = u_MVPMatrix * a_Position; }";
const GLchar * defaultFragShader = "uniform sampler2D u_Texture; precision mediump float; varying vec2 v_TexCoordinate; uniform float u_Alpha; void main() { gl_FragColor = texture2D(u_Texture, v_TexCoordinate); }";
const GLchar * testFragShader = "#version 150 \n uniform sampler2D u_Texture; precision mediump float; varying vec2 v_TexCoordinate;out vec4 colourOut; \nvoid main() { \n colourOut = texture2D(u_Texture, v_TexCoordinate); \n}";
const GLchar * defaultAttributes[2] = { "a_Position", "a_TexCoordinate" };

Program::Program()
{
	mVertexShaderHandle = loadShader(GL_VERTEX_SHADER, defaultVertexShader);
	mFragShaderHandle = loadShader(GL_FRAGMENT_SHADER, testFragShader);

	mProgramHandle = createProgram(mVertexShaderHandle, mFragShaderHandle, 2, defaultAttributes);
}

Program::~Program()
{
	glDeleteShader(mVertexShaderHandle);
	glDeleteShader(mFragShaderHandle);
	glDeleteProgram(mProgramHandle);
	checkGLError("glDeleteProgram");
}

GLuint sGL::createProgram(GLuint vertexShader, GLuint fragShader, GLuint count = 0, const GLchar** names = NULL)
{
	GLuint program = glCreateProgram();

	if (program != 0)
	{
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragShader);

		if (count > 0)
		{
			for (GLuint i = 0; i < count; i++)
			{
				glBindAttribLocation(program, i + 1, names[i]);
			}
		}

		glLinkProgram(program);

		GLint result;
		glGetProgramiv(program, GL_LINK_STATUS, &result);

		if (result == 0)
		{

			glDeleteProgram(program);
			program = 0;
		}
	}

	if (program == 0)
	{
		cout << "Error creating program!" << endl;
		CV_Error(0, "Error creating GL program!");
	}

	return program;
}

GLuint sGL::loadShader(GLenum type, const GLchar* code)
{
	GLuint shader = glCreateShader(type);

	if (shader != 0)
	{
		glShaderSource(shader, 1, &code, NULL);
		glCompileShader(shader);
		checkGLError("glCompileShader");

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

		if (status == 0)
		{
			cout << "Error creating shader!" << endl;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &status);
			char *log = new char[status];
			glGetShaderInfoLog(shader, status, NULL, log);

			cout << "Log: " << log << endl;

			delete[] log;

			glDeleteShader(shader);
			shader = 0;
		}
	}

	if (shader == 0)
	{
		CV_Error(0, "Shader creation failure!");
	}

	return shader;
}

void sGL::checkGLError(string function)
{
	GLenum error;
	if ((error = glGetError()) != GL_NO_ERROR)
	{
		cout << "GL Error! Function: " << function << endl;
		CV_Error(error, "GL Error");
	}
}