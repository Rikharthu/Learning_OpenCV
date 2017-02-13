#define _CRT_SECURE_NO_DEPRECATE
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdio>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	cv::namedWindow("Example2_10", cv::WINDOW_AUTOSIZE);
	cv::VideoCapture cap;
	if (argc == 1) {
		cap.open(0); // open the first camera
	}
	else {
		cap.open(argv[1]);
	}
	if (!cap.isOpened()) { // check if we succeeded
		std::cerr << "Couldn't open capture." << std::endl;
		//return -1;
	}

	waitKey(0);
}