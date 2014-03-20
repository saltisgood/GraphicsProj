#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Util.h"
#include "Hands.h"

using namespace cv;
using namespace std;
using namespace proj;

const int THRESH = 120;
const int MAX_THRESH = 255;

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

int main(int argc, char** argv)
{
	if (!interpretCommandSwitches(argc, argv))
	{
		return -1;
	}

#ifdef _DEBUG
	debugPrintCommands();
#endif 

	//const string file = argv[1];
	VideoCapture vid(Commands.mFilename);

	if (!vid.isOpened())
	{
		cout << "Video could not be opened!" << endl;
		return -1;
	}
	namedWindow( "Contours", CV_WINDOW_AUTOSIZE );

	proj::Colour gloveColour(8, 110, 97);

	for (vid >> src; src.data != NULL; vid >> src, tickCount++)
	{
		vid >> src;

		//debugDisplayImage(src);

		// Load source image
		//src = imread(argv[1]);
		src_key = src.clone();

		//debugDisplayImage(src_key);

		// Extract only glove colour
		proj::chromaKey(src_key, gloveColour);

		//debugDisplayImage(src_key);

		// Convert image to grey
		cvtColor(src_key, src_gray, CV_BGR2GRAY);

		//debugDisplayImage(src_gray);

		// Blur
		blur(src_gray, src_gray, Size(3, 3));

		//debugDisplayImage(src_gray);
		// Create Window
		//string source = "Source";
		//namedWindow(source);
		//imshow(source, src);

		// Fucking thresh
		thresherShark();

		char c = (char)cvWaitKey(50);
        if (c == 27) break;
	}
	
	// Wait for user input
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
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	
	// Detecte edges using Threshold
	threshold(src_gray, threshold_output, THRESH, MAX_THRESH, THRESH_BINARY);
	// Find contours
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point>> contours_poly(contours.size());
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
	else if (tickCount == 62)
	{
		calibrate(sortedHands, false);
	}
	else if (tickCount > 62)
	{
		if (!mHands.updateHands(sortedHands))
		{
			cout << "Blerghhhh! Errorrr on frame " << tickCount << endl;
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
		imshow( "Contours", drawing );
	}
	else
	{
		imshow("Contours", src);
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