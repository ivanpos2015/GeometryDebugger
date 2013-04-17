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



// GeometryDebugger.h : main header file for the GeometryDebugger application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CGeometryDebuggerApp:
// See GeometryDebugger.cpp for the implementation of this class
//

class CGeometryDebuggerApp : public CWinAppEx
{
    SmartPtr<ISceneGraph> m_SceneGraph;
    SmartPtr<IDebugServer> m_DebugServer;
	SmartPtr<ISceneObjectCamera> m_spCamera;

    BOOL InitSceneGraph();
    BOOL InitDebugServer();
	BOOL InitCamera();

public:
	CGeometryDebuggerApp();


	virtual BOOL InitInstance();
	virtual int ExitInstance();

    ISceneGraph* GetSceneGraph();
	ISceneObjectCamera* GetCamera();

// Implementation

public:
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
    virtual int Run();

    afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGeometryDebuggerApp theApp;
