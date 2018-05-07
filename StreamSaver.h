//------------------------------------------------------------------------------
// <copyright file="StreamSaver.h">
//     Written by Elham Dolatabadi, A PhD candidate at the Univ of Toronto, Toronto Rehab
//									A memeber of IATSL - Intelligent Assistive Technology and Systems Lab
//------------------------------------------------------------------------------

#pragma once

#include "stdafx.h"
#include <stdio.h>
#include <vector>
#include <deque>
#include <array>
#include "resource.h"
#include <fstream>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include <memory>
#include "skeleton.h"

using namespace std;

class SaverViewer;

#define	MAX_LENGTH_STR	500

class StreamSaver 
{
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;
	static const int        cColorWidth = 1920;
	static const int        cColorHeight = 1080;
private:
	/// Initialization
	void							InitializeStreamSaver();
	/// UnInitialization
	void							UnInitializeStreamSaver();
	/// Allocataion
	void							AllocatePointer();
	/// Save Bitmap images to a file
	HRESULT							SaveColorBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, LPCTSTR lpszFilePath);
	HRESULT							SaveDepthBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, LPCTSTR lpszFilePath);
	HRESULT							SaveColorPNGToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, LPCTSTR lpszFilePath);

	/// Get the name of the file where screenshot will be stored
	///HRESULT							GetScreenshotFileName(wchar_t *screenshotName, UINT screenshotNameSize);

	/// Save the binary images to a file
	HRESULT							SaveRawDepth(USHORT usDepthValue[cDepthWidth * cDepthHeight], WCHAR foldername[MAX_PATH], int counter);
	HRESULT							SaveRawColor(BYTE* p, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, WCHAR foldername[MAX_PATH], int counter);

	/// Stop the Saving the streams
	bool							StopSaveColor(bool StopStatus);	
	bool							StopSaveDepth(bool StopStatus);
	bool							StopSaveSkeleton(bool StopStatus);

	/// Removes the first element in the deque container
	void							Pop_FrontColor();
	void							Pop_FrontDepth();

	/// Clear the Buffer
	void							ClearSkeletonBuffer();
	void							ClearColorBuffer();
	void							ClearDepthBuffer();

	CRITICAL_SECTION				CriticalSection_Skeleton; // Variable to Critical Section object
	CRITICAL_SECTION				CriticalSection_Color; // Variable to Critical Section object
	CRITICAL_SECTION				CriticalSection_Depth; // Variable to Critical Section object
	
/*	struct	SkeletonStream // Dynamic Buufer to store Skeleton streams
	{  
		deque <NUI_SKELETON_DATA> SkeletonJointPosition[NUI_SKELETON_COUNT];
		deque <DWORD> dwframenumber;
		deque <LARGE_INTEGER> FrameTime;
	};
	
	struct SkeletonStream  {
		deque <INT64> liTimeStamp;
		deque <DWORD>  dwFrameNumber;
		deque <Joint_Data> SkeletonJoint[BODY_COUNT];
	};
	SkeletonStream*					SkeletonBuffer;
	*/
	deque <_NUI_SKELETON_FRAME> SkeletonBuffer;
	deque <int> counter;
	

	struct ImageStream // Dynamic Buufer to store color streams
	{
		deque <shared_ptr<vector <BYTE> > >				ImageBuffer;
		//deque <DWORD >									width;
		//deque <DWORD >									height;
		deque <DWORD >									FrameNumber;
		deque <INT64>									FrameTime;
	};

	ImageStream*										 ColorBuffer;
	ImageStream*										 DepthBuffer;

	deque <shared_ptr<vector <USHORT> > >				RawDepthBuffer;				

	ofstream						SkeletonJoint;		// .binary file to write the skeletonjoint
	ofstream						SkeletonOrient;		// .binary file to write the skeletonOrient
	ofstream						SkeletonTime;		// .binary file to write the timestamp	
	ofstream						BGpointCloud;		// .binary file to write the pointcloud
	ofstream						RawDepth;			// .binary file to write the binary depth
	ofstream						RawColor;			// .binary file to write the binary color

	bool							EnInitializeColor;
	bool							EnInitializeDepth;	
	bool							EnInitializeSkel;
	int								s_Counter;	
	int								c_Counter,cc_Counter;	
	int								c_old,d_old;
	int								c_temp, d_temp;
	int								d_Counter,dd_Counter;								

public:
	/// Constructor
	StreamSaver();

	/// Destructor
	~StreamSaver();

	// Save Skeleton Streams 
	int								SaveSkeletonStream(bool EnStop_Skel);

	// Save Color Streams 
	int								SaveColorStream(bool EnStop_Color, WCHAR foldername[MAX_PATH], int EnColorStatus, int fps);
	//int								SaveColorStream();

	// Save Depth Streams 
	int								SaveDepthStream(bool EnStop_Depth, WCHAR foldername[MAX_PATH], int EnDepthStatus, int fps);

	
	// Push back the Skeleton streams into a buffer
	void							BufferSkeletonStream(_NUI_SKELETON_FRAME pFrame);

	// Push back the Color streams into a buffer
	void							BufferColorStream(const BYTE* pImage, INT64 ColorliTimeStamp);

	int								BufferSaveColorStream(BYTE* pImage, INT64 ColorliTimeStamp,bool EnStop_Color, WCHAR foldername[MAX_PATH], int EnColorStatus);

	// Push back the Depth streams into a buffer
	void							BufferDepthStream(const BYTE* pImage, INT64 DepthliTimeStamp, const USHORT* DepthValue);

	// create a CreatePointCloud matrix containg depth pixles transfomed to skeleton space 
	void							CreatePointCloud(ICoordinateMapper* e_pCoordinateMapper, const UINT16* usDepthValue, WCHAR foldername[MAX_PATH]);

	// Open a .binary file to save the skeleton streams
	void							OpenSkeletonFile(WCHAR foldername[MAX_PATH]);

	// Open a .binary file to save raw depth streams
	void							OpenRawDepthFile(WCHAR foldername[MAX_PATH],int counter);

	// Open a .binary file to save raw color streams
	void							OpenRawColorFile(WCHAR foldername[MAX_PATH],int counter);	
};

