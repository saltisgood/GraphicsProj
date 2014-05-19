#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/opengl_interop.hpp>

#include <GL/GLee.h>

#ifdef WIN32
#include <Windows.h>
#include <gl/GL.h>
#endif

#include "Util.h"
#include "Hands.h"
#include "Matrix.h"
#include "Sprite.h"
#include "Looper.h"

using namespace cv;
using namespace std;
using namespace proj;
using namespace sGL;

const int THRESH = 120;
const int MAX_THRESH = 255;
const string WINDOW_TITLE = "Nick's Graphics Project";

int tickCount = 0;

Mat src; Mat src_gray; Mat src_key;
Scalar mRed(0, 0, 255, 255);
Hands mHands = Hands();

switches Commands = switches();

void thresherShark();
void calibrate(vector<Rect>&, bool);
bool interpretCommandSwitches(int, char**);
void printUsage();

#ifdef _DEBUG
void debugPrintCommands();
#endif

// Main function. Entry point to the program.
int main(int argc, char** argv)
{
	
	// Read the command line arguments and set the program up as required
	if (!interpretCommandSwitches(argc, argv))
	{
		return -1;
	}
	
	// If this is a debug build, print out the arguments
#ifdef _DEBUG
	debugPrintCommands();
#endif 

	VideoCapture vid;
	if (Commands.mIsVideo)
	{
		// If a video input is requested, attempt to open it
		vid = VideoCapture(Commands.mFilename);
	}
	else if (Commands.mIsCamera)
	{
		// If a camera input is requested, attempt to open it
		vid = VideoCapture(Commands.mCameraId);
	}
	else
	{
		// How did you get here???
		cout << "w0t" << endl;
		return -1;
	}

	// If the video/camera wasn't able to be opened then quit the program.
	if (!vid.isOpened())
	{
		// Print out the appropriate error message
		cout << ((Commands.mIsCamera) ? "Camera" : "Video") << " couldn't be opened!" << endl;
		return -1;
	}
	
	Size vidSize = Size((int) vid.get(CV_CAP_PROP_FRAME_WIDTH), (int) vid.get(CV_CAP_PROP_FRAME_HEIGHT));

	disp::Display disp = disp::Display(vidSize.width, vidSize.height);
	disp::Looper looper = disp::Looper(&disp, &vid, Commands);
	looper.loop();
	
	// Wait for end
	waitKey(0);

	return 0;
}

bool interpretCommandSwitches(int argc, char** argv)
{
	if (argc <= 1)
	{
		printUsage();
		// Set Defaults
		Commands.mIsCamera = true;
		Commands.mCameraId = 0;
		return true;
	}

	for (int i = 1; i < argc; i++)
	{
		string arg(argv[i]);

		if (arg[0] == 'v')
		{
			if (!Commands.mIsVideo && !Commands.mIsCamera)
			{
				Commands.mIsVideo = true;
			}
			else if (Commands.mIsVideo)
			{
				Commands.mFilename = arg;
			}
			else
			{
				printUsage();
				return false;
			}
		}
		else if (arg[0] == 'c')
		{
			if (!Commands.mIsCamera && !Commands.mIsVideo)
			{
				Commands.mIsCamera = true;
			}
			else
			{
				printUsage();
				return false;
			}
		}
		else if (arg[0] == '-')
		{
			if (arg.compare("-Ddisplay") == 0)
			{
				Commands.mDebugDisplay = true;
			}
		}
		else if (arg[0] >= '0' && arg[0] <= '9')
		{
			if (Commands.mIsCamera)
			{
				Commands.mCameraId = atoi(argv[i]);
			}
			else
			{
				Commands.mFilename = arg;
			}
		}
		else
		{
			if (Commands.mIsVideo)
			{
				Commands.mFilename = arg;
			}
			else
			{
				printUsage();
				return false;
			}
		}
	}

	if (!Commands.mIsCamera && !Commands.mIsVideo)
	{
		Commands.mIsCamera = true;
		Commands.mCameraId = 0;
	}

	return true;
}

#ifdef _DEBUG
void debugPrintCommands()
{
	cout << "Command switches: " << endl;
	if (Commands.mIsVideo)
	{
		cout << "Video input enabled" << endl << "Input file: " << Commands.mFilename << endl;
	} 
	else if (Commands.mIsCamera)
	{
		cout << "Camera input enabled" << endl << "Camera id: " << Commands.mCameraId << endl;
	}
	
	if (Commands.mDebugDisplay)
	{
		cout << "Debug display enabled" << endl;
	}
}
#endif

void printUsage()
{
	cout << "Usage: GraphicsProj -options c(amera)/v(ideo) cameraId/filename" << endl << 
			"First parameter: Either camera or video to specify what input to use. (Camera default)" << endl <<
			"Second parameter: Based on the first option, where to take that input from." << endl << endl <<
			"Options: " << "-Ddisplay: Display the debug drawing overlay" << endl;
			
}

void thresherShark()
{
	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	
	// Detecte edges using Threshold
	threshold(src_gray, threshold_output, THRESH, MAX_THRESH, THRESH_BINARY);
	// Find contours
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}

	vector<Rect> sortedHands = sortRect(boundRect, 5);

	if (tickCount == 0)
	{
		calibrate(sortedHands, true);
	}
	else if (tickCount == 124)
	{
		calibrate(sortedHands, false);
	}
	else if (tickCount > 124)
	{
		if (!mHands.updateHands(sortedHands))
		{
#ifdef _DEBUG
			cout << "Blerghhhh! Errorrr on frame " << tickCount << endl;
#endif
		}
	}

	if (Commands.mDebugDisplay)
	{
		Mat drawing = src.clone();
		for (int i = 0; i < MAX_HANDS; i++)
		{
			rectangle(drawing, mHands.getPrevRect(i).tl(), mHands.getPrevRect(i).br(), mRed, 2, 8, 0);

			Line ln = mHands.getLine(i);
			ln.resetListIter();
			for (int j = 0; j < ln.getLineNumbers(); j++)
			{
				Point curr = ln.getCurrentPoint();
				Point next = ln.getNextPoint();
				line(drawing, curr, next, mRed, 3);
			}
		}
		//imshow(WINDOW_TITLE, drawing );
	}
	else
	{
		//imshow(WINDOW_TITLE, src);
	}
}

void calibrate(vector<Rect>& rects, bool open)
{
	if (rects[0].x < rects[1].x)
	{
		mHands.getHand(Hand::LEFT).calibrate(rects[0], open);
		mHands.getHand(Hand::RIGHT).calibrate(rects[1], open);
	}
	else
	{
		mHands.getHand(Hand::RIGHT).calibrate(rects[0], open);
		mHands.getHand(Hand::LEFT).calibrate(rects[1], open);
	}
}
