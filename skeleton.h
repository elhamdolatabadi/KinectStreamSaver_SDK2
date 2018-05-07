
#include "resource.h"
#include <fstream>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include <memory>

using namespace std;

typedef struct Joint_Data {
	Joint JointsPositions[JointType_Count];
	D2D1_POINT_2F JointsPoints[JointType_Count];
}Joint_Data;


typedef struct _NUI_SKELETON_FRAME {
	INT64 liTimeStamp;
	DWORD dwFrameNumber;	
	Joint_Data SkeletonJoint[BODY_COUNT];
} NUI_SKELETON_FRAME;


