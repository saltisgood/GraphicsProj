#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Util.h"
#include "Hands.h"

using namespace cv;
using namespace std;
using namespace proj;

Mat src; Mat src_gray; Mat src_key;
int thresh = 120;
int max_thresh = 255;
RNG rng(12345);
Hands mHands = Hands();
int tickCount = 0;
switches Commands = switches();

const int MAX_HANDS = 2;

/// Function header
void thresh_callback(int, void* );

void button_callback(int,  void*);

void thresherShark();

void debugDisplayImage(Mat&);

void calibrate(vector<Rect>&, bool);

bool interpretCommandSwitches(int, char**);

void printUsage();

int main(int argc, char** argv)
{
	if (!interpretCommandSwitches(argc, argv))
	{
		return -1;
	}

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
	if (argc < 3)
	{
		printUsage();
		return false;
	}

	string arg(argv[1]);
	if (arg[0] == 'c')
	{
		Commands.mIsCamera = true;
	}
	else if (arg[0] == 'v')
	{
		Commands.mIsVideo = true;
	}
	else
	{
		printUsage();
		return false;
	}

	if (Commands.mIsVideo)
	{
		Commands.mFilename = string(argv[2]);
	}
	else
	{
		Commands.mCameraId = atoi(argv[2]);
	}

#ifdef _DEBUG
	cout << "Command switches: " << endl;
	if (Commands.mIsVideo)
	{
		cout << "Video input enabled" << endl << "Input file: " << Commands.mFilename << endl;
	} 
	else if (Commands.mIsCamera)
	{
		cout << "Camera input enabled" << endl << "Camera id: " << Commands.mCameraId << endl;
	}
	else
	{
		printUsage();
	}

#endif

	return true;
}

void printUsage()
{
	cout << "Usage: GraphicsProj c(amera)/v(ideo) cameraId/filename" << endl << 
			"First option: Either camera or video to specify what input to use." << endl <<
			"Second option: Based on the first option, where to take that input from." << endl;
}

bool displayed = false;

void debugDisplayImage(Mat& img)
{
	if (!displayed)
	{
		namedWindow("Debug");
		displayed = true;
	}

	imshow("Debug", img);
	waitKey(0);
}

const int MAX_TRANS_DIFF = 10;

void thresherShark()
{
	Mat threshold_output;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	
	// Detecte edges using Threshold
	threshold(src_gray, threshold_output, thresh, max_thresh, THRESH_BINARY);
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

	vector<Rect> hands (2);
	int max = 0, index = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		int area = boundRect[i].area();
		if (area > max)
		{
			max = area;
			index = i;
		}
	}
	hands[0] = boundRect[index];

	int index2 = 0;
	for (int i = 0, max = 0; i < contours.size(); i++)
	{
		int area = boundRect[i].area();
		if (area > max && i != index)
		{
			max = area;
			index2 = i;
		}
	}
	hands[1] = boundRect[index2];

	if (hands[1].x < hands[0].x)
	{
		Rect swap = hands[0];
		hands[0] = hands[1];
		hands[1] = swap;
	}

	if (tickCount == 0)
	{
		calibrate(hands, true);
	}
	else if (tickCount == 62)
	{
		calibrate(hands, false);
	}
	else if (tickCount == 183)
	{
		if (!mHands.updateHands(sortedHands))
		{
			cout << "Blerghhhh! Errorrr on frame " << tickCount << endl;
			//waitKey(0);
		}
	}
	else if (tickCount > 62)
	{
		if (!mHands.updateHands(sortedHands))
		{
			cout << "Blerghhhh! Errorrr on frame " << tickCount << endl;
			//waitKey(0);
		}
	}

	//mHands.setHandRect(0, hands[0]);
	//mHands.setHandRect(1, hands[1]);

	Mat drawing = src.clone();
	for (int i = 0; i < MAX_HANDS; i++)
	{
		Scalar colour(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		rectangle(drawing, mHands.getPrevRect(i).tl(), mHands.getPrevRect(i).br(), colour, 2, 8, 0);

		Line ln = mHands.getLine(i);
		ln.resetListIter();
		for (int j = 0; j < ln.getLineNumbers(); j++)
		{
			Point curr = ln.getCurrentPoint();
			Point next = ln.getNextPoint();
			//line(drawing, ln.getCurrentPoint(), ln.getNextPoint(), colour, 6);
			line(drawing, curr, next, colour, 3);
		}
	}

	/// Show in a window
  
  //createButton("button", button_callback, );
  imshow( "Contours", drawing );
}

void button_callback(int, void*)
{
	cout << "Button pressed" << endl;
}

void calibrate(vector<Rect>& rects, bool open)
{
	if (rects[0].x < rects[1].x)
	{
		mHands.calibrateHand(0, open, rects[0]);
		mHands.calibrateHand(1, open, rects[1]);
		mHands.setHandRect(0, rects[0]);
		mHands.setHandRect(1, rects[1]);
	}
	else
	{
		mHands.calibrateHand(1, open, rects[0]);
		mHands.setHandRect(1, rects[0]);
		mHands.calibrateHand(0, open, rects[1]);
		mHands.setHandRect(0, rects[1]);
	}
}