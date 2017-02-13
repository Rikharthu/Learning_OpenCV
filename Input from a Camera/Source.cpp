#define _CRT_SECURE_NO_DEPRECATE
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdio>
#define ESC_KEY 27


using namespace cv;
using namespace std;

void transformImage(const Mat & source, Mat & result);

// first camera has id of 0, if is the only one connected
const int CAMERA_ID = 0;
const int DELAY_BETWEEN_FRAMES = 33;
const string WINDOW_NAME_ORIGINAL_OUTPUT = "camera_output";
const string WINDOW_NAME_TRANSFORMED_OUTPUT = "transformed_camera_output";
const string WINDOW_NAME_LOG_POLAR_OUTPUT = "log_polar";

VideoCapture capture;
bool paused = false;


int main(int argc, char** argv) {
	// prepare windows
	namedWindow(WINDOW_NAME_ORIGINAL_OUTPUT, cv::WINDOW_AUTOSIZE);
	namedWindow(WINDOW_NAME_TRANSFORMED_OUTPUT, cv::WINDOW_AUTOSIZE);
	namedWindow(WINDOW_NAME_LOG_POLAR_OUTPUT, cv::WINDOW_AUTOSIZE);

	
	double fps;
	if (argc == 1) {
		// if passed an integer - opens a camera with that id
		capture.open(CAMERA_ID);
		// pass -1 to "just pick any"
		// capture.get(CAP_PROP_FPS); doesnt work for cameras:
		fps = getCameraFps();		
	}
	else {
		capture.open(argv[1]);
		// get video framerate
		fps = capture.get(CAP_PROP_FPS);
	}
	if (!capture.isOpened()) { // check if we succeeded
		std::cerr << "Couldn't open capture." << std::endl;
		//return -1;
	}

	printf("fps=%d", fps);
	Size size(
		(int)capture.get(CAP_PROP_FRAME_WIDTH), 
		(int)capture.get(CAP_PROP_FRAME_HEIGHT)
	);

	// prepare VideoWriter
	VideoWriter writer;
	// TODO переписать что есть что из документации конструктора
	// initialize it to match camera output parameters
	writer.open("output.avi",			// file name
		CV_FOURCC('M', 'J', 'P', 'G'),	// video code (with which video stream will be compressed)
		fps,							// replay frame rate
		size);							// size of the image

	Mat logpolarFrame,bgrFrame;
	Mat transformedFrame;
	for (;;) {
		if (!paused) {
			capture >> bgrFrame;
			// TODO create timeout
			if (bgrFrame.empty()) break; // no camera input
			imshow(WINDOW_NAME_ORIGINAL_OUTPUT, bgrFrame);

			// WRITING
			// convert to a log-polar format (something like what your eye actually sees)
			logPolar(bgrFrame,			// input
				logpolarFrame,			// output
				Point2f(				// Centerpoint for log-polar transformation
					bgrFrame.cols / 2,	// x
					bgrFrame.rows / 2	// y
				),
				40,						// Magnitude (scale parameter)
				WARP_FILL_OUTLIERS		// Fill outliers with 'zero'
			);
			imshow(WINDOW_NAME_LOG_POLAR_OUTPUT, logpolarFrame);
			// write out the log-polar image to a new video file
			writer << logpolarFrame;

			// transformations
			transformImage(bgrFrame, transformedFrame);
			imshow(WINDOW_NAME_TRANSFORMED_OUTPUT, transformedFrame);
		}
		char c = (char)waitKey(DELAY_BETWEEN_FRAMES);
		if (c == ESC_KEY) break;
		else if (c == 'p') paused = !paused;
	}

	writer.release();
	return 0;
}

void transformImage(const Mat & source, Mat & result) {
	// convert image from bgr to grayscale format
	cvtColor(result, result, CV_BGR2GRAY);
	// Canny edge detector (needs only a single-channel image
	Canny(result, result, 10, 100, 3, true);
}

// Camera's FPS must be calculated experimentally
int getCameraFps() {
	// Calculate fps:
	// Number of frames to capture
	int num_frames = 120;

	// Start and end times
	time_t start, end;

	// Variable for storing video frames
	Mat frame;
	cout << "Capturing " << num_frames << " frames" << endl;

	// Start time
	time(&start);

	// Grab a few frames
	for (int i = 0; i < num_frames; i++)
	{
		capture >> frame;
	}

	// End Time
	time(&end);

	// Time elapsed
	double seconds = difftime(end, start);
	cout << "Time taken : " << seconds << " seconds" << endl;
	// Calculate frames per second
	double fps = num_frames / seconds;
	cout << "Estimated frames per second : " << fps << endl;

	return fps;
}