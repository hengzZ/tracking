#include "BlobTrack.h"
#include <vector>

BlobTrack::BlobTrack() 
{

}

BlobTrack::~BlobTrack()
{

}

void BlobTrack::predictNextPosition(target_Blob &existingBlob) {
    int numPositions = existingBlob.centerPositions.size();

    if (numPositions == 1) {
		existingBlob.predictedNextPosition.x = existingBlob.centerPositions.back().x;
		existingBlob.predictedNextPosition.y = existingBlob.centerPositions.back().y;
    }
    else if (numPositions == 2) {
        int deltaX = existingBlob.centerPositions[1].x - existingBlob.centerPositions[0].x;
        int deltaY = existingBlob.centerPositions[1].y - existingBlob.centerPositions[0].y;
		existingBlob.predictedNextPosition.x = existingBlob.centerPositions.back().x + deltaX;
		existingBlob.predictedNextPosition.y = existingBlob.centerPositions.back().y + deltaY;
    }
    else if (numPositions == 3) {
        int sumOfXChanges = ((existingBlob.centerPositions[2].x - existingBlob.centerPositions[1].x) * 2) +
            ((existingBlob.centerPositions[1].x - existingBlob.centerPositions[0].x) * 1);

        int deltaX = (int)std::round((float)sumOfXChanges / 3.0);

        int sumOfYChanges = ((existingBlob.centerPositions[2].y - existingBlob.centerPositions[1].y) * 2) +
            ((existingBlob.centerPositions[1].y - existingBlob.centerPositions[0].y) * 1);

        int deltaY = (int)std::round((float)sumOfYChanges / 3.0);

		existingBlob.predictedNextPosition.x = existingBlob.centerPositions.back().x + deltaX;
		existingBlob.predictedNextPosition.y = existingBlob.centerPositions.back().y + deltaY;
    }
    else if (numPositions == 4) {
        int sumOfXChanges = ((existingBlob.centerPositions[3].x - existingBlob.centerPositions[2].x) * 3) +
            ((existingBlob.centerPositions[2].x - existingBlob.centerPositions[1].x) * 2) +
            ((existingBlob.centerPositions[1].x - existingBlob.centerPositions[0].x) * 1);

        int deltaX = (int)std::round((float)sumOfXChanges / 6.0);

        int sumOfYChanges = ((existingBlob.centerPositions[3].y - existingBlob.centerPositions[2].y) * 3) +
            ((existingBlob.centerPositions[2].y - existingBlob.centerPositions[1].y) * 2) +
            ((existingBlob.centerPositions[1].y - existingBlob.centerPositions[0].y) * 1);

        int deltaY = (int)std::round((float)sumOfYChanges / 6.0);

		existingBlob.predictedNextPosition.x = existingBlob.centerPositions.back().x + deltaX;
		existingBlob.predictedNextPosition.y = existingBlob.centerPositions.back().y + deltaY;
    }
    else if (numPositions >= 5) {
        int sumOfXChanges = ((existingBlob.centerPositions[numPositions - 1].x - existingBlob.centerPositions[numPositions - 2].x) * 4) +
            ((existingBlob.centerPositions[numPositions - 2].x - existingBlob.centerPositions[numPositions - 3].x) * 3) +
            ((existingBlob.centerPositions[numPositions - 3].x - existingBlob.centerPositions[numPositions - 4].x) * 2) +
            ((existingBlob.centerPositions[numPositions - 4].x - existingBlob.centerPositions[numPositions - 5].x) * 1);

        int deltaX = (int)std::round((float)sumOfXChanges / 10.0);

        int sumOfYChanges = ((existingBlob.centerPositions[numPositions - 1].y - existingBlob.centerPositions[numPositions - 2].y) * 4) +
            ((existingBlob.centerPositions[numPositions - 2].y - existingBlob.centerPositions[numPositions - 3].y) * 3) +
            ((existingBlob.centerPositions[numPositions - 3].y - existingBlob.centerPositions[numPositions - 4].y) * 2) +
            ((existingBlob.centerPositions[numPositions - 4].y - existingBlob.centerPositions[numPositions - 5].y) * 1);

        int deltaY = (int)std::round((float)sumOfYChanges / 10.0);

		existingBlob.predictedNextPosition.x = existingBlob.centerPositions.back().x + deltaX;
		existingBlob.predictedNextPosition.y = existingBlob.centerPositions.back().y + deltaY;
    }
    else {
        // should never get here!
    }
}

void BlobTrack::matchCurrentFrameBlobsToExistingBlobs(std::vector<target_Blob> &existingBlobs, std::vector<target_Blob> &currentFrameBlobs)
{
	for (auto &existingBlob : existingBlobs)
	{
		existingBlob.blnCurrentMatchFoundOrNewBlob = false;
		predictNextPosition(existingBlob);
	}
	for(auto &currentFrameBlob : currentFrameBlobs){
		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 1000000.0;

		for (unsigned int i = 0; i < existingBlobs.size(); i++){
			if (existingBlobs[i].blnStillBeingTracked == true){
				//根据中心点之间的距离，去匹配已经存在的目标.
				double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);
				if (dblDistance < dblLeastDistance){
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}

		if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5)   //距离小于对角线一半，即：匹配上
			addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
		else
			addNewBlob(currentFrameBlob, existingBlobs);  //新目标
	}

	for (int i = 0; i < existingBlobs.size(); i++) {
		if (existingBlobs[i].blnCurrentMatchFoundOrNewBlob == false)
			existingBlobs[i].intNumOfConsecutiveFramesWithoutAMatch++;
		if (existingBlobs[i].intNumOfConsecutiveFramesWithoutAMatch >= 5)   //连续5帧以上没有匹配上，认为不再跟踪
			existingBlobs[i].blnStillBeingTracked = false;

		//需要增加对数组的判断，防止数组过大。
		if (existingBlobs[i].intNumOfConsecutiveFramesWithoutAMatch >= 50)   //连续50帧以上没有匹配上，删除
			existingBlobs.erase(existingBlobs.begin() + i);
	}
}


void BlobTrack::addBlobToExistingBlobs(target_Blob &currentFrameBlob, std::vector<target_Blob> &existingBlobs, int &intIndex)
{
	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;
	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;
	existingBlobs[intIndex].blnStillBeingTracked = true;
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;

	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());
}


void BlobTrack::addNewBlob(target_Blob &currentFrameBlob, std::vector<target_Blob> &existingBlobs)
{
	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;
	existingBlobs.push_back(currentFrameBlob);
}


double BlobTrack::distanceBetweenPoints(cv::Point point1, cv::Point point2)
{
	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}
