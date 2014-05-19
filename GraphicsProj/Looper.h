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
		Looper(Display *disp, cv::VideoCapture *vidIn, proj::switches& args);
		~Looper();

		void loop();
	private:
		void init();
		void imgMod();
		void shapeDetect();
		void interpretImg(cv::vector<cv::Rect>& shapes);

		Display * const mDisplay;
		cv::VideoCapture * const mVideoInput;
		proj::switches const& mArgs;

		cv::Mat mSource;
		cv::Mat mImageKey;
		cv::Mat mImageMod;

		sGL::Matrix mProjMatrix;
		sGL::Matrix mViewMatrix;
		sGL::Matrix mVPMatrix;

		sGL::Sprite *mSprite;

		cv::ogl::Texture2D *mTexture;

		proj::Colour mGloveColour;

		proj::Hands mHands;

		cv::Scalar mDrawColour;

		proj::Logic mProgramLogic;

		uint mTicks;

		friend void onMouse(int event, int x, int y, int flags, void *data);
		friend void onDraw(void *data);
	};

	void onMouse(int event, int x, int y, int flags, void *data);
	void onDraw(void *data);


	// DEBUG ONLY!
	void calibrate(proj::Hands&, cv::vector<cv::Rect>&, bool);
}

#endif