#ifndef MATRIX_H_
#define MATRIX_H_

#include <iostream>

namespace sGL
{
	const int MAT4 = 16;

	class Matrix
	{
	public:
		Matrix();
		~Matrix();

		void setIdentity();
		void translate(float x, float y, float z);
		void frustum(float left, float right, float bottom, float top, float pNear, float pFar);
		void ortho(float left, float right, float bottom, float top, float pNear, float pFar);
		float* getPointer() const { return mArray; }

		float& operator[](int x);
		void operator=(Matrix&);
	private:
		float *mArray;
	};

	Matrix& operator*(Matrix&, Matrix&);
	std::ostream& operator<<(std::ostream&, Matrix&);
}

#endif