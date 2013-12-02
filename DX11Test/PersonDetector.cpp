#include "stdafx.h"
#include "PersonDetector.h"


#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>

using namespace cv;

PersonDetector::PersonDetector(void)
{

	capture = new VideoCapture(0);


}


void PersonDetector::captureFrame()
{


	//Mat myframe;
	if(capture->isOpened())
	{
		int threshold_value = 0;
		int threshold_type = 3;
		int const max_BINARY_value = 255;
		cout << "In capture ..." << endl;

		capture->read(frame);

		cvtColor(frame, grey, CV_RGB2GRAY);
		adaptiveThreshold(grey, bw, 255, ADAPTIVE_THRESH_MEAN_C , THRESH_BINARY, 9 , 20);


		erode( bw, tmp1 , Mat(), Point(-1,-1), 8);
		dilate( tmp1, final , Mat(), Point(-1,-1), 8);



	
	}
}


vector<float> PersonDetector::getHumans(){
	vector<float> fringe;
	
	int count = 0;
	int color = 0xFF;
	
	for(int i=0;i<final.cols;i++){

		count = 0;
		color = 0xFF;
		for(int j=0;j<final.rows;j++){
		//for(int j=final.rows-1;j>=0;j--){
			if(final.at<uchar>(j,i) != color){
				count ++;
				color = final.at<uchar>(j,i);

				if(count == 1 || count & 2 == 1){

					fringe.push_back((float)(final.rows - j - 1.0f));
				
				}
			}
		}
		if(count == 0){
			fringe.push_back(final.rows - 1.0f);
		
		}
	
	}
	return fringe;


}

