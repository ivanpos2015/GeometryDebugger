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

#include "RenderableMesh.h"
#include "RenderableText.h"
#include "RenderableDepthMapMesh.h"


//--------------------------------------------------------------------------------------
// Factory for renderable objects
//--------------------------------------------------------------------------------------
HRESULT CreateRenderableShape(__in ID3D11Device *pDevice, __in ID3D11DeviceContext* pDeviceContext, __in ISceneObject *pSceneObject, __in IDxFont *pFont, __deref_out IRenderableShape **ppResult)
{
    SmartPtr<ISceneObjectDepthMapMesh> spDepthMapMesh;
    HRESULT hr = pSceneObject->QueryInterface(&spDepthMapMesh);

    if (spDepthMapMesh != NULL)
    {
        return RenderableDepthMapMesh::CreateInstance(ppResult, pDevice, pSceneObject);
    }

    SmartPtr<ISceneObjectMesh> spMesh;
    hr = pSceneObject->QueryInterface(&spMesh);

    if (spMesh == NULL)
    {	
        hr = pSceneObject->GetInterface(__uuidof(ISceneObjectMesh), (IUnknown**)&spMesh);
    }

    if (spMesh != NULL)
    {
        hr = RenderableMesh::CreateInstance(ppResult, pDevice, pSceneObject);
    }

    if (spMesh == NULL)
    {
        SmartPtr<ISceneObjectText> spText;
        hr = pSceneObject->QueryInterface(&spText);

        if (SUCCEEDED(hr))
        {
            hr = RenderableText::CreateInstance(ppResult, pDevice, pDeviceContext, pSceneObject, pFont);
        }
    }

    return hr;
}