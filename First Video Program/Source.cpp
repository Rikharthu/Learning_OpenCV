#define _CRT_SECURE_NO_DEPRECATE
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

void blurImage(const Mat&, Mat&);
void transformImage(const Mat&, Mat&);
void readWritePixels(const Mat&, Mat&);


const std::string WINDOW_NAME = "VIDEO_WINDOW";
const string WINDOW_TRANSFORMED = "VIDEO_WINDOW_TRANSFORMED";
// also plays a role of label
const string TRACKBAR_NAME = "TRACKBAR";

int g_slider_position = 0;
// if positive - how many frames will be shown, negative => run mode
int g_run = 1;
// start out in single mode. allows us to update trackbar position without triggering single-step mode
int	g_dontset = 0;
VideoCapture g_cap;


void onTrackbarSlide(int pos, void *) {
	// move to frame
	g_cap.set(CAP_PROP_POS_FRAMES, pos);

	if (!g_dontset) // not 0 => pause
		g_run = 1;
	g_dontset = 0;
}

int main(int argc, char** argv) {
	// create windows
	namedWindow(WINDOW_NAME, cv::WINDOW_AUTOSIZE);
	namedWindow(WINDOW_TRANSFORMED, cv::WINDOW_AUTOSIZE);

	// VideoCapture is initialized to the beginning of the video
	if (argc > 1) {
		// try to open passed videofile
		string videoFile = argv[1];
		printf("Opening %s", videoFile.c_str());
		g_cap.open(videoFile);
	}
	else {
		puts("No video file specified!");
		getchar();
		return -1;
	}
	// check if we succeeded
	if (!g_cap.isOpened()) { 
		std::cerr << "Couldn't open capture." << std::endl;
		//return -1;
	}	

	// get number of frames in the video file
	int frames = (int)g_cap.get(CAP_PROP_FRAME_COUNT);
	// width and height
	int tmpW = (int)g_cap.get(CAP_PROP_FRAME_WIDTH);
	int tmpH = (int)g_cap.get(CAP_PROP_FRAME_HEIGHT);
	// frames per second
	double fps = g_cap.get(CAP_PROP_FPS);
	// by knowing fps, calculate delay between each frame
	double delay = 1000 / fps;

	// print video info
	cout << "Video has " << frames << " frames of dimensions(" << tmpW << ", " << tmpH << ")." << endl;

	// create a trackbar and attach it to named trackbar in the named window
	createTrackbar(TRACKBAR_NAME, WINDOW_NAME, &g_slider_position, frames, onTrackbarSlide);

	// frame data
	Mat frame;
	Mat transformedFrame;
	for (;;) {
		if (g_run != 0) {
			// read frame-by-frame into frame Matrix
			g_cap >> frame;
			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			// check if there are frames to show
			if (frame.empty()) break; // Ran out of film
			// set g_dontset to 1, so that the next trackbar callback will not put us into single-step mode
			g_dontset = 1;
			// set trackbar's position in a window
			setTrackbarPos(TRACKBAR_NAME, WINDOW_NAME, current_pos);
			imshow(WINDOW_NAME, frame);

			// process image frame
			//blurImage(frame, transformedFrame);			
			readWritePixels(frame, transformedFrame);
			transformImage(frame, transformedFrame);
			// show transformed image frame
			if (!transformedFrame.empty()) {
				imshow(WINDOW_TRANSFORMED, transformedFrame);
			}

			g_run -= 1;
		}

		char c = (char)waitKey(delay);
		if (c == 's') // single step
		{
			g_run = 1; cout << "Single step, run = " << g_run << endl;
		}
		if (c == 'r') // run mode
		{
			g_run = -1; cout << "Run mode, run = " << g_run << endl;
		}
		if (c == 27) // escape
			break;
	}

	// release resources
	g_cap.release();
	// destroy the window and deallocate any associated memory usage
	destroyWindow(WINDOW_NAME);
	destroyWindow(WINDOW_TRANSFORMED);
	return 0;
}

// take as references in order to save memory
void blurImage(const Mat & source, Mat & result) {
	// Do the smoothing
	// ( Note: Could use GaussianBlur(), blur(), medianBlur() or bilateralFilter(). )
	GaussianBlur(source, result, Size(5, 5), 3, 3);
	GaussianBlur(result, result, Size(5, 5), 3, 3);
}

void transformImage(const Mat & source, Mat & result) {
	// use pyramid-down algorithm to shrink image twice
	pyrDown(source, result);
	//pyrDown(result, result);
	// convert image from bgr to grayscale format
	cvtColor(result, result, CV_BGR2GRAY);
	// Canny edge detector (needs only a single-channel image
	Canny(result, result, 10, 100, 3, true);
}

void readWritePixels(const Mat & source, Mat & result) {
	int x = 16, y = 32;
	Vec3b intensity = source.at<Vec3b>(y, x);
	// ( Note: We could write img_rgb.at< cv::Vec3b >(x,y)[0] )
	// retrieve pixel info
	uchar blue = intensity[0];
	uchar green = intensity[1];
	uchar red = intensity[2];

	cout << "At (x,y) = (" << x << ", " << y <<
		"): (blue, green, red) = (" <<
		(unsigned int)blue <<
		", " << (unsigned int)green << ", " <<
		(unsigned int)red << ")" << std::endl;

	// grayscale image
	cvtColor(source, result, CV_BGR2GRAY);
	cout << "Gray pixel there is: " <<
		(unsigned int)result.at<uchar>(y, x) << std::endl;	
}