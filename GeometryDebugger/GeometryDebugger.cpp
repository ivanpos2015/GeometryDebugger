//
// Copyright (C) 2013, Alojz Kovacik, http://kovacik.github.com
//
// This file is part of Geometry Debugger.
//
// Geometry Debugger is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Geometry Debugger is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Geometry Debugger. If not, see <http://www.gnu.org/licenses/>.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "GeometryDebugger.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGeometryDebuggerApp
BEGIN_MESSAGE_MAP(CGeometryDebuggerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CGeometryDebuggerApp::OnAppAbout)
END_MESSAGE_MAP()


// CGeometryDebuggerApp construction
CGeometryDebuggerApp::CGeometryDebuggerApp()
{
	m_bHiColorIcons = TRUE;

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("GeometryDebugger.AppID.1_0"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CGeometryDebuggerApp object

CGeometryDebuggerApp theApp;



int AllocHook( int allocType, void *userData, size_t size, int blockType, long requestNumber, const unsigned char *filename, int lineNumber)
{
    UNREFERENCED_PARAMETER(allocType);
    UNREFERENCED_PARAMETER(userData);
    UNREFERENCED_PARAMETER(size);
    UNREFERENCED_PARAMETER(blockType);
    UNREFERENCED_PARAMETER(requestNumber);
    UNREFERENCED_PARAMETER(filename);
    UNREFERENCED_PARAMETER(lineNumber);

    int b;
    if (allocType == _HOOK_ALLOC)
    {
        if (requestNumber == 332)
        {
            b = 0;
        }
    }


    return TRUE;

}


// CGeometryDebuggerApp initialization

BOOL CGeometryDebuggerApp::InitInstance()
{
    _CrtSetAllocHook(AllocHook);


	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    if (!InitContextMenuManager())
        return false;

	if (!InitKeyboardManager())
        return false;

	if (!InitTooltipManager())
        return false;

    if (!InitSceneGraph())
        return false;

	if (!InitCamera())
		return false;

    if (!InitDebugServer())
        return false;

    CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);




    
	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

int CGeometryDebuggerApp::ExitInstance()
{
    m_DebugServer->Destroy();
    m_DebugServer = NULL;

    m_SceneGraph->Destroy();
    m_SceneGraph = NULL;


	//TODO: handle additional resources you may have added
	return CWinAppEx::ExitInstance();
}



// main running routine until thread exits
int CGeometryDebuggerApp::Run()
{
	_AFX_THREAD_STATE* pState = AfxGetThreadState();

	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;

	// acquire and dispatch messages until a WM_QUIT message is received.
	for (;;)
	{
		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
		{
			// call OnIdle while in bIdle state
			if (!OnIdle(lIdleCount++))
				bIdle = FALSE; // assume "no idle" state
		}

		// phase2: wait for messages while sleeping, update internal idle handlers
        while (!::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
		{    

            CMainFrame *pFrame = (CMainFrame*)m_pMainWnd;
            if (pFrame)
            {
                if (!pFrame->OnIdle())
                    break;
            }

            Sleep(10);
        }

		// phase3: pump messages while available
		do
		{
           
            // pump message, but quit on WM_QUIT
			if (!PumpMessage())
				return ExitInstance();

            CMainFrame *pFrame = (CMainFrame*)m_pMainWnd;
            if (pFrame)
            {
                m_SceneGraph->Validate();
                pFrame->Update();
            }

            // reset "no idle" state after pumping "normal" message
			//if (IsIdleMessage(&m_msgCur))
			if (IsIdleMessage(&(pState->m_msgCur)))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}

        } while (::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE));
	}
}


// CGeometryDebuggerApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CGeometryDebuggerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CGeometryDebuggerApp customization load/save methods

void CGeometryDebuggerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CGeometryDebuggerApp::LoadCustomState()
{
}

void CGeometryDebuggerApp::SaveCustomState()
{
}

// CGeometryDebuggerApp message handlers


BOOL CGeometryDebuggerApp::InitSceneGraph()
{
    HRESULT hr = CreateSceneGraph(&m_SceneGraph);

    return hr == S_OK;
}

BOOL CGeometryDebuggerApp::InitCamera()
{
    HRESULT hr = CreateCamera(L"camera", &m_spCamera);
    return hr == S_OK;
}

BOOL CGeometryDebuggerApp::InitDebugServer()
{
	HRESULT hr = CreateDebugServer(DEBUG_SERVER_DEFAULT_PORT, m_SceneGraph, m_spCamera, &m_DebugServer);
    return hr == S_OK;
}

ISceneGraph* CGeometryDebuggerApp::GetSceneGraph()
{
    return m_SceneGraph;
}

ISceneObjectCamera* CGeometryDebuggerApp::GetCamera()
{
    return m_spCamera;
}