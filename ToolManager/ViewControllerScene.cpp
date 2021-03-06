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


#include "StdAfx.h"
#include "ViewControllerScene.h"


ViewControllerScene::ViewControllerScene(void)
{
}

ViewControllerScene::~ViewControllerScene(void)
{
}

HRESULT ViewControllerScene::Initialize()
{
    return S_OK;
}

HRESULT ViewControllerScene::SetRenderer(__in IRenderer *pRenderer)
{
    if (!pRenderer)
        return E_INVALIDARG;

    m_Renderer = pRenderer;
    return S_OK;
}

HRESULT ViewControllerScene::GetRenderer(__deref_out_opt IRenderer **ppRenderer)
{
    if (!ppRenderer)
        return E_POINTER;
    
    return m_Renderer.CopyToOpt(ppRenderer);
}


HRESULT CreateSceneViewController(__deref_out IViewControllerScene **ppViewController)
{
    if (!ppViewController)
    {
        return E_POINTER;
    }

    return ViewControllerScene::CreateInstance(ppViewController);
}