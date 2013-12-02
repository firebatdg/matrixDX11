#pragma once

#include <vector>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
class PersonDetector
{
public:

	VideoCapture *capture;

	Mat frame;
	Mat grey;
	Mat bw;

	Mat tmp1;
	Mat final;


	PersonDetector(void);
	void captureFrame();
	vector<float> getHumans();

};

