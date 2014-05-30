#include "Looper.h"

#include <time.h>

using namespace disp;
using namespace std;
using namespace cv;
using namespace proj;
using namespace sGL;

const string WINDOW_TITLE = "Graphics Project";
const int THRESH = 10;
const int MAX_THRESH = 255;
const int ESC_KEY = 27;

Looper::Looper(Display display, VideoCapture& videoIn, const switches& args) : 
	mDisplay(display),
	mVideoInput(videoIn),
	mArgs(args),

	mSource(),
	mImageKey(),
	mImageMod(),

	mViewMatrix(),
	mProjMatrix(),
	mVPMatrix(),

#ifdef __CPP11
	mSprite(nullptr),
#else
	mSprite(NULL),
#endif
	mTexture(new cv::ogl::Texture2D()),

	mGloveColour(8, 110, 97),

	mHands(),

	mBG(),

	mDrawColour(0, 0, 255, 255),

	mProgramLogic(display),

	mTicks(0)
{
	init();
}

Looper::~Looper()
{
	mTexture->release();

#ifndef __CPP11
	if (mSprite)
	{
		delete mSprite;
		mSprite = NULL;
	}
#endif
}

void Looper::init()
{
	namedWindow(WINDOW_TITLE, CV_WINDOW_OPENGL | CV_WINDOW_AUTOSIZE);
	resizeWindow(WINDOW_TITLE, mDisplay.getWidth(), mDisplay.getHeight());

	float ratio = (float) mDisplay.getWidth() / (float) mDisplay.getHeight();

	if (ratio > 1.0f)
	{
		mProjMatrix.frustum(-ratio, ratio, -1, 1, 1, 10);
	}
	else
	{
		mProjMatrix.frustum(-1, 1, -1.0f / ratio, 1.0f / ratio, 1, 10);
	}

	int orth = MIN(mDisplay.getWidth(), mDisplay.getHeight());

	mViewMatrix.ortho(-orth / 2.0f, orth / 2.0f, -orth / 2.0f, orth / 2.0f, 0.1f, 100.0f);

	mVPMatrix = (mViewMatrix * mProjMatrix);

#ifdef __CPP11
	mSprite.reset(new Sprite(mDisplay.getWidth(), mDisplay.getHeight()));
#else
	mSprite = new Sprite(mDisplay.getWidth(), mDisplay.getHeight());
#endif
	mSprite->setTexture(mTexture);

	setMouseCallback(WINDOW_TITLE, onMouse, this);
	setOpenGlDrawCallback(WINDOW_TITLE, onDraw, this);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, mDisplay.getWidth(), mDisplay.getHeight());
}

void Looper::loop()
{
	clock_t time = clock();

	for (mVideoInput >> mSource; mSource.data != NULL; mVideoInput >> mSource, mTicks++)
	{
		// Do stuff!
		imgMod();

		if (mArgs.debugDisplay)
		{
			for (int i = 0; i < MAX_HANDS; i++)
			{
				rectangle(mSource, mHands.getPrevRect(i).tl(), mHands.getPrevRect(i).br(), mDrawColour, 2, 8, 0);

				Line ln = mHands.getLine(i);
				ln.resetListIter();
				for (int j = 0; j < ln.getLineNumbers(); j++)
				{
					Point curr = ln.getCurrentPoint();
					Point next = ln.getNextPoint();
					line(mSource, curr, next, mDrawColour, 3);
				}
			}
		}
		mProgramLogic.drawAnything(mSource);
		
		mTexture->copyFrom(mSource);
		updateWindow(WINDOW_TITLE);

		char c = (char)waitKey(25);



		if (c == ESC_KEY) break;
	}

	time = clock() - time;

	printf("Execution took %d ticks (%f seconds)\n", time, ((float)time / CLOCKS_PER_SEC));
}

void Looper::imgMod()
{
	// Apply a small blur to reduce noise
	static Size boxBlurSize(5,5);
	blur(mSource, mSource, boxBlurSize);
	// Extract the matte from the static background
	mBG.extractForeground(mSource);
	static Mat mat;
	// Copy the original image
	mImageKey = mSource.clone();
	// Use the mask to key out the moving image
	mBG.applyMask(mImageKey);
	// Apply a reverse bluescreen method to get a grayscale keyed out image
	proj::rgbKey(mImageKey, mat, proj::Colour::BLUE, false, false);
	// Overlay the keyed out source (original colour) image over the BG
	mBG.composite(mSource);
	// Detect shapes using the gray matte
	shapeDetect(mat);
}

void Looper::shapeDetect(Mat& img)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//Detect edges
	threshold(img, img, THRESH, MAX_THRESH, THRESH_BINARY);
	static Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	static const Point pt(-1, -1);
	morphologyEx(img, img, MORPH_CLOSE, kernel, pt, 1);
	// Find shapes
	findContours(img, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	// Approximate contours to polys
	vector<vector<Point> > shapes(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), shapes[i], 3, true);
		boundRect[i] = boundingRect(Mat(shapes[i]));
	}

	vector<Rect> sortedHands = sortRect(boundRect, 5);

	interpretImg(sortedHands);
}

void Looper::interpretImg(vector<Rect>&)
{
	if (mArgs.isCamera)
	{

	}
	else // Is Video
	{
#ifdef _DEBUG
		/*
		if (mTicks == 0)
		{
			calibrate(mHands, shapes, true);
		}
		else if (mTicks == 124)
		{
			calibrate(mHands, shapes, false);
		}
		else if (mTicks > 124)
		{
			if (!mHands.updateHands(shapes))
			{
				cout << "Blerghhhh! Errorrr on frame " << mTicks << endl;
			}
		}
		*/
#else
		// Nothing yet
#endif
	}
}

void disp::onMouse(int event, int x, int y, int flags, void *data)
{
	//TODO
}

void disp::onDraw(void *data)
{
	Looper *looper = static_cast<Looper*>(data);

	glClear(GL_COLOR_BUFFER_BIT);
	
	looper->mSprite->draw(looper->mVPMatrix);
}

#ifdef _DEBUG
void disp::calibrate(Hands& hands, vector<Rect>& rects, bool open)
{
	if (rects[0].x < rects[1].x)
	{
		hands.getHand(Hand::LEFT).calibrate(rects[0], open);
		hands.getHand(Hand::RIGHT).calibrate(rects[1], open);
	}
	else
	{
		hands.getHand(Hand::RIGHT).calibrate(rects[0], open);
		hands.getHand(Hand::LEFT).calibrate(rects[1], open);
	}
}
#endif