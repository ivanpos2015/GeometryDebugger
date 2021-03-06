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
#include "RenderableDepthMapMesh.h"

using namespace DirectX;

RenderableDepthMapMesh::RenderableDepthMapMesh()
{
	m_VertexCount = 0;
	m_IndexCount = 0;
	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

RenderableDepthMapMesh::~RenderableDepthMapMesh()
{
}

HRESULT RenderableDepthMapMesh::InitializeBuffers(__in ISceneObjectMesh *pMesh)
{
	SmartPtr<IMeshStream> spMeshStream;
	HRESULT hr = pMesh->GetMeshStream(MeshStreamType::MST_VERTEX_POSITIONS, &spMeshStream);
    IF_FAILED_RETURN(hr);

	hr = InitializeVertexBuffer(spMeshStream);
    IF_FAILED_RETURN(hr);

	spMeshStream = NULL;
	hr = pMesh->GetMeshStream(MeshStreamType::MST_TRIANGLES_VERTEX_INDEXES, &spMeshStream);
    IF_FAILED_RETURN(hr);

	return InitializeIndexBuffer(spMeshStream);
}

HRESULT RenderableDepthMapMesh::InitializeVertexBuffer(__in IMeshStream *pStream)
{
    if (!pStream)
    {
        return E_INVALIDARG;
    }

	HRESULT hr;
	
	SmartPtr<IVertexStream> spVertexStream;
	hr = pStream->QueryInterface(&spVertexStream);

	if (SUCCEEDED(hr))
	{
		VertexType* vertices = NULL;
		hr = spVertexStream->Lock(&vertices);
				
		if (SUCCEEDED(hr) && vertices)
		{
			UINT streamItemSize = pStream->GetItemSize();
			UINT streamLength = pStream->GetLength();

			// Set up the description of the static vertex buffer
			D3D11_BUFFER_DESC vertexBufferDesc;
			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = streamItemSize * streamLength;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			// Give the subresource structure a pointer to the vertex data
			D3D11_SUBRESOURCE_DATA vertexData;
			vertexData.pSysMem = vertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			// Now create the vertex buffer
            m_spVertexBuffer = NULL;
			hr = m_spDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_spVertexBuffer);

            IF_FAILED_RETURN(hr);

            m_VertexCount = streamLength;
            hr = spVertexStream->Unlock();
		}
	}

	return hr;
}

HRESULT RenderableDepthMapMesh::InitializeIndexBuffer(__in IMeshStream *pStream)
{
    if (!pStream)
    {
        return E_INVALIDARG;
    }

	HRESULT hr;

	SmartPtr<IIndexStream> spIndexStream;
	hr = pStream->QueryInterface(&spIndexStream);

	if (spIndexStream != NULL)
	{
		unsigned long* indices = NULL;
		hr = spIndexStream->Lock(&indices);
				
		if (SUCCEEDED(hr) && indices)
		{
			UINT streamItemSize = pStream->GetItemSize();
			UINT streamLength = pStream->GetLength();

			// Set up the description of the static index buffer
			D3D11_BUFFER_DESC indexBufferDesc;
			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = streamItemSize * streamLength;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;
				
			// Give the subresource structure a pointer to the index data
			D3D11_SUBRESOURCE_DATA indexData;
			indexData.pSysMem = indices;
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;
				
			// Create the index buffer
            m_spIndexBuffer = NULL;
			hr = m_spDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_spIndexBuffer);

            IF_FAILED_RETURN(hr);
			
            m_IndexCount = streamLength;

            hr = spIndexStream->Unlock();	
		}
	}

	return hr;
}

HRESULT RenderableDepthMapMesh::UpdateFromSceneObject(__in ISceneObject *pSceneObject)
{
	if (!pSceneObject)
	{
		return E_INVALIDARG;
	}

    SmartPtr<ISceneObjectMesh> spMesh;
    HRESULT hr = pSceneObject->GetInterface(__uuidof(ISceneObjectMesh), (IUnknown**)&spMesh);
	
	if (spMesh != NULL)
	{
		m_spSceneObject = pSceneObject;

		// Primitive topology type convertor
		m_PrimitiveTopology = (D3D11_PRIMITIVE_TOPOLOGY)spMesh->GetPrimitiveTopologyType();
		
		hr = InitializeBuffers(spMesh);
	}

	return hr;
}

UINT RenderableDepthMapMesh::GetRequiredPasses()
{
	return 1;
}

HRESULT RenderableDepthMapMesh::GetCameraMatricesFromRenderer(__in IRendererHandler *pRenderer, __out XMMATRIX *viewMatrix, __out XMMATRIX *projectionMatrix)
{
	SmartPtr<IRenderer> spRenderer;
	HRESULT hr = pRenderer->QueryInterface(&spRenderer);
    IF_FAILED_RETURN(hr);

	SmartPtr<ISceneObjectCamera> spCamera;
	hr = spRenderer->GetCamera(&spCamera);
    IF_FAILED_RETURN(hr);

	hr = spCamera->GetViewMatrix(viewMatrix);
    IF_FAILED_RETURN(hr);

	return spCamera->GetProjectionMatrix(projectionMatrix);
}

HRESULT RenderableDepthMapMesh::Draw(UINT passIndex, __in IRendererHandler *pRenderer)
{	
	UNREFERENCED_PARAMETER(passIndex);

    // Get transform matrices 
	XMMATRIX viewMatrix, projectionMatrix;
    HRESULT hr = pRenderer->GetCameraMatrices(&viewMatrix, &projectionMatrix);
    IF_FAILED_RETURN(hr);

	// Get suitable shader
	SmartPtr<IShader> spShader;
	hr = pRenderer->GetShaderInterface(__uuidof(IDepthMapShader), &spShader);
    IF_FAILED_RETURN(hr);
		
	// Get object world transform
	XMMATRIX worldMatrix = m_spSceneObject->GetWorldTransform();

	// Fill shader constants with transformations
	hr = spShader.CastTo<IDepthMapShader>()->SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix);
    IF_FAILED_RETURN(hr);

	spShader->SetVertextAndPixelShader();
			
    pRenderer->DrawIndexedPrimitive(&m_spVertexBuffer.p, sizeof(VertexType), m_spIndexBuffer, DXGI_FORMAT_R32_UINT, 
										 m_IndexCount, m_PrimitiveTopology);

	return hr;
}

HRESULT RenderableDepthMapMesh::Initialize(__in ID3D11Device *pDevice, __in ISceneObject *pSceneObject)
{
	if (!pDevice || !pSceneObject)
	{
		return E_INVALIDARG;
	}

	m_spDevice = pDevice;

	return UpdateFromSceneObject(pSceneObject);
}