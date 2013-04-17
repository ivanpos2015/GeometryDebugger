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
using namespace DirectX;


int _tmain(int argc, _TCHAR* argv[])
{
    //
    // Initialization part
    //
    SmartPtr<ISceneGraph> spSceneGraph;
    HRESULT hr = CreateSceneGraph(&spSceneGraph);
    IF_FAILED_RETURN_INT(hr, ExitCode::UnknownError);

    SmartPtr<ISceneGraphEditor> spEditor;
    hr = CreateSceneGraphEditor(spSceneGraph, &spEditor);
    IF_FAILED_RETURN_INT(hr, ExitCode::UnknownError);

    UINT width = 100, height = 100;

    SmartPtr<IRenderer> spRenderer;
    hr = CreateRendererDirectX11(width, height, spSceneGraph, &spRenderer);
    IF_FAILED_RETURN_INT(hr, ExitCode::UnknownError);
	
    
    // 
    // Snapshot part
    //
    VertexType pVertices[3];
    pVertices[0].position = XMFLOAT3(-45.0f, -45.0f, 750.0f); // Bottom left
    pVertices[0].color = XMFLOAT4(0.0f, 45.0f, 0.0f, 45.0f);
		
    pVertices[1].position = XMFLOAT3(0.0f, 45.0f, 750.0f);  // Top middle.
    pVertices[1].color = XMFLOAT4(0.0f, 45.0f, 0.0f, 45.0f);

    pVertices[2].position = XMFLOAT3(45.0f, -45.0f, 750.0f);  // Bottom right.
    pVertices[2].color = XMFLOAT4(0.0f, 45.0f, 0.0f, 45.0f);

    unsigned long pIndices[3];

    // Load the index array with data.
    pIndices[0] = 0;  // Bottom left.
    pIndices[1] = 1;  // Top middle.
    pIndices[2] = 2;  // Bottom right.

    spEditor->CreateDepthMapMeshObject(pVertices, 3, pIndices, 3);

    SmartPtr<ISceneObjectCamera> spCamera;
    hr = CreateCamera(L"camera", &spCamera);
    IF_FAILED_RETURN_INT(hr, ExitCode::UnknownError);

    hr = spRenderer->SetCamera(spCamera);
    IF_FAILED_RETURN_INT(hr, ExitCode::UnknownError);

    hr = spCamera->SetProjectionMatrix(XMMatrixOrthographicLH((FLOAT)width, (FLOAT)height, 0.0f, 1000.0f));
    IF_FAILED_RETURN_INT(hr, ExitCode::UnknownError);

    // Replace the string parameter with file path for storing snapshot data
    hr = spRenderer->TakeDepthRenderTargetSnapshot(L"your_path_goes_here");
    IF_FAILED_RETURN_INT(hr, ExitCode::UnknownError);

    return 0;
}

