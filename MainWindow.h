//------------------------------------------------------------------------------
// <copyright file="BodyBasics.h" company="Microsoft">
//     The Kinect for Windows APIs used here are preliminary and subject to change
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once
#include "SaverViewer.h"
#include "resource.h"
//#include "skeleton.h"
#include "ImageRenderer.h"
#include <fstream>

#include "StreamSaver.h"


class CKinectSources
{
    static const int        cDepthWidth  = 512;
    static const int        cDepthHeight = 424;
	static const int        cColorWidth = 1920;
	static const int        cColorHeight = 1080;

public:
	/// <summary>
	/// Constructor
	/// </summary>
	CKinectSources();

	/// <summary>
	/// Destructor
	/// </summary>
	~CKinectSources();

	/// <summary>
	/// Handles window messages, passes most to the class instance to handle
	/// </summary>
	/// <param name="hWnd">window message is for</param>
	/// <param name="uMsg">message</param>
	/// <param name="wParam">message data</param>
	/// <param name="lParam">additional message data</param>
	/// <returns>result of message processing</returns>
	static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/// <summary>
	/// Handle windows messages for a class instance
	/// </summary>
	/// <param name="hWnd">window message is for</param>
	/// <param name="uMsg">message</param>
	/// <param name="wParam">message data</param>
	/// <param name="lParam">additional message data</param>
	/// <returns>result of message processing</returns>
	LRESULT CALLBACK        DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/// <summary>
	/// Creates the main window and begins processing
	/// </summary>
	/// <param name="hInstance"></param>
	/// <param name="nCmdShow"></param>
	///int                     Run(HINSTANCE hInstance, int nCmdShow);
	int                     Run(HINSTANCE hInstance, int nCmdShow);

	/// <summary>
	/// Creates the main window and begins processing
	/// </summary>
	/// <param name="hInstance"></param>
	/// <param name="nCmdShow"></param>
	void                     InitThread();

	/// <summary>
	/// Handler function to process WM_CLOSE message
	/// </summary>
	/// <param name="hWnd">Handle to the window</param>
	/// <param name="wParam">Command parameter</param>
	void OnClose(HWND hWnd, WPARAM wParam);

private:
	HWND                    m_hWnd;
	// Body
	INT64                   m_bnStartTime;
	INT64                   m_bnLastCounter;
	double                  m_bfFreq;
	DWORD                   m_bnNextStatusTime;
	DWORD                   m_bnFramesSinceUpdate;

	// Color
	INT64                   m_cnStartTime;
	INT64                   m_cnLastCounter;
	double                  m_cfFreq;
	DWORD                   m_cnNextStatusTime;
	DWORD                   m_cnFramesSinceUpdate;

	// Depth
	INT64                   m_dnStartTime;
	INT64                   m_dnLastCounter;
	double                  m_dfFreq;
	DWORD                   m_dnNextStatusTime;
	DWORD                   m_dnFramesSinceUpdate;

	// Current Kinect
	IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;

	// Frame reader
	IMultiSourceFrameReader* m_pMultiSourceFrameReader;

	// Body reader
	IBodyFrameReader*       m_pBodyFrameReader;

	// Color reader
	//IColorFrameReader*      m_pColorFrameReader;

	// Depth reader
	//IDepthFrameReader*      m_pDepthFrameReader;

	// Direct2D
	ImageRenderer*          m_pDrawColor;
	ID2D1Factory*           m_pD2DFactory;
	//ID2D1Factory*           m_pD2DFactory2;
	RGBQUAD*                m_pColorRGBX;
	ImageRenderer*          m_pDrawDepth;	
	RGBQUAD*                m_pDepthRGBX;

	// Body/hand drawing
	ID2D1HwndRenderTarget*  m_pRenderTarget;
	ID2D1SolidColorBrush*   m_pBrushJointTracked;
	ID2D1SolidColorBrush*   m_pBrushJointInferred;
	ID2D1SolidColorBrush*   m_pBrushBoneTracked;
	ID2D1SolidColorBrush*   m_pBrushBoneInferred;
	ID2D1SolidColorBrush*   m_pBrushHandClosed;
	ID2D1SolidColorBrush*   m_pBrushHandOpen;
	ID2D1SolidColorBrush*   m_pBrushHandLasso;

	/// <summary>
	/// Main processing function
	/// </summary>
	void                    Update();
	//void                    UpdateBody();
	//void                    UpdateColor();
	//void                    UpdateDepth();

	/// <summary>
	/// Initializes the default Kinect sensor
	/// </summary>
	/// <returns>S_OK on success, otherwise failure code</returns>
	HRESULT                 InitializeDefaultSensor();

	/// <summary>
	/// Handle new body data
	/// <param name="nTime">timestamp of frame</param>
	/// <param name="nBodyCount">body data count</param>
	/// <param name="ppBodies">body data in frame</param>
	/// </summary>
	void                    ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies);

	/// <summary>
	/// Handle new color data
	/// <param name="nTime">timestamp of frame</param>
	/// <param name="pBuffer">pointer to frame data</param>
	/// <param name="nWidth">width (in pixels) of input image data</param>
	/// <param name="nHeight">height (in pixels) of input image data</param>
	/// </summary>
	void                    ProcessColor(INT64 nTime, RGBQUAD* pBuffer, int nWidth, int nHeight);

	/// <summary>
	/// Handle new depth data
	/// <param name="nTime">timestamp of frame</param>
	/// <param name="pBuffer">pointer to frame data</param>
	/// <param name="nWidth">width (in pixels) of input image data</param>
	/// <param name="nHeight">height (in pixels) of input image data</param>
	/// <param name="nMinDepth">minimum reliable depth</param>
	/// <param name="nMaxDepth">maximum reliable depth</param>
	/// </summary>
	void                    ProcessDepth(INT64 nTime, const UINT16* pBuffer, int nHeight, int nWidth, USHORT nMinDepth, USHORT nMaxDepth);

	/// <summary>
	/// Set the status bar message
	/// </summary>
	/// <param name="szMessage">message to display</param>
	/// <param name="nShowTimeMsec">time in milliseconds for which to ignore future status messages</param>
	/// <param name="bForce">force status update</param>
	bool                    SetStatusMessage_Body(WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce);
	bool                    SetStatusMessage_Color(WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce);
	bool                    SetStatusMessage_Depth(WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce);

	/// <summary>
	/// Ensure necessary Direct2d resources are created
	/// </summary>
	/// <returns>S_OK if successful, otherwise an error code</returns>
	HRESULT EnsureDirect2DResources();

	/// <summary>
	/// Dispose Direct2d resources 
	/// </summary>
	void DiscardDirect2DResources();

	/// <summary>
	/// Converts a body point to screen space
	/// </summary>
	/// <param name="bodyPoint">body point to tranform</param>
	/// <param name="width">width (in pixels) of output buffer</param>
	/// <param name="height">height (in pixels) of output buffer</param>
	/// <returns>point in screen-space</returns>
	D2D1_POINT_2F           BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height);

	/// <summary>
	/// Draws a body 
	/// </summary>
	/// <param name="pJoints">joint data</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	void                    DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints);

	/// <summary>
	/// Draws a hand symbol if the hand is tracked: red circle = closed, green circle = opened; blue circle = lasso
	/// </summary>
	/// <param name="handState">state of the hand</param>
	/// <param name="handPosition">position of the hand</param>
	void                    DrawHand(HandState handState, const D2D1_POINT_2F& handPosition);

	/// <summary>
	/// Draws one bone of a body (joint to joint)
	/// </summary>
	/// <param name="pJoints">joint data</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	/// <param name="joint0">one joint of the bone to draw</param>
	/// <param name="joint1">other joint of the bone to draw</param>
	void                    DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1);

	/// <summary>
	/// Get the name of the file where screenshot will be stored.
	/// </summary>
	/// <param name="lpszFilePath">string buffer that will receive screenshot file name.</param>
	/// <param name="nFilePathSize">number of characters in lpszFilePath string buffer.</param>
	/// <returns>
	/// S_OK on success, otherwise failure code.
	/// </returns>
	HRESULT                 GetScreenshotFileName(LPWSTR lpszFilePath, UINT nFilePathSize);
	HRESULT                 GetScreenshotFileName2(LPWSTR lpszFilePath, UINT nFilePathSize);

	/// <summary>
	/// Save passed in image data to disk as a bitmap
	/// </summary>
	/// <param name="pBitmapBits">image data to save</param>
	/// <param name="lWidth">width (in pixels) of input image data</param>
	/// <param name="lHeight">height (in pixels) of input image data</param>
	/// <param name="wBitsPerPixel">bits per pixel of image data</param>
	/// <param name="lpszFilePath">full file path to output bitmap to</param>
	/// <returns>indicates success or failure</returns>
	HRESULT                 SaveBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, LPCWSTR lpszFilePath);


	// Open a .binary file to save the skeleton streams
	void				   OpenSkeletonFile(WCHAR foldername[MAX_PATH]);
	void                   SaveSkeletonFrame(_NUI_SKELETON_FRAME skelframe);

	HANDLE				   e_hSaveThread;
	HANDLE				   e_hStopSaveThread;
	/// <summary>
	/// Thread to handle Kinect processing, calls class instance thread processor
	/// </summary>
	/// <param name="pParam">instance pointer</param>
	/// <returns>always 0</returns>
	static DWORD WINAPI     e_SaveThread(LPVOID pParam);

	/// <summary>
	/// Thread to handle Kinect processing
	/// </summary>
	/// <returns>always 0</returns>
	DWORD WINAPI            e_SaveThread();

	//_NUI_SKELETON_FRAME		e_pskeletonjoint;
	ofstream				SkeletonJoint;		// .binary file to write the skeletonjoint
	ofstream				SkeletonOrient;		// .binary file to write the skeletonOrient
	ofstream				SkeletonTime;		// .binary file to write the timestamp	

	bool					EnCapture;
	bool					EnStop;

	StreamSaver*			e_pstreamsaver;
	SaverViewer*			e_psaverviewer;
	int counter;
	int temp;

	bool					Enable;
};
