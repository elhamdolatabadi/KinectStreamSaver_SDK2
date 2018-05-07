//------------------------------------------------------------------------------
// <copyright file="BodyBasics.cpp" company="Microsoft">
//     The Kinect for Windows APIs used here are preliminary and subject to change
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "MainWindow.h"
#include "stdafx.h"
#include "resource.h"


#define SKEL		  0
#define COLOR		  1
#define DEPTH		  2
#define STREAM        1
#define BG			  2

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);	

	CKinectSources application;	
    application.Run(hInstance, nCmdShow);	
}

/// <summary>
/// Constructor
/// </summary>
CKinectSources::CKinectSources() :
m_hWnd(NULL),
m_bnStartTime(0),
m_bnLastCounter(0),
m_bnFramesSinceUpdate(0),
m_bfFreq(0),
m_bnNextStatusTime(0),
m_cnStartTime(0),
m_cnLastCounter(0),
m_cnFramesSinceUpdate(0),
m_cfFreq(0),
m_cnNextStatusTime(0),
m_dnStartTime(0),
m_dnLastCounter(0),
m_dnFramesSinceUpdate(0),
m_dfFreq(0),
m_dnNextStatusTime(0),
m_pKinectSensor(NULL),
m_pCoordinateMapper(NULL),
//m_pBodyFrameReader(NULL),
m_pD2DFactory(NULL),
m_pRenderTarget(NULL),
m_pBrushJointTracked(NULL),
m_pBrushJointInferred(NULL),
m_pBrushBoneTracked(NULL),
m_pBrushBoneInferred(NULL),
m_pBrushHandClosed(NULL),
m_pBrushHandOpen(NULL),
m_pBrushHandLasso(NULL),
//m_pColorFrameReader(NULL),
m_pDrawColor(NULL),
m_pColorRGBX(NULL),
m_pMultiSourceFrameReader(NULL)
{
	LARGE_INTEGER qpf = { 0 };	
	if (QueryPerformanceFrequency(&qpf))
	{
		m_bfFreq = double(qpf.QuadPart);
		m_cfFreq = double(qpf.QuadPart);
		m_dfFreq = double(qpf.QuadPart);
	}

	// create heap storage for color pixel data in RGBX format
	m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];

	// create heap storage for depth pixel data in RGBX format
	m_pDepthRGBX = new RGBQUAD[cDepthWidth * cDepthHeight];

	// create heap storage for Saver Application
	e_pstreamsaver = new StreamSaver;

	counter = 0;
	temp = 0;
	Enable = true;
}
  

/// <summary>
/// Destructor
/// </summary>
CKinectSources::~CKinectSources()
{
    DiscardDirect2DResources();

	// clean up Direct2D renderer
	if (m_pDrawColor)
	{
		delete m_pDrawColor;
		m_pDrawColor = NULL;
	}

	if (m_pColorRGBX)
	{
		delete[] m_pColorRGBX;
		m_pColorRGBX = NULL;
	}

	// clean up Direct2D renderer
	if (m_pDrawDepth)
	{
		delete m_pDrawDepth;
		m_pDrawDepth = NULL;
	}

	if (m_pDepthRGBX)
	{
		delete[] m_pDepthRGBX;
		m_pDepthRGBX = NULL;
	}

	if (e_pstreamsaver)
	{
		delete e_pstreamsaver;
		e_pstreamsaver = NULL;
	}

	if (e_psaverviewer)
	{
		delete e_psaverviewer;
		e_psaverviewer = NULL;
	}
    // clean up Direct2D
    SafeRelease(m_pD2DFactory);

	// done with frame reader
	SafeRelease(m_pMultiSourceFrameReader);

    // done with body frame reader
   // SafeRelease(m_pBodyFrameReader);

     //done with coordinate mapper
    SafeRelease(m_pCoordinateMapper);

	// done with color frame reader
	//SafeRelease(m_pColorFrameReader);

	// done with depth frame reader
	//SafeRelease(m_pDepthFrameReader);

    // close the Kinect Sensor
    if (m_pKinectSensor)
    {
        m_pKinectSensor->Close();
    }

    SafeRelease(m_pKinectSensor);
}

/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
void CKinectSources::InitThread()
{
	// Create a new thread
	e_hSaveThread = CreateThread(NULL, 0, e_SaveThread, this, 0, NULL);

	// Create an event to stop the savethread
	e_hStopSaveThread = CreateEventW(nullptr, TRUE, FALSE, nullptr);
}

/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
int CKinectSources::Run(HINSTANCE hInstance, int nCmdShow)
{
    MSG       msg = {0};
    WNDCLASS  wc;

    // Dialog custom window class
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.lpfnWndProc   = DefDlgProcW;
	wc.lpszClassName = L"KinectSourcesClass";

    if (!RegisterClassW(&wc))
    {
        return 0;
    }

    // Create main application window
    HWND hWndApp = CreateDialogParamW(
        NULL,
        MAKEINTRESOURCE(IDD_APP),		
        NULL,
		(DLGPROC)CKinectSources::MessageRouter,
        reinterpret_cast<LPARAM>(this));	
	
    // Show window
    ShowWindow(hWndApp, nCmdShow);

	// create thread
	InitThread();
	
	// Main message loop
    while (WM_QUIT != msg.message)
    {
		Update();
		//UpdateBody();
		//UpdateColor();
		//UpdateDepth();

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if (hWndApp && IsDialogMessageW(hWndApp, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

	WaitForSingleObject(e_hSaveThread, INFINITE);
	CloseHandle(e_hSaveThread);

    return static_cast<int>(msg.wParam);
}

/// <summary>
/// Main processing function
/// </summary>
void CKinectSources::Update()
{
	if (!m_pMultiSourceFrameReader)
	{
		return;
	}

	IMultiSourceFrame* pMultiSourceFrame = NULL;
	IDepthFrame* pDepthFrame = NULL;
	IColorFrame* pColorFrame = NULL;
	IBodyIndexFrame* pBodyIndexFrame = NULL;
	IBodyFrame* pBodyFrame = NULL;

	HRESULT hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);
	HRESULT hr_b = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

	if (SUCCEEDED(hr))
	{
		IDepthFrameReference* pDepthFrameReference = NULL;

		hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
		}

		SafeRelease(pDepthFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		IColorFrameReference* pColorFrameReference = NULL;

		hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pColorFrameReference->AcquireFrame(&pColorFrame);
		}

		SafeRelease(pColorFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		IBodyIndexFrameReference* pBodyIndexFrameReference = NULL;
		//IBodyFrameReference*	  pBodyFrameReference = NULL;

		hr = pMultiSourceFrame->get_BodyIndexFrameReference(&pBodyIndexFrameReference);
		//hr = pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameReference);
		HRESULT hr1;
		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameReference->AcquireFrame(&pBodyIndexFrame);			
		}

		SafeRelease(pBodyIndexFrameReference);
		//SafeRelease(pBodyFrameReference);
	}

	
	if (SUCCEEDED(hr))
	{
		INT64 nDepthTime = 0;
		IFrameDescription* pDepthFrameDescription = NULL;
		int nDepthWidth = 0;
		int nDepthHeight = 0;
		UINT nDepthBufferSize = 0;
		UINT16 *pDepthBuffer = NULL;
		USHORT nDepthMinReliableDistance = 0;
		USHORT nDepthMaxReliableDistance = 0;

		IFrameDescription* pColorFrameDescription = NULL;
		int nColorWidth = 0;
		int nColorHeight = 0;
		ColorImageFormat imageFormat = ColorImageFormat_None;
		UINT nColorBufferSize = 0;
		RGBQUAD *pColorBuffer = NULL;

		IFrameDescription* pBodyIndexFrameDescription = NULL;
		int nBodyIndexWidth = 0;
		int nBodyIndexHeight = 0;
		UINT nBodyIndexBufferSize = 0;
		BYTE *pBodyIndexBuffer = NULL;

		// get depth frame data

		hr = pDepthFrame->get_RelativeTime(&nDepthTime);

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_FrameDescription(&pDepthFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameDescription->get_Width(&nDepthWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameDescription->get_Height(&nDepthHeight);
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
			hr = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);
		}

		if (SUCCEEDED(hr))
		{
			ProcessDepth(nDepthTime, pDepthBuffer, nDepthWidth, nDepthHeight, nDepthMinReliableDistance, nDepthMaxReliableDistance);
		}

		// get color frame data

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pColorFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameDescription->get_Width(&nColorWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameDescription->get_Height(&nColorHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
		}

		if (SUCCEEDED(hr))
		{
			if (imageFormat == ColorImageFormat_Bgra)
			{
				hr = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));
			}
			else if (m_pColorRGBX)
			{
				pColorBuffer = m_pColorRGBX;
				nColorBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
				hr = pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, reinterpret_cast<BYTE*>(pColorBuffer), ColorImageFormat_Bgra);
			}
			else
			{
				hr = E_FAIL;
			}
		}

		if (SUCCEEDED(hr))
		{
			ProcessColor(nDepthTime, pColorBuffer, nColorWidth, nColorHeight);
		}

		// get body index frame data

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrame->get_FrameDescription(&pBodyIndexFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameDescription->get_Width(&nBodyIndexWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameDescription->get_Height(&nBodyIndexHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrame->AccessUnderlyingBuffer(&nBodyIndexBufferSize, &pBodyIndexBuffer);
		}			
		
		SafeRelease(pDepthFrameDescription);
		SafeRelease(pColorFrameDescription);
		SafeRelease(pBodyIndexFrameDescription);
	}

	if (SUCCEEDED(hr_b))
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

	SafeRelease(pDepthFrame);
	SafeRelease(pColorFrame);
	SafeRelease(pBodyIndexFrame);
	SafeRelease(pMultiSourceFrame);
	SafeRelease(pBodyFrame);
}

/// <summary>
/// Handles window messages, passes most to the class instance to handle
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CKinectSources::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CKinectSources* pThis = NULL;
    
    if (WM_INITDIALOG == uMsg)
    {
		pThis = reinterpret_cast<CKinectSources*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
		pThis = reinterpret_cast<CKinectSources*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

/// <summary>
/// Handle windows messages for the class instance
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CKinectSources::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;

            // Init Direct2D
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);		

			// Create and initialize a new Direct2D image renderer (take a look at ImageRenderer.h)
			// We'll use this to draw the data we receive from the Kinect to the screen
			m_pDrawColor = new ImageRenderer(); 
			HRESULT hrc = m_pDrawColor->Initialize(GetDlgItem(m_hWnd, IDC_ColorView), m_pD2DFactory, cColorWidth, cColorHeight, cColorWidth * sizeof(RGBQUAD));
			if (FAILED(hrc))
			{
				SetStatusMessage_Color(L"Failed to initialize the Direct2D draw device.", 10000, true);
			}

			// Create and initialize a new Direct2D image renderer (take a look at ImageRenderer.h)
			// We'll use this to draw the data we receive from the Kinect to the screen
			m_pDrawDepth = new ImageRenderer();
			HRESULT hrd = m_pDrawDepth->Initialize(GetDlgItem(m_hWnd, IDC_DepthView), m_pD2DFactory, cDepthWidth, cDepthHeight, cDepthWidth * sizeof(RGBQUAD));
			if (FAILED(hrd))
			{
				SetStatusMessage_Depth(L"Failed to initialize the Direct2D draw device.", 10000, true);
			}

			// Create and initialize a new SaveView
			e_psaverviewer = new SaverViewer;
			//e_psaverviewer->OnInit(GetDlgItem(m_hWnd, IDD_APP));
			e_psaverviewer->OnInit(m_hWnd);

            // Get and initialize the default Kinect sensor
            InitializeDefaultSensor();
        }
        break;

		case WM_COMMAND:
		{
			e_psaverviewer->OnCommand(wParam);			
		}
			break;

        // If the titlebar X is clicked, destroy app
        case WM_CLOSE:
			OnClose(hWnd, wParam);
            //DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            // Quit the main message pump
            PostQuitMessage(0);
            break;
    }

    return FALSE;
}

/// <summary>
/// Initializes the default Kinect sensor
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT CKinectSources::InitializeDefaultSensor()
{
    HRESULT hr;

    hr = GetDefaultKinectSensor(&m_pKinectSensor);
    if (FAILED(hr))
    {
        return hr;
    }

    if (m_pKinectSensor)
    {
        // Initialize the Kinect and get coordinate mapper and the body reader

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_BodyIndex,
				&m_pMultiSourceFrameReader);
			//hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
			//hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
			//hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		}

        IBodyFrameSource* pBodyFrameSource = NULL;
		// get the color reader
		//IColorFrameSource* pColorFrameSource = NULL;
		//get the depth reader
		//IDepthFrameSource* pDepthFrameSource = NULL;

        

        

        if (SUCCEEDED(hr))
        {
            hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
			//hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
			//hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
        }

        if (SUCCEEDED(hr))
        {
            hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
        }

        SafeRelease(pBodyFrameSource);
    }

    if (!m_pKinectSensor || FAILED(hr))
    {
		SetStatusMessage_Body(L"No ready Kinect found!", 10000, true);
        return E_FAIL;
    }

    return hr;
}

/// <summary>
/// Handler function to process WM_CLOSE message
/// </summary>
/// <param name="hWnd">Handle to the window</param>
/// <param name="wParam">Command parameter</param>
void CKinectSources::OnClose(HWND hWnd, WPARAM wParam)
{
	
	// Stop stream event thread
	if (INVALID_HANDLE_VALUE != e_hStopSaveThread)
	{
		SetEvent(e_hStopSaveThread);
	}

	// Destroy the window
	if (nullptr != hWnd)
	{
		DestroyWindow(hWnd);
	}
}


/// <summary>
/// Thread to handle Kinect processing, calls class instance thread processor
/// </summary>
/// <param name="pParam">instance pointer</param>
/// <returns>always 0</returns>
DWORD WINAPI CKinectSources::e_SaveThread(LPVOID pParam)
{
	CKinectSources *pthis = (CKinectSources *)pParam;
	return pthis->e_SaveThread();
}


/// <summary>
/// Thread to handle Kinect processing
/// </summary>
/// <returns>always 0</returns>
///DWORD WINAPI CKinectSources::Nui_ProcessThread(HINSTANCE hInstance, int nCmdShow)
///{
DWORD WINAPI CKinectSources::e_SaveThread()
{
	// Main thread loop
	bool SaveProcessing = true;
	int index = 4;

	while (SaveProcessing)
	{
		// stop event was signalled;
		if (WAIT_OBJECT_0 == WaitForSingleObject(e_hStopSaveThread, 1))
		{
			SaveProcessing = false;
			break;
		}
		// save the color stream
		index = e_pstreamsaver->SaveColorStream(e_psaverviewer->EnStop_Color, e_psaverviewer->FolderName_c, e_psaverviewer->EnColorStatus, e_psaverviewer->RecFPS);

		switch (index)
		{
		case 0:
			e_psaverviewer->SendErrorText(COLOR); //	SendErrorText(Color);
			break;
		case 1:
			e_psaverviewer->SetColorRecordingText(1);
			break;
		case 2:
			e_psaverviewer->SetColorRecordingText(0);
			break;
		default:
			break;
		}

		//e_pstreamsaver->SaveColorStream();

		// save the skeleton stream	
		index = e_pstreamsaver->SaveSkeletonStream(e_psaverviewer->EnStop_Skel);

		switch (index)
		{
		case 0:
			e_psaverviewer->SendErrorText(SKEL); //	SendErrorText(Color);
			break;
		case 1:
			e_psaverviewer->SetSkelRecordingText(1);
			break;
		case 2:
			e_psaverviewer->SetSkelRecordingText(0);
			break;
		default:
			break;
		}

		// save the Depth stream
		index = e_pstreamsaver->SaveDepthStream(e_psaverviewer->EnStop_Depth, e_psaverviewer->FolderName_d, e_psaverviewer->EnDepthStatus, e_psaverviewer->RecFPS);

		switch (index)
		{
		case 0:
			e_psaverviewer->SendErrorText(DEPTH); //	SendErrorText(Color);
			break;
		case 1:
			e_psaverviewer->SetDepthRecordingText(1);
			break;
		case 2:
			e_psaverviewer->SetDepthRecordingText(0);
			break;
		default:
			break;
		}
	}

	return 0;
}
