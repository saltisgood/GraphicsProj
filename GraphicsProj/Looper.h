#ifndef LOOPER_H_
#define LOOPER_H_

#include <opencv2/highgui/highgui.hpp>

#include "Display.h"
#include "Util.h"
#include "Matrix.h"
#include "Sprite.h"
#include "Line.h"
#include "Hands.h"
#include "Logic.h"

namespace disp
{
	class Looper
	{
	public:
		Looper(Display disp, cv::VideoCapture& vidIn, const proj::switches& args);
		virtual ~Looper();

		void loop();
	private:
		void init();
		void imgMod();
		void shapeDetect();
		void interpretImg(cv::vector<cv::Rect>& shapes);

		Display mDisplay;
		cv::VideoCapture& mVideoInput;
		const proj::switches& mArgs;

		cv::Mat mSource;
		cv::Mat mImageKey;
		cv::Mat mImageMod;

		sGL::Matrix mViewMatrix;
		sGL::Matrix mProjMatrix;
		sGL::Matrix mVPMatrix;

#ifdef __CPP11
		std::shared_ptr<sGL::Sprite>
#else
		sGL::Sprite *
#endif
		mSprite;

#ifdef __CPP11
		std::shared_ptr<cv::ogl::Texture2D>
#else
		cv::ogl::Texture2D *
#endif
		mTexture;

		proj::Colour mGloveColour;

		proj::Hands mHands;

		cv::Scalar mDrawColour;

		proj::Logic mProgramLogic;

		uint mTicks;

		friend void onMouse(int event, int x, int y, int flags, void *data);
		friend void onDraw(void *data);
	};

#ifdef _DEBUG
	void calibrate(proj::Hands&, cv::vector<cv::Rect>&, bool);
#endif
}

#endif