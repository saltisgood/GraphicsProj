#include "Looper.h"

#ifdef _DEBUG
#include <time.h>
#endif

using namespace disp;
using namespace std;
using namespace cv;
using namespace proj;
using namespace sGL;

const string WINDOW_TITLE = "Graphics Project";
const int THRESH = 120;
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
#ifdef _DEBUG
	clock_t time = clock();
#endif
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

#ifdef _DEBUG
	time = clock() - time;

	printf("Execution took %d ticks (%f seconds)\n", time, ((float)time / CLOCKS_PER_SEC));
#endif
}

void Looper::imgMod()
{
	static Mat mask;

	// Copy source image
	//mImageKey = mSource.clone();
	//Remove background
	//mBG.extractForeground(mImageKey);
	//proj::rgbKey(mSource, mask, proj::Colour::BLUE, false);
	//mSource = mask;
	static Size boxBlurSize(5,5);
	blur(mSource, mSource, boxBlurSize);
	mBG.extractForeground(mSource);
	mBG.composite(mSource);
	return;
	// Extract colour
	proj::chromaKey(mImageKey, mGloveColour);
	// Convert to grayscale
	cvtColor(mImageKey, mImageMod, CV_BGR2GRAY);
	// Blur
	blur(mImageMod, mImageMod, Size(3, 3));

	shapeDetect();	
}

void Looper::shapeDetect()
{
	Mat threshold_out;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//Detect edges
	threshold(mImageMod, threshold_out, THRESH, MAX_THRESH, THRESH_BINARY);
	// Find shapes
	findContours(threshold_out, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
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

void Looper::interpretImg(vector<Rect>& shapes)
{
	if (mArgs.isCamera)
	{

	}
	else // Is Video
	{
#ifndef _DEBUG
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