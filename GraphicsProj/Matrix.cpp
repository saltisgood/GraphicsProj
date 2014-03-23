#include "Matrix.h"

using namespace sGL;
using namespace std;

Matrix sTemp = Matrix();

Matrix::Matrix()
{
	mArray = new float[MAT4];

	setIdentity();
}

Matrix::~Matrix()
{
	delete[] mArray;
}

void Matrix::setIdentity()
{
	for (int i = 0; i < MAT4; i++)
	{
		mArray[i] = 0;
	}

	for (int i = 0; i < MAT4; i += 5)
	{
		mArray[i] = 1.0f;
	}
}

void Matrix::translate(float x, float y, float z)
{
	for (int i = 0; i < 4; i++)
	{
		mArray[12 + i] += mArray[i] * x + mArray[i + 4] * y + mArray[i + 8] * z;
	}
}

float& Matrix::operator[](int x)
{
	return mArray[x];
}

Matrix& sGL::operator*(Matrix& m1, Matrix& m2)
{
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			float tmp = 0.0f;

			for (int i = 0; i < 4; i++)
			{
				tmp += m1[(4 * y) + i] * m2[x + (4 * i)];
			}

			sTemp[(4 * y) + x] = tmp;
		}
	}

	return sTemp;
}

void Matrix::frustum(float left, float right, float bottom, float top, float near, float far)
{
	if (left == right || top == bottom || near == far || near <= 0.0f || far <= 0.0f)
	{
		//Error
		return;
	}

	float r_width = 1.0f / (right - left);
	float r_height = 1.0f / (top - bottom);
	float r_depth = 1.0f / (near - far);
	
	float x = 2.0f * (near * r_width);
	float y = 2.0f * (near * r_height);

	float A = (right + left) * r_width;
	float B = (top + bottom) * r_height;
	float C = (far + near) * r_depth;
	float D = 2.0f * (far * near * r_depth);

	mArray[0] = x;
	mArray[1] = 0.0f;
	mArray[2] = 0.0f;
	mArray[3] = 0.0f;
	mArray[4] = 0.0f;
	mArray[5] = y;
	mArray[6] = 0.0f;
	mArray[7] = 0.0f;
	mArray[8] = A;
	mArray[9] = B;
	mArray[10] = C;
	mArray[11] = -1.0f;
	mArray[12] = 0.0f;
	mArray[13] = 0.0f;
	mArray[14] = D;
	mArray[15] = 0.0f;
}

void Matrix::ortho(float left, float right, float bottom, float top, float near, float far)
{
	if (left == right || bottom == top || near == far)
	{
		// Error 
		return;
	}

	float r_width = 1.0f / (right - left);
	float r_height = 1.0f / (top - bottom);
	float r_depth = 1.0f / (far - near);
	
	float x = 2.0f * r_width;
	float y = 2.0f * r_height;
	float z = -2.0f * r_depth;

	float tx = -(right + left) * r_width;
	float ty = -(top + bottom) * r_height;
	float tz = -(far + near) * r_depth;

	mArray[0] = x;
	mArray[1] = 0.0f;
	mArray[2] = 0.0f;
	mArray[3] = 0.0f;
	mArray[4] = 0.0f;
	mArray[5] = y;
	mArray[6] = 0.0f;
	mArray[7] = 0.0f;
	mArray[8] = 0.0f;
	mArray[9] = 0.0f;
	mArray[10] = z;
	mArray[11] = 0.0f;
	mArray[12] = tx;
	mArray[13] = ty;
	mArray[14] = tz;
	mArray[15] = 1.0f;
}

void Matrix::operator=(Matrix& m1)
{
	if (this == &m1)
	{
		// Self-assignment check
		return;
	}

	for (int i = 0; i < MAT4; i++)
	{
		mArray[i] = m1.mArray[i];
	}
}

ostream& sGL::operator<<(ostream& stream, Matrix& mat)
{
	stream << "{ " << endl;
	for (int y = 0; y < 4; y++)
	{
		stream << "{ " << mat[(4 * y) + 0] << ", " << mat[(4 * y) + 1] << ", " << mat[(4 * y) + 2] << ", " << mat[(4 * y) + 3] << "}, " << endl;
	}
	stream << "}" << endl;

	return stream;
}