#ifndef MATRIX_H_
#define MATRIX_H_

#include <iostream>
#include <memory>
#include "Util.h"

namespace sGL
{
	const int MAT4_W = 4;
	const int MAT4_H = 4;
	const int MAT4 = MAT4_W * MAT4_H;

	class Matrix
	{
		friend std::ostream& operator<<(std::ostream&, const Matrix&);

	public:
		Matrix();
		~Matrix();

		void setIdentity();
		void translate(float x, float y, float z);
		void frustum(float left, float right, float bottom, float top, float near, float far);
		void ortho(float left, float right, float bottom, float top, float near, float far);

		float * getPointer() const 
		{ 
			return mArray
#ifdef __CPP11
			.get() 
#endif
			;
		}

		float& operator[](uchar x);
		void operator=(const Matrix&);
		Matrix& operator*(const Matrix&) const;

	private:

#ifdef __CPP11
		const std::unique_ptr<float[]> mArray;
#else
		float * const mArray;
#endif
		
	};

	std::ostream& operator<<(std::ostream&, const Matrix&);
}

#endif