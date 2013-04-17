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


#pragma once

class RenderableDZImage : public ImplementSmartObject
    <
        RenderableDZImage, 
        ClassFlags<CF_ALIGNED_MEMORY>,
        IRenderableDZImage
    >
{
private:

    struct TexturedVertexType
	{
		XMFLOAT3 Position;
		XMFLOAT2 Texture;
		XMFLOAT4 Color;
	};

	SmartPtr<ISceneObjectDZImage> m_spSceneObjectDZImage;

	UINT m_TexWidth;
	UINT m_TexHeight;

	SmartPtr<ID3D11ShaderResourceView> m_spFontSheet;
	SmartPtr<IDxFont> m_spFont;

	SmartPtr<ID3D11Buffer> m_spVertexBuffer;
	SmartPtr<ID3D11Buffer> m_spIndexBuffer;
	int m_VertexCount, m_IndexCount;

	SmartPtr<ID3D11Device> m_spDevice; 
    SmartPtr<ID3D11DeviceContext> m_spDeviceContext;

	HRESULT InitializeBuffers(__in ISceneObjectMesh *pMesh);
	HRESULT InitializeVertexBuffer( __in IMeshStream *pStream);
	HRESULT InitializeIndexBuffer(__in IMeshStream *pStream);

public:
    RenderableDZImage();
    ~RenderableDZImage();

    HRESULT UpdateFromSceneObject(__in ISceneObject *pSceneObject);
    UINT    GetRequiredPasses();
    HRESULT Draw(UINT passIndex, __in IRendererHandler *pRenderer);

	HRESULT Initialize(__in ID3D11Device *pDevice, __in ID3D11DeviceContext* pDeviceContext, __in ISceneObject *pSceneObject);
    HRESULT GetInterface(const GUID &extensionId, __deref_out IUnknown **ppInterface);
};