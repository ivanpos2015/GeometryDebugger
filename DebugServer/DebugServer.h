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

class DebugServer : public ImplementSmartObject
    <
        DebugServer, 
        IDebugServer
    >
{
    AsyncOperation<DebugServer> m_Worker;

    bool m_Terminate;
    wchar_t m_ClientAddress[128];
    
    WORD m_Port;
    bool m_Initialized;

    SmartPtr<ISceneGraphEditor> m_spSceneGraphEditor;
	SmartPtr<ISceneObjectCamera> m_spCamera;

    void HandleClientData(SOCKET socket);
    HRESULT ProcessMessage(class NetworkReadStream &ns);
    HRESULT ProcessMessage(NetworkReadStream &ns, UINT MessageId);

    static HRESULT ReadMessageData(NetworkReadStream &ns, BYTE *pData, UINT dataSize);
    static HRESULT GetMessageId(NetworkReadStream &ns, UINT *pMessageId);
	static DirectX::XMFLOAT4 DWORDColorToXMFLOAT4(__in const DWORD &dwColor);

	HRESULT ClearSceneGraph(NetworkReadStream &ns);
	HRESULT SetCameraPositionAndScale(NetworkReadStream &ns);
	HRESULT SetCameraPositionAtUpFov(NetworkReadStream &ns);
	HRESULT CreateLine(NetworkReadStream &ns, UINT depth);
	HRESULT CreateVector(NetworkReadStream &ns, UINT depth);
	HRESULT CreateTextMarker(NetworkReadStream &ns, UINT depth);
	HRESULT BeginGroup(NetworkReadStream &ns);
	HRESULT EndGroup(NetworkReadStream &ns);
    HRESULT AddPointsToPointCloud(NetworkReadStream &ns);
    HRESULT EndPointCloud(NetworkReadStream &ns, UINT depth);

public:
    DebugServer(void);
    ~DebugServer(void);

    HRESULT Initialize(WORD port, __in ISceneGraph *pSceneGraph, __in ISceneObjectCamera *pCamera);
    HRESULT Destroy();
    HRESULT Invoke();
};

