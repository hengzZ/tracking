#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "CompressiveTracker.h"
#include "kcftracker.hpp"

using namespace std;
using namespace cv;


void PrintHelp(int argc, char** argv);
void ReadOptions(int argc, char** argv, std::string& video_name);
void OnMouse(int event, int x, int y, int flags, void* user_data);


// Global image
static Mat frame;
static Point pre_point;
static Point cur_point;


int main(int argc, char** argv)
{
	string video_name;
	string window_name = "demo";
	ReadOptions(argc, argv, video_name);

	VideoCapture capt(video_name);
	if(!capt.isOpened()){
		cerr << "error: can't open the video " << video_name << endl;
		return -1;
	}
	Size frame_size = Size((int)capt.get(CAP_PROP_FRAME_WIDTH), (int)capt.get(CAP_PROP_FRAME_HEIGHT));
	capt >> frame;
	if(frame.empty()){
		cerr << "error: first frame is empty" << endl;
		return -1;
	}

	// Get Box
	namedWindow(window_name, WINDOW_AUTOSIZE);
	setMouseCallback(window_name, OnMouse); // set callback related to a window
	imshow(window_name, frame);
	waitKey(0); // wait key infinitely

	// Tracking
	Mat grayImg;
	Rect box(pre_point,cur_point);
	//Rect box(200,200,200,200);
	//CompressiveTracker ct;	// CT Tracker
	// Note: KCF Tracker
	// KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);
	//KCFTracker kcf(true, false, true, false);
	KCFTracker kcf(false, true, false, false); 

	//cvtColor(frame, grayImg, COLOR_BGR2GRAY);
	//ct.init(grayImg, box);
	kcf.init(box, frame);

	while(1){
		capt >> frame;
		if(frame.empty()){
			cerr << "error: empty frame" << endl;
			break;
		}
		
		std::chrono::time_point<std::chrono::system_clock> start;
		start = std::chrono::system_clock::now();
		//cvtColor(frame, grayImg, COLOR_BGR2GRAY);
		//ct.processFrame(grayImg, box); // process frame;
		Rect result = kcf.update(frame);
		rectangle(frame, result, Scalar(200,0,0), 2);
		//putText(frame,"Object",cvPoint(0,20),2,1,CV_RGB(25,200,25));
		std::cout << "Process Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << " ms"<< endl;

		imshow(window_name, frame);
		char key_val = (char)waitKey(10);
		if(27 == key_val) break;
	}

	destroyAllWindows();
	return 0;
}

void ReadOptions(int argc, char** argv, std::string& video_name)
{
	if(argc < 3){
		PrintHelp(argc, argv);
		exit(0);
	}
	if(strcmp(argv[1], "-v")){
		PrintHelp(argc, argv);
		exit(-1);
	}
	std::string str(argv[2]);
	video_name = str;
}

void PrintHelp(int argc, char** argv)
{
	printf("[Usage]: %s -v video_name\n",argv[0]);
}

void OnMouse(int event, int x, int y, int flags, void* user_data)
{
	char text[64];
	Mat temp_img = frame.clone();
	//frame.copyTo(temp_img);

	if(event == EVENT_LBUTTONDOWN){
		pre_point = Point(x,y);
		sprintf(text, "(%d,%d)", x, y);
		putText(temp_img, text, pre_point, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255), 2);
		circle(temp_img, pre_point, 2, Scalar(0,0,255), FILLED, LINE_AA, 0);
		imshow("demo", temp_img);
	}else if(event == EVENT_MOUSEMOVE && !(flags & EVENT_FLAG_LBUTTON)){
		cur_point = Point(x,y);
		sprintf(text, "(%d,%d)", x, y);
		putText(temp_img, text, cur_point, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 2);
		circle(temp_img, cur_point, 2, Scalar(0,255,0), FILLED, LINE_AA, 0);
		imshow("demo", temp_img);
	}else if(event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON)){
		cur_point = Point(x,y);
		sprintf(text, "(%d,%d)", pre_point.x, pre_point.y);
		putText(temp_img, text, pre_point, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255), 2);
		sprintf(text, "(%d,%d)", x, y);
		putText(temp_img, text, cur_point, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255), 2);
		rectangle(temp_img, pre_point, cur_point, Scalar(0,255,0), 2);
		imshow("demo", temp_img);
	}else if(event == EVENT_LBUTTONUP){
		cur_point = Point(x,y);
		sprintf(text, "(%d,%d)", pre_point.x, pre_point.y);
		putText(temp_img, text, pre_point, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255), 2);
		sprintf(text, "(%d,%d)", x, y);
		putText(temp_img, text, cur_point, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255), 2);
		rectangle(temp_img, pre_point, cur_point, Scalar(255,0,0), 2);
		imshow("demo", temp_img);
	}
}
