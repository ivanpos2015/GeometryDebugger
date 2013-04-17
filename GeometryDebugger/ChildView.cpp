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



// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "GeometryDebugger.h"
#include "ChildView.h"
#include "..\SceneGraph\SceneGraphLib.h"
#include "..\RendererDirectX11\RendererDirectX11Lib.h"
#include "..\ToolManager\ToolManagerLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
    InitializeToolManager();
}

CChildView::~CChildView()
{
    m_ToolManager->Destroy();
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
    
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_MOUSELEAVE()

    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONUP()
    ON_WM_MBUTTONDOWN()
    ON_WM_MBUTTONDBLCLK()
    ON_WM_MBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONDBLCLK()
    ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

HRESULT CChildView::InitializeRenderer()
{
    HRESULT hr = CreateRendererDirectX11(this->m_hWnd, theApp.GetSceneGraph(), &m_Renderer );
    if (SUCCEEDED(hr))
    {
        hr = m_ViewController->SetRenderer(m_Renderer);

        if (SUCCEEDED(hr))
        {
			hr = m_Renderer->SetCamera(theApp.GetCamera());
        }
    }
    
    return hr;
}

void CChildView::Update()
{
    if (m_Renderer)
    {
        m_Renderer->Draw(true);
    }
}

void CChildView::OnPaint() 
{
    if (m_Renderer == NULL)
    {
        InitializeRenderer();
    }
    else
    {
        RECT rect;
        GetClientRect(&rect);
        m_Renderer->Resize( rect );
    }
	
    CPaintDC dc(this); 

    if (m_Renderer)
    {
        m_Renderer->Draw(true);
    }
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    m_ToolManager->ProcessMouseEvent(nFlags, pt, zDelta);
    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CChildView::OnMouseLeave()
{
    CWnd::OnMouseLeave();
}

void CChildView::OnMouseMove(UINT flags, CPoint pt)
{
    CWnd::OnMouseMove(flags, pt);
    m_ToolManager->ProcessMouseEvent(flags, pt);
}

void CChildView::OnLButtonDown(UINT flags, CPoint pt)
{
    CWnd::OnLButtonDown(flags, pt);
    m_ToolManager->ProcessMouseEvent(flags, pt);
}

void CChildView::OnLButtonDblClk(UINT flags, CPoint pt)
{
    CWnd::OnRButtonDblClk(flags, pt);
    m_ToolManager->ProcessMouseEvent(flags, pt);
}

void CChildView::OnLButtonUp(UINT flags, CPoint pt)
{
    CWnd::OnLButtonUp(flags, pt);
    m_ToolManager->ProcessMouseEvent(flags, pt);
}

void CChildView::OnMButtonDown(UINT flags, CPoint pt)
{
    CWnd::OnMButtonDown(flags, pt);
    m_ToolManager->ProcessMouseEvent(flags, pt);
}

void CChildView::OnMButtonDblClk(UINT flags, CPoint pt)
{
    CWnd::OnRButtonDblClk(flags, pt);
    m_ToolManager->ProcessMouseEvent(flags, pt);
}

void CChildView::OnMButtonUp(UINT flags, CPoint pt)
{
    CWnd::OnMButtonUp(flags, pt);
    m_ToolManager->ProcessMouseEvent(flags, pt);
}

void CChildView::OnRButtonDown(UINT flags, CPoint pt)
{
    CWnd::OnRButtonDown(flags, pt);
    m_ToolManager->ProcessMouseEvent(flags, pt);
}

void CChildView::OnRButtonDblClk(UINT flags, CPoint pt)
{
    CWnd::OnRButtonDblClk(flags, pt);
    m_ToolManager->ProcessMouseEvent(flags, pt);
}

void CChildView::OnRButtonUp(UINT flags, CPoint pt)
{
    CWnd::OnRButtonUp(flags, pt);
    m_ToolManager->ProcessMouseEvent(flags, pt);
}

HRESULT CChildView::InitializeToolManager()
{ 
    HRESULT hr = CreateSceneViewController(&m_ViewController);
    if (SUCCEEDED(hr))
    {
        hr = CreateToolManager(m_ViewController, &m_ToolManager);
        if (SUCCEEDED(hr))
        {
			SmartPtr<IToolSource> spToolSource;
            hr = CreateBackgroundToolSource(m_hWnd, &spToolSource);

            if (SUCCEEDED(hr))
            {
				// Add tool for camera movement and rotation
				SmartPtr<ITool> spCameraTool;
				hr = CreateCameraMoveTool(&spCameraTool);

				if (SUCCEEDED(hr))
				{
					hr = spToolSource->AddTool(spCameraTool);

					if (SUCCEEDED(hr))
					{
						hr = m_ToolManager->AddToolSource(spToolSource);
					}
				}
            }
        }
    }
    return hr;
}

bool CChildView::OnIdle()
{
    return m_ToolManager->OnIdle() != S_OK;
}