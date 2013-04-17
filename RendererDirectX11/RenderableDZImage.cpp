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
#include "RenderableDZImage.h"

RenderableDZImage::RenderableDZImage()
{
    m_IndexCount = 0;
    m_VertexCount = 0;
}

RenderableDZImage::~RenderableDZImage()
{
}

UINT RenderableDZImage::GetRequiredPasses()
{
    return 1;
}

HRESULT RenderableDZImage::UpdateFromSceneObject(__in ISceneObject *pSceneObject)
{
    if (!pSceneObject)
    {
        return E_INVALIDARG;
    }

    SmartPtr<ISceneObjectDZImage> spImage;
    HRESULT hr = pSceneObject->QueryInterface(&spImage);

    if (spImage != NULL)
    {
        SmartPtr<ISceneObjectMesh> spMesh;
        hr = spImage->GetInterface(__uuidof(ISceneObjectMesh), (IUnknown**)&spMesh);

        if (SUCCEEDED(hr))
        {
            m_spSceneObjectDZImage = spImage;
            hr = InitializeBuffers(spMesh);
        }
    }

    return hr;
}

HRESULT RenderableDZImage::InitializeBuffers(__in ISceneObjectMesh *pMesh)
{
	if (!pMesh)
	{
		return E_INVALIDARG;
	}

	if (!m_spDevice)
	{
		return E_FAIL;
	}

	SmartPtr<IMeshStream> spMeshStream;
	HRESULT hr = pMesh->GetMeshStream(MeshStreamType::MST_VERTEX_POSITIONS, &spMeshStream);

	if (SUCCEEDED(hr))
	{
		hr = InitializeVertexBuffer(spMeshStream);

		if (SUCCEEDED(hr))
		{
			spMeshStream = NULL;
			hr = pMesh->GetMeshStream(MeshStreamType::MST_TRIANGLES_VERTEX_INDEXES, &spMeshStream);

			if (SUCCEEDED(hr))
			{
				hr = InitializeIndexBuffer(spMeshStream);
			}
			else
			{
				// Return S_OK if no index buffer found
				return S_OK;
			}
		}
	}

	return hr;
}

HRESULT RenderableDZImage::InitializeVertexBuffer(__in IMeshStream *pStream)
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

HRESULT RenderableDZImage::InitializeIndexBuffer(__in IMeshStream *pStream)
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

//--------------------------------------------------------------------------------------
// Factory for renderable objects
//--------------------------------------------------------------------------------------
LPDIRECTDRAWSURFACE CreateD3DTexture( LPDIRECTDRAW lpDD,
                                  	int width, int height,
                                  	int mipmap,
                                  	DDPIXELFORMAT *pixelFormat)
{
  LPDIRECTDRAWSURFACE surface;  /* pointer to surface to be created */
  DDSURFACEDESC ddsd;       	/* description of surface to create */
  DWORD mipLevels = 1;      	/* number of mip-map levels to create
                               	(1 = just original texture) */
  DWORD flags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_CAPS;
  DWORD caps = DDSCAPS_TEXTURE | DDSCAPS_3DDEVICE;
  HRESULT result;

  /* put texture in video memory if driver allows it */
  if (VideoMemoryTexturesAllowed())
	caps |= DDSCAPS_VIDEOMEMORY;
  else
	caps |= DDSCAPS_SYSTEMMEMORY;

  if (mipmap)
  {
	/* count how many mip-map levels we need */
	int mipWidth = width;
	int mipHeight = height;

	/* smallest mip-map we want is 2 x 2 */
	while ((mipWidth > 2) && (mipHeight > 2))
	{
  	mipLevels++;
  	mipWidth  /= 2;
  	mipHeight /= 2;
	}

	if (mipLevels > 1)
	{
  	/* tell it we want mip-maps */
  	flags |= DDSD_MIPMAPCOUNT;
  	caps  |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
	}
  }

  /* set up buffer properties */
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize      	= sizeof(ddsd);
  ddsd.dwFlags     	= flags;
  ddsd.dwWidth     	= width;
  ddsd.dwHeight    	= height;
  ddsd.ddpfPixelFormat = *pixelFormat;
  ddsd.ddsCaps.dwCaps  = caps;
  ddsd.dwMipMapCount   = mipLevels;

  /* create texture surface and associated mip-maps */
  result = IDirectDraw_CreateSurface( lpDD, &ddsd, &surface, NULL);

  if (result != DD_OK) return NULL;

  return surface;
}

HRESULT RenderableDZImage::Draw(UINT passIndex, __in IRendererHandler *pRenderer)
{	
    UNREFERENCED_PARAMETER(passIndex);

    XMMATRIX viewMatrix, projectionMatrix;
    HRESULT hr = pRenderer->GetCameraMatrices(&viewMatrix, &projectionMatrix);
    IF_FAILED_RETURN(hr);

    // Get suitable renderer
    SmartPtr<IRenderer> spRenderer;
    hr = pRenderer->QueryInterface(&spRenderer);
    IF_FAILED_RETURN(hr);

    UINT screenWidth = 0, screenHeight = 0;
    spRenderer->GetWindowWidthAndHeight(screenWidth, screenHeight);

   
    // TODO: Get texture tile streams on demand (depending on viewport)

    // Get font shader wrapper
    SmartPtr<IShader> spShader;
    hr = pRenderer->GetShaderInterface(__uuidof(IFontShader), &spShader);
    IF_FAILED_RETURN(hr);

    // Get 2D projection matrices
    projectionMatrix = XMMatrixOrthographicLH((FLOAT)screenWidth, (FLOAT)screenHeight, 1.0f, 1000.0f);
	viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, -1.0f)), XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f)), XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)));

    // Fill shader constants with transformation matrices
    hr = spShaderWrapper.CastTo<IFontShader>()->SetShaderParametersAndTexture(worldMatrix, viewMatrix, projectionMatrix, m_spFontSheet);
    IF_FAILED_RETURN(hr);

    spShaderWrapper->SetVertextAndPixelShader();

    pRenderer->TurnOnAlphaBlending();

    ID3D11Buffer* vertexBuffer = m_spVertexBuffer;
    pRenderer->DrawIndexedPrimitive(&vertexBuffer, sizeof(TexturedVertexType), m_spIndexBuffer, DXGI_FORMAT_R32_UINT, 
        m_IndexCount, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pRenderer->TurnOffAlphaBlending();		

    return hr;
}

HRESULT RenderableDZImage::Initialize(__in ID3D11Device *pDevice, __in ID3D11DeviceContext* pDeviceContext, __in ISceneObject *pSceneObject)
{
    if (!pDevice || !pDeviceContext || !pSceneObject)
    {
        return E_INVALIDARG;
    }

    m_spDevice = pDevice;
    m_spDeviceContext = pDeviceContext;

    return UpdateFromSceneObject(pSceneObject);
}
