#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2\opencv.hpp"

using namespace cv;
using namespace std;

const string WINDOW_NAME = "WINDOW_PICTURE";

int main(int argc, char ** argv) {
	Mat image;
	string imageFile = argv[1];
	image = imread(imageFile, 1);
	// first parameter - is the name of the picture
	// 2nd parameter when it's 1 --> Read it in BGR format
	// when it's 0 --> Read it in GrayScale format

	// check if we have an image
	if (image.empty())return -1;

	// create the window
	namedWindow(WINDOW_NAME, // window name
		CV_WINDOW_FREERATIO);

	// show the image inside the window with name WINDOW_NAME
	imshow(WINDOW_NAME, image);
	// show the window until any key pressed
	waitKey(0);
	// destroy the window and deallocate any associated memory usage
	destroyWindow(WINDOW_NAME);
	return 0;
}