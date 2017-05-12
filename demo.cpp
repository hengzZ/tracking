#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <chrono>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "CompressiveTracker.h"

using namespace std;
using namespace cv;

#define MAX_PATH	128

void PrintHelp(int argc, char** argv);
void ReadOptions(int argc, char** argv, std::string& video_name);
void OnMouse(int event, int x, int y, int flags, void* user_data);

int g_x1, g_y1, g_x2, g_y2;
Mat frame;

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
	imshow("image", frame);

	//setMouseCallback(window_name,OnMouse);
	//char key_val = (char)waitKey(0); // infinitely
	//destroyWindow("image");

	Mat grayImg;
	Rect box(200,200,200,200);
	CompressiveTracker ct;	// CT Tracker
	KCFTracker kcf(true, false, true, false); // KCF Tracker

	cvtColor(frame, grayImg, COLOR_BGR2GRAY);
	ct.init(grayImg, box);

	namedWindow(window_name, WINDOW_AUTOSIZE);
	while(1){
		capt >> frame;
		if(frame.empty()){
			cerr << "error: empty frame" << endl;
			break;
		}
		
		std::chrono::time_point<std::chrono::system_clock> start;
		start = std::chrono::system_clock::now();
		cvtColor(frame, grayImg, COLOR_BGR2GRAY);
		ct.processFrame(grayImg,box); // process frame;
		rectangle(frame, box, Scalar(200,0,0), 2);
		//putText(frame,"Object",cvPoint(0,20),2,1,CV_RGB(25,200,25));
		std::cout << "Process Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << " ms"<< endl;

		imshow(window_name, frame);
		char key_val = (char)waitKey(15);
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
	if(event == EVENT_LBUTTONDOWN){
		g_x1 = x;
		g_y1 = y;
	}else if(event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON)){
		g_x2 = x;
		g_y2 = y;
	}
	Mat temp_img;
	//temp_img = frame.clone();
	frame.copyTo(temp_img);
	rectangle(temp_img, Point(g_x1,g_y1), Point(g_x2,g_y2), Scalar(0,255,0), 2);
	imshow("image",temp_img);
}
