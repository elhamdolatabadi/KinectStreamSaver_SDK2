//------------------------------------------------------------------------------
// <copyright file="BodyBasics.cpp" company="Microsoft">
//     The Kinect for Windows APIs used here are preliminary and subject to change
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "MainWindow.h"

static const float c_JointThickness = 3.0f;
static const float c_TrackedBoneThickness = 6.0f;
static const float c_InferredBoneThickness = 1.0f;
static const float c_HandSize = 30.0f;

/*/// <summary>
/// Main processing function
/// </summary>
void CKinectSources::UpdateBody()
{
	if (!m_pBodyFrameReader)
	{
		return;
	}

	IBodyFrame* pBodyFrame = NULL;

	HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;

		hr = pBodyFrame->get_RelativeTime(&nTime);

		IBody* ppBodies[BODY_COUNT] = { 0 };

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			ProcessBody(nTime, BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}

	SafeRelease(pBodyFrame);
}
*/

/// <summary>
/// Handle new body data
/// <param name="nTime">timestamp of frame</param>
/// <param name="nBodyCount">body data count</param>
/// <param name="ppBodies">body data in frame</param>
/// </summary>
void CKinectSources::ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies)
{
    if (m_hWnd)
    {
        HRESULT hr = EnsureDirect2DResources();

        if (SUCCEEDED(hr) && m_pRenderTarget && m_pCoordinateMapper)
        {
            m_pRenderTarget->BeginDraw();
            m_pRenderTarget->Clear();
           
			RECT rct;
            GetClientRect(GetDlgItem(m_hWnd, IDC_VIDEOVIEW), &rct);
            int width = rct.right;
            int height = rct.bottom;
			_NUI_SKELETON_FRAME	e_pskeletonjoint;
			e_pskeletonjoint = {};
			counter++;

            for (int i = 0; i < nBodyCount; ++i)
            {
                IBody* pBody = ppBodies[i];			
                if (pBody)
                {
                    BOOLEAN bTracked = false;
                    hr = pBody->get_IsTracked(&bTracked);

                    if (SUCCEEDED(hr) && bTracked)
                    {
                        Joint joints[JointType_Count]; 
                       // D2D1_POINT_2F jointPoints[JointType_Count];
                        HandState leftHandState = HandState_Unknown;
                        HandState rightHandState = HandState_Unknown;

                       // pBody->get_HandLeftState(&leftHandState);
                        pBody->get_HandRightState(&rightHandState);

                       // hr = pBody->GetJoints(_countof(joints), joints);
						hr = pBody->GetJoints(_countof(joints), e_pskeletonjoint.SkeletonJoint[i].JointsPositions);

                        if (SUCCEEDED(hr))
                        {
                           // for (int j = 0; j < _countof(joints); ++j)
							for (int j = 0; j < _countof(joints); ++j)
                            {
                               // jointPoints[j] = BodyToScreen(joints[j].Position, width, height);
								e_pskeletonjoint.SkeletonJoint[i].JointsPoints[j] = BodyToScreen(e_pskeletonjoint.SkeletonJoint[i].JointsPositions[j].Position, width, height);
                            }

							//DrawBody(joints, jointPoints); 
                            //DrawHand(leftHandState, jointPoints[JointType_HandLeft]);
                            //DrawHand(rightHandState, jointPoints[JointType_HandRight]);

							DrawBody(e_pskeletonjoint.SkeletonJoint[i].JointsPositions, e_pskeletonjoint.SkeletonJoint[i].JointsPoints);
							DrawHand(leftHandState, e_pskeletonjoint.SkeletonJoint[i].JointsPoints[JointType_HandLeft]);
							DrawHand(rightHandState, e_pskeletonjoint.SkeletonJoint[i].JointsPoints[JointType_HandRight]);
                        }
                    }
                }
			
            }

			// ----------------- record body index ----------------------------------------- //
			e_pskeletonjoint.liTimeStamp = nTime;
			e_pskeletonjoint.dwFrameNumber = counter;

			if (e_psaverviewer->EnCapture)
			{
				// capture skeleton
				if (e_psaverviewer->EnSkel)
				{
					if (e_psaverviewer->OpenSkelFile)
					{
						e_pstreamsaver->OpenSkeletonFile(e_psaverviewer->FolderName_s);
						e_psaverviewer->OpenSkelFile = false;
					}
					e_pstreamsaver->BufferSkeletonStream(e_pskeletonjoint);
					e_psaverviewer->SetSkelBufferingText(e_psaverviewer->EnCapture);						
				}
			}
			else
				e_psaverviewer->SetSkelBufferingText(e_psaverviewer->EnCapture);
			



			//SaveSkeletonFrame(e_pskeletonjoint);
			// ------------------------------------------------------------------------------

            hr = m_pRenderTarget->EndDraw();

            // Device lost, need to recreate the render target
            // We'll dispose it now and retry drawing
            if (D2DERR_RECREATE_TARGET == hr)
            {
                hr = S_OK;
                DiscardDirect2DResources();
            }
        }

        if (!m_bnStartTime)
        {
            m_bnStartTime = nTime;
        }

        double fps = 0.0;

        LARGE_INTEGER qpcNow = {0};
        if (m_bfFreq)
        {
            if (QueryPerformanceCounter(&qpcNow))
            {
                if (m_bnLastCounter)
                {
                    m_bnFramesSinceUpdate++;
                    fps = m_bfFreq * m_bnFramesSinceUpdate / double(qpcNow.QuadPart - m_bnLastCounter);
                }
            }
        }

        WCHAR szStatusMessage[64];
        StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L" FPS = %0.2f    Time = %I64d", fps, (nTime - m_bnStartTime));

		if (SetStatusMessage_Body(szStatusMessage, 1000, false))
        {
            m_bnLastCounter = qpcNow.QuadPart;
            m_bnFramesSinceUpdate = 0;
        }
    }
}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
/// <param name="showTimeMsec">time in milliseconds to ignore future status messages</param>
/// <param name="bForce">force status update</param>
bool CKinectSources::SetStatusMessage_Body(WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce)
{
    DWORD now = GetTickCount();

    if (m_hWnd && (bForce || (m_bnNextStatusTime <= now)))
    {
        SetDlgItemText(m_hWnd, IDC_STATUS, szMessage);
        m_bnNextStatusTime = now + nShowTimeMsec;

        return true;
    }

    return false;
}

/// <summary>
/// Ensure necessary Direct2d resources are created
/// </summary>
/// <returns>S_OK if successful, otherwise an error code</returns>
HRESULT CKinectSources::EnsureDirect2DResources()
{
    HRESULT hr = S_OK;

    if (m_pD2DFactory && !m_pRenderTarget)
    {
        RECT rc;
        GetWindowRect(GetDlgItem(m_hWnd, IDC_VIDEOVIEW), &rc);  

        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        D2D1_SIZE_U size = D2D1::SizeU(width, height);
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;

        // Create a Hwnd render target, in order to render to the window set in initialize
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            rtProps,
            D2D1::HwndRenderTargetProperties(GetDlgItem(m_hWnd, IDC_VIDEOVIEW), size),
            &m_pRenderTarget
        );

        if (FAILED(hr))
        {
            SetStatusMessage_Body(L"Couldn't create Direct2D render target!", 10000, true);
            return hr;
        }

        // light green
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.27f, 0.75f, 0.27f), &m_pBrushJointTracked);

        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &m_pBrushJointInferred);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 1.0f), &m_pBrushBoneTracked);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 1.0f), &m_pBrushBoneInferred);

        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 0.5f), &m_pBrushHandClosed);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 0.5f), &m_pBrushHandOpen);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 0.5f), &m_pBrushHandLasso);
    }

    return hr;
}

/// <summary>
/// Dispose Direct2d resources 
/// </summary>
void CKinectSources::DiscardDirect2DResources()
{
    SafeRelease(m_pRenderTarget);

    SafeRelease(m_pBrushJointTracked);
    SafeRelease(m_pBrushJointInferred);
    SafeRelease(m_pBrushBoneTracked);
    SafeRelease(m_pBrushBoneInferred);

    SafeRelease(m_pBrushHandClosed);
    SafeRelease(m_pBrushHandOpen);
    SafeRelease(m_pBrushHandLasso);
}

/// <summary>
/// Converts a body point to screen space
/// </summary>
/// <param name="bodyPoint">body point to tranform</param>
/// <param name="width">width (in pixels) of output buffer</param>
/// <param name="height">height (in pixels) of output buffer</param>
/// <returns>point in screen-space</returns>
D2D1_POINT_2F CKinectSources::BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height)
{
    // Calculate the body's position on the screen
    DepthSpacePoint depthPoint = {0};	
	m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);

    float screenPointX = static_cast<float>(depthPoint.X * width) / cDepthWidth;
    float screenPointY = static_cast<float>(depthPoint.Y * height) / cDepthHeight;

    return D2D1::Point2F(screenPointX, screenPointY);
}

/// <summary>
/// Draws a body 
/// </summary>
/// <param name="pJoints">joint data</param>
/// <param name="pJointPoints">joint positions converted to screen space</param>
void CKinectSources::DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints)
{
    // Draw the bones

    // Torso
    DrawBone(pJoints, pJointPoints, JointType_Head, JointType_Neck);
    DrawBone(pJoints, pJointPoints, JointType_Neck, JointType_SpineShoulder);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_SpineMid);
    DrawBone(pJoints, pJointPoints, JointType_SpineMid, JointType_SpineBase);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderRight);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderLeft);
    DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipRight);
    DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipLeft);
    
    // Right Arm    
    DrawBone(pJoints, pJointPoints, JointType_ShoulderRight, JointType_ElbowRight);
    DrawBone(pJoints, pJointPoints, JointType_ElbowRight, JointType_WristRight);
    DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_HandRight);
    DrawBone(pJoints, pJointPoints, JointType_HandRight, JointType_HandTipRight);
    DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_ThumbRight);

    // Left Arm
    DrawBone(pJoints, pJointPoints, JointType_ShoulderLeft, JointType_ElbowLeft);
    DrawBone(pJoints, pJointPoints, JointType_ElbowLeft, JointType_WristLeft);
    DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_HandLeft);
    DrawBone(pJoints, pJointPoints, JointType_HandLeft, JointType_HandTipLeft);
    DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_ThumbLeft);

    // Right Leg
    DrawBone(pJoints, pJointPoints, JointType_HipRight, JointType_KneeRight);
    DrawBone(pJoints, pJointPoints, JointType_KneeRight, JointType_AnkleRight);
    DrawBone(pJoints, pJointPoints, JointType_AnkleRight, JointType_FootRight);

    // Left Leg
    DrawBone(pJoints, pJointPoints, JointType_HipLeft, JointType_KneeLeft);
    DrawBone(pJoints, pJointPoints, JointType_KneeLeft, JointType_AnkleLeft);
    DrawBone(pJoints, pJointPoints, JointType_AnkleLeft, JointType_FootLeft);

    // Draw the joints
    for (int i = 0; i < JointType_Count; ++i)
    {
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(pJointPoints[i], c_JointThickness, c_JointThickness);

        if (pJoints[i].TrackingState == TrackingState_Inferred)
        {
            m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointInferred);
        }
        else if (pJoints[i].TrackingState == TrackingState_Tracked)
        {
            m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointTracked);
        }
    }
}

/// <summary>
/// Draws one bone of a body (joint to joint)
/// </summary>
/// <param name="pJoints">joint data</param>
/// <param name="pJointPoints">joint positions converted to screen space</param>
/// <param name="pJointPoints">joint positions converted to screen space</param>
/// <param name="joint0">one joint of the bone to draw</param>
/// <param name="joint1">other joint of the bone to draw</param>
void CKinectSources::DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1)
{
    TrackingState joint0State = pJoints[joint0].TrackingState;
    TrackingState joint1State = pJoints[joint1].TrackingState;

    // If we can't find either of these joints, exit
    if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
    {
        return;
    }

    // Don't draw if both points are inferred
    if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
    {
        return;
    }

    // We assume all drawn bones are inferred unless BOTH joints are tracked
    if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
    {
        m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneTracked, c_TrackedBoneThickness);
    }
    else
    {
        m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneInferred, c_InferredBoneThickness);
    }
}

/// <summary>
/// Draws a hand symbol if the hand is tracked: red circle = closed, green circle = opened; blue circle = lasso
/// </summary>
/// <param name="handState">state of the hand</param>
/// <param name="handPosition">position of the hand</param>
void CKinectSources::DrawHand(HandState handState, const D2D1_POINT_2F& handPosition)
{
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(handPosition, c_HandSize, c_HandSize);

    switch (handState)
    {
        case HandState_Closed:
            m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandClosed);
            break;

        case HandState_Open:
            m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandOpen);
            break;

        case HandState_Lasso:
            m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandLasso);
            break;
    }
}


/*/// Open a .binary file to save the skeleton streams
void CKinectSources::OpenSkeletonFile(WCHAR foldername[MAX_PATH])
{
	WCHAR SkeletonFileName[MAX_PATH];
	StringCchPrintfW(SkeletonFileName, _countof(SkeletonFileName), L"%s\\Joint_Position.binary", foldername);
	SkeletonJoint.open(SkeletonFileName, ios::binary);

	StringCchPrintfW(SkeletonFileName, _countof(SkeletonFileName), L"%s\\Joint_Orientation.binary", foldername);
	SkeletonOrient.open(SkeletonFileName, ios::binary);

	StringCchPrintfW(SkeletonFileName, _countof(SkeletonFileName), L"%s\\liTimeStamp.binary", foldername);
	SkeletonTime.open(SkeletonFileName, ios::binary);

}

/// <summary>
void CKinectSources::SaveSkeletonFrame(_NUI_SKELETON_FRAME skelframe)
{
	float jbuffer[JointType_Count*BODY_COUNT][4] = { 0, 0 };
	for (int i = 0; i < BODY_COUNT; i++)
	{
		Joint pJoints[JointType_Count];		
		for (int j = 0; j < JointType_Count; j++)
		{
			int k = i * JointType_Count + j;
			pJoints[j] = skelframe.SkeletonJoint[i].JointsPositions[j];

			if (pJoints[j].TrackingState != TrackingState_Inferred && pJoints[j].TrackingState != TrackingState_Tracked)
			{
				pJoints[j].Position.X = 0;
				pJoints[j].Position.Y = 0;
				pJoints[j].Position.Z = 0;
				pJoints[j].TrackingState = TrackingState_NotTracked;
			}			
			// jointPosition
			jbuffer[k][0] = pJoints[j].Position.X;
			jbuffer[k][1] = pJoints[j].Position.Y;
			jbuffer[k][2] = pJoints[j].Position.Z;
			jbuffer[k][3] = pJoints[j].TrackingState;
				/*SkeletonJoint << pJoints[j].Position.X << ","
						  << pJoints[j].Position.Y << ","
						  << pJoints[j].Position.Z << ","
						  << pJoints[j].TrackingState << endl;
		}
	}
	SkeletonJoint.write((const char*)(jbuffer), JointType_Count * BODY_COUNT * 4 * sizeof(float));
	SkeletonTime.write((const char*)(&skelframe.liTimeStamp), sizeof(INT64));
	//SkeletonTime << skelframe.liTimeStamp << endl;
}
*/