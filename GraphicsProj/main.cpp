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
Matrix mProjMatrix = Matrix();
Matrix mViewMatrix = Matrix();
Matrix mVPMatrix = Matrix();
Sprite *mSprite;

switches Commands = switches();

void thresherShark();
void calibrate(vector<Rect>&, bool);
bool interpretCommandSwitches(int, char**);
void printUsage();

void mouseCallback(int event, int x, int y, int flags, void* data);
void drawcallback(void* data);

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

	// The video/camera opened successfully, so display the window
	namedWindow(WINDOW_TITLE, CV_WINDOW_OPENGL | CV_WINDOW_AUTOSIZE);
	resizeWindow(WINDOW_TITLE, vidSize.width, vidSize.height);

	float ratio = (float) vidSize.width / (float) vidSize.height;

	if (vidSize.width > vidSize.height)
	{
		mProjMatrix.frustum(-ratio, ratio, -1, 1, 1, 10);
	}
	else
	{
		mProjMatrix.frustum(-1, 1, -1.0f / ratio, 1.0f / ratio, 1, 10);
	}

	int orth = min(vidSize.width, vidSize.height);

	mViewMatrix.ortho(-orth / 2.0f, orth / 2.0f, -orth / 2.0f, orth / 2.0f, 0.1f, 100.0f);

	cout << "mProjMatrix = " << mProjMatrix << endl;
	cout << "mViewMatrix = " << mViewMatrix << endl;

	//mVPMatrix = (mProjMatrix * mViewMatrix);
	mVPMatrix = (mViewMatrix * mProjMatrix);

	cout << "mVPMatrix = " << mVPMatrix << endl;
	
	mSprite = new Sprite(vidSize.width, vidSize.height);

	setMouseCallback(WINDOW_TITLE, mouseCallback);

	ogl::Texture2D tex;

	setOpenGlDrawCallback(WINDOW_TITLE, drawcallback, &tex);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, vidSize.width, vidSize.height);

	//updateWindow(WINDOW_TITLE);
	
	if (GLEE_VERSION_3_0)
	{
		cout << "Version 3 enabled " << endl;
	}

	if (GLEE_ARB_shader_objects)
	{
		GLuint shader = glCreateShader(GL_VERTEX_SHADER);
		GLenum error;
		if ((error = glGetError()) != GL_NO_ERROR)
		{
			cout << "error - " << error << endl;
		}
		else
		{
			cout << "Success! Shader - " << shader <<  endl;
		}
	}

	proj::Colour gloveColour(8, 110, 97);

	for (vid >> src; src.data != NULL; vid >> src, tickCount++)
	{
		//debugDisplayImage(src);

		tex.copyFrom(src);
		updateWindow(WINDOW_TITLE);

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

		char c = (char)cvWaitKey(25);
        if (c == 27) break;
	}
	
	// Wait for user input
	waitKey(0);

	delete mSprite;

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

void mouseCallback(int event, int x, int y, int flags, void* userdata)
{
	cout << "Mouse callback: Event - " << event << ", x - " << x << ", y - " << y << ", flags - " << flags << endl;
}

void drawcallback(void* userdata)
{
	
	ogl::Texture2D* pTex = static_cast<ogl::Texture2D*>(userdata);
    if (pTex->empty())
        return;

	glClear(GL_COLOR_BUFFER_BIT);

	mSprite->setTexture(pTex);
	mSprite->draw(mVPMatrix);
	
	/*
    glLoadIdentity();

    glTranslated(0.0, 0.0, 1.0);

    glRotatef( 55, 1, 0, 0 );
    glRotatef( 45, 0, 1, 0 );
    glRotatef( 0, 0, 0, 1 );

    static const int coords[6][4][3] = {
        { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
        { { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
        { { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
        { { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
        { { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
        { { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
    };

    glEnable(GL_TEXTURE_2D);
    pTex->bind();

    for (int i = 0; i < 6; ++i) {
                glColor3ub( i*20, 100+i*10, i*42 );
                glBegin(GL_QUADS);
                for (int j = 0; j < 4; ++j) {
                        glVertex3d(0.2*coords[i][j][0], 0.2 * coords[i][j][1], 0.2*coords[i][j][2]);
                }
                glEnd();
    }

	*/
}