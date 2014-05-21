#include <iostream>

#include <opencv2/core/core.hpp>

#include "Util.h"
#include "Looper.h"

using namespace cv;
using namespace std;
using namespace proj;

switches Commands = switches();

bool interpretCommandSwitches(int, char**);
void printUsage();

#ifdef _DEBUG
void debugPrintCommands();
void modImg(const std::string&);
#endif

// Main function. Entry point to the program.
int main(int argc, char** argv)
{
	// Read the command line arguments and set the program up as required
	if (!interpretCommandSwitches(argc, argv))
	{
		return EXIT_FAILURE;
	}
	
	// If this is a debug build, print out the arguments
#ifdef _DEBUG
	debugPrintCommands();
#endif 

	VideoCapture vid;
	if (Commands.isVideo)
	{
		// If a video input is requested, attempt to open it
		vid = VideoCapture(Commands.filename);
	}
	else if (Commands.isCamera)
	{
		// If a camera input is requested, attempt to open it
		vid = VideoCapture(Commands.cameraId);
	}
	else
	{
		// How did you get here???
		cout << "w0t" << endl;
		return EXIT_FAILURE;
	}

	// If the video/camera wasn't able to be opened then quit the program.
	if (!vid.isOpened())
	{
		// Print out the appropriate error message
		cout << ((Commands.isCamera) ? "Camera" : "Video") << " couldn't be opened!" << endl;
		return EXIT_FAILURE;
	}

	disp::Looper looper(disp::Display((int) vid.get(CV_CAP_PROP_FRAME_WIDTH), (int) vid.get(CV_CAP_PROP_FRAME_HEIGHT)), vid, Commands);
	looper.loop();
	
	// Wait for end
	waitKey(0);

	return EXIT_SUCCESS;
}

bool interpretCommandSwitches(int argc, char** argv)
{
	if (argc <= 1)
	{
		printUsage();
		// Set Defaults
		Commands.isCamera = true;
		Commands.cameraId = 0;
		return true;
	}

	for (int i = 1; i < argc; i++)
	{
		string arg(argv[i]);

		if (arg[0] == 'v')
		{
			if (!Commands.isVideo && !Commands.isCamera)
			{
				Commands.isVideo = true;
			}
			else if (Commands.isVideo)
			{
				Commands.filename = arg;
			}
			else
			{
				printUsage();
				return false;
			}
		}
		else if (arg[0] == 'c')
		{
			if (!Commands.isCamera && !Commands.isVideo)
			{
				Commands.isCamera = true;
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
				Commands.debugDisplay = true;
			}
		}
		else if (arg[0] >= '0' && arg[0] <= '9')
		{
			if (Commands.isCamera)
			{
				Commands.cameraId = atoi(argv[i]);
			}
			else
			{
				Commands.filename = arg;
			}
		}
		else
		{
			if (Commands.isVideo)
			{
				Commands.filename = arg;
			}
			else
			{
				printUsage();
				return false;
			}
		}
	}

	if (!Commands.isCamera && !Commands.isVideo)
	{
		Commands.isCamera = true;
		Commands.cameraId = 0;
	}

	return true;
}

#ifdef _DEBUG
void debugPrintCommands()
{
	cout << "Command switches: " << endl;
	if (Commands.isVideo)
	{
		cout << "Video input enabled" << endl << "Input file: " << Commands.filename << endl;
	} 
	else if (Commands.isCamera)
	{
		cout << "Camera input enabled" << endl << "Camera id: " << Commands.cameraId << endl;
	}
	
	if (Commands.debugDisplay)
	{
		cout << "Debug display enabled" << endl;
	}
}

#include <opencv2\core\core.hpp>

void modImg(const std::string& filename)
{
	cv::Mat img;
	img = imread(filename);

	//greenKey(img);
	rgbKey(img, GREEN);
}

#endif

void printUsage()
{
	cout << "Usage: GraphicsProj -options c(amera)/v(ideo) cameraId/filename" << endl << 
			"First parameter: Either camera or video to specify what input to use. (Camera default)" << endl <<
			"Second parameter: Based on the first option, where to take that input from." << endl << endl <<
			"Options: " << "-Ddisplay: Display the debug drawing overlay" << endl;
			
}
