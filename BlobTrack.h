#ifndef BLOBTRACK_H
#define BLOBTRACK_H
#include "opencv2/opencv.hpp"

struct target_Blob
{
	std::vector<cv::Point> currentContour;  //轮廓
	cv::Rect currentBoundingRect;           //外接矩形
	double dblCurrentDiagonalSize;          //对角线
	double dblCurrentAspectRatio;           //宽高比
	bool blnCurrentMatchFoundOrNewBlob;     //是否是新目标或被匹配上的目标
	bool blnStillBeingTracked;              //是否一致被跟踪
	int intNumOfConsecutiveFramesWithoutAMatch;  //连续没被匹配到的帧数
	cv::Point predictedNextPosition;         //下时刻的预测中心点（位置） 
	std::vector<cv::Point> centerPositions;  //所有的中心位置

};

class BlobTrack {
	public:
		BlobTrack();
		~BlobTrack();
		
	public:
        void matchCurrentFrameBlobsToExistingBlobs(std::vector<target_Blob> &existingBlobs, std::vector<target_Blob> &currentFrameBlobs);
		
	private:	
		void predictNextPosition(target_Blob &existingBlob);
		void addBlobToExistingBlobs(target_Blob &currentFrameBlob, std::vector<target_Blob> &existingBlobs, int &intIndex);
		void addNewBlob(target_Blob &currentFrameBlob, std::vector<target_Blob> &existingBlobs);
		double distanceBetweenPoints(cv::Point point1, cv::Point point2);
};

#endif    // BLOBTRACK_H


