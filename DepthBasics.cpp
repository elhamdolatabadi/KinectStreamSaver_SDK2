//------------------------------------------------------------------------------
// <copyright file="DepthBasics.cpp" company="Microsoft">
//     The Kinect for Windows APIs used here are preliminary and subject to change
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "MainWindow.h"

/*/// <summary>
/// Main processing function
/// </summary>
void CKinectSources::UpdateDepth()
{
    if (!m_pDepthFrameReader)
    {
        return;
    }

    IDepthFrame* pDepthFrame = NULL;

    HRESULT hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);

    if (SUCCEEDED(hr))
    {
        INT64 nTime = 0;
        IFrameDescription* pFrameDescription = NULL;
        int nWidth = 0;
        int nHeight = 0;
        USHORT nDepthMinReliableDistance = 0;
        USHORT nDepthMaxReliableDistance = 0;
        UINT nBufferSize = 0;
        UINT16 *pBuffer = NULL;

        hr = pDepthFrame->get_RelativeTime(&nTime);

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->get_FrameDescription(&pFrameDescription);
        }

        if (SUCCEEDED(hr))
        {
            hr = pFrameDescription->get_Width(&nWidth);
        }

        if (SUCCEEDED(hr))
        {
            hr = pFrameDescription->get_Height(&nHeight);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->get_DepthMaxReliableDistance(&nDepthMaxReliableDistance);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);            
        }

        if (SUCCEEDED(hr))
        {
            ProcessDepth(nTime, pBuffer, nWidth, nHeight, nDepthMinReliableDistance, nDepthMaxReliableDistance);
        }

        SafeRelease(pFrameDescription);
    }

    SafeRelease(pDepthFrame);
}*/

/// <summary>
/// Handle new depth data
/// <param name="nTime">timestamp of frame</param>
/// <param name="pBuffer">pointer to frame data</param>
/// <param name="nWidth">width (in pixels) of input image data</param>
/// <param name="nHeight">height (in pixels) of input image data</param>
/// <param name="nMinDepth">minimum reliable depth</param>
/// <param name="nMaxDepth">maximum reliable depth</param>
/// </summary>
void CKinectSources::ProcessDepth(INT64 nTime, const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth)
{
    if (m_hWnd)
    {
        if (!m_dnStartTime)
        {
            m_dnStartTime = nTime;
        }

        double fps = 0.0;

        LARGE_INTEGER qpcNow = {0};
        if (m_dfFreq)
        {
            if (QueryPerformanceCounter(&qpcNow))
            {
                if (m_dnLastCounter)
                {
                    m_dnFramesSinceUpdate++;
                    fps = m_dfFreq * m_dnFramesSinceUpdate / double(qpcNow.QuadPart - m_dnLastCounter);
                }
            }
        }

        WCHAR szStatusMessage[64];
        StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L" FPS = %0.2f    Time = %I64d", fps, (nTime - m_dnStartTime));

        if (SetStatusMessage_Depth(szStatusMessage, 1000, false))
        {
            m_dnLastCounter = qpcNow.QuadPart;
            m_dnFramesSinceUpdate = 0;
        }
    }

    // Make sure we've received valid data
    if (m_pDepthRGBX && pBuffer && (nWidth == cDepthWidth) && (nHeight == cDepthHeight))
    {
        RGBQUAD* pRGBX = m_pDepthRGBX;
		const UINT16* depthbuff = pBuffer;
		
		/*UINT depthPointCount = nWidth*nHeight;	
		CameraSpacePoint cameraPoint = { 0 };
		DepthSpacePoint depthPoint = { 0 };	

		if (Enable)
		{
			ofstream PCfile;
			WCHAR file[MAX_PATH];
			StringCchPrintfW(file, _countof(file), L"C:\\KinectData\\PC\\pointcloud1.csv");
			PCfile.open(file);
			int i = 0;
			for (int y = 0; y < nHeight; y++)
			{
				for (int x = 0; x < nWidth; x++)
				{
					depthPoint.X = x;
					depthPoint.Y = y;
					m_pCoordinateMapper->MapDepthPointToCameraSpace(depthPoint, pBuffer[i], &cameraPoint);
					PCfile << fixed << cameraPoint.X << "," << fixed << cameraPoint.Y << "," << fixed << cameraPoint.Z << endl;
					i++;
				}
			}
			PCfile.close();			
			Enable = false;
		}*/		

        // end pixel is start + width*height - 1
        const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);		

        while (pBuffer < pBufferEnd)
        {
            USHORT depth = *pBuffer;

            // To convert to a byte, we're discarding the most-significant
            // rather than least-significant bits.
            // We're preserving detail, although the intensity will "wrap."
            // Values outside the reliable depth range are mapped to 0 (black).

            // Note: Using conditionals in this loop could degrade performance.
            // Consider using a lookup table instead when writing production code.
            BYTE intensity = static_cast<BYTE>((depth >= nMinDepth) && (depth <= nMaxDepth) ? (depth % 256) : 0);

            pRGBX->rgbRed   = intensity;
            pRGBX->rgbGreen = intensity;
            pRGBX->rgbBlue  = intensity;

            ++pRGBX;
            ++pBuffer;
        }
		
		// Buffer the Depth stream
		if (e_psaverviewer->EnCapture)
		{
			// capture Depth images
			if (e_psaverviewer->EnDepth)
			{
				if (e_psaverviewer->OpenRawDepth)
				{
					e_pstreamsaver->OpenRawDepthFile(e_psaverviewer->FolderName_d, 0);
					e_psaverviewer->OpenRawDepth = false;
				}

				e_pstreamsaver->BufferDepthStream(reinterpret_cast<BYTE*>(m_pDepthRGBX), nTime, depthbuff);
				e_psaverviewer->SetDepthBufferingText(e_psaverviewer->EnCapture);
			}

			if (e_psaverviewer->EnBG)
			{				
				//e_psaverviewer->EnDepthStatus = 1; // save the depth stream as an image
				//e_pstreamsaver->BufferDepthStream(reinterpret_cast<BYTE*>(m_pDepthRGBX), nTime, depthbuff);
				e_psaverviewer->SetBGBufferingText(e_psaverviewer->EnBG);
				e_psaverviewer->SetBGRecordingText(e_psaverviewer->EnBG);
				e_pstreamsaver->CreatePointCloud(m_pCoordinateMapper, depthbuff, e_psaverviewer->FolderName);				
				e_psaverviewer->EnBG = false;
				e_psaverviewer->SetBGBufferingText(e_psaverviewer->EnBG);
				e_psaverviewer->SetBGRecordingText(e_psaverviewer->EnBG);
			}
		}
		else
			e_psaverviewer->SetDepthBufferingText(e_psaverviewer->EnCapture);

        // Draw the data with Direct2D
        m_pDrawDepth->Draw(reinterpret_cast<BYTE*>(m_pDepthRGBX), cDepthWidth * cDepthHeight * sizeof(RGBQUAD)); 

      //  if (m_bSaveScreenshot)
   /*     {
            WCHAR szScreenshotPath[MAX_PATH];

            // Retrieve the path to My Photos
            GetScreenshotFileName2(szScreenshotPath, _countof(szScreenshotPath));

            // Write out the bitmap to disk
            HRESULT hr = SaveBitmapToFile(reinterpret_cast<BYTE*>(m_pDepthRGBX), nWidth, nHeight, sizeof(RGBQUAD) * 8, szScreenshotPath);

            WCHAR szStatusMessage[64 + MAX_PATH];
            if (SUCCEEDED(hr))
            {
                // Set the status bar to show where the screenshot was saved
                StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L"Screenshot saved to %s", szScreenshotPath);
            }
            else
            {
                StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L"Failed to write screenshot to %s", szScreenshotPath);
            }

           // SetStatusMessage(szStatusMessage, 5000, true);

            // toggle off so we don't save a screenshot again next frame
         //   m_bSaveScreenshot = false;
        }*/
    }
}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
/// <param name="showTimeMsec">time in milliseconds to ignore future status messages</param>
/// <param name="bForce">force status update</param>
bool CKinectSources::SetStatusMessage_Depth(WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce)
{
	DWORD now = GetTickCount();

	if (m_hWnd && (bForce || (m_dnNextStatusTime <= now)))
	{
		SetDlgItemText(m_hWnd, IDC_STATUS3, szMessage);
		m_dnNextStatusTime = now + nShowTimeMsec;

		return true;
	}

	return false;
}

/// <summary>
/// Get the name of the file where screenshot will be stored.
/// </summary>
/// <param name="lpszFilePath">string buffer that will receive screenshot file name.</param>
/// <param name="nFilePathSize">number of characters in lpszFilePath string buffer.</param>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
HRESULT CKinectSources::GetScreenshotFileName2(LPWSTR lpszFilePath, UINT nFilePathSize)
{
    WCHAR* pszKnownPath = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Pictures, 0, NULL, &pszKnownPath);

    if (SUCCEEDED(hr))
    {
        // Get the time
        WCHAR szTimeString[MAX_PATH];
        GetTimeFormatEx(NULL, 0, NULL, L"hh'-'mm'-'ss", szTimeString, _countof(szTimeString));

        // File name will be KinectScreenshotDepth-HH-MM-SS.bmp
        StringCchPrintfW(lpszFilePath, nFilePathSize, L"%s\\KinectScreenshot-Depth-%s.bmp", pszKnownPath, szTimeString);
    }

    if (pszKnownPath)
    {
        CoTaskMemFree(pszKnownPath);
    }

    return hr;
}


