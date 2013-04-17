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
#include "DebugServer.h"

#include "NetworkReadStream.h"
#include "GraphicDebugProtocol.h"

using namespace GraphicDebugProtocol;
using namespace DirectX;
using namespace DirectX::PackedVector;

DebugServer::DebugServer(void)
{
    m_Initialized = false;
    m_Terminate = false;
}

DebugServer::~DebugServer(void)
{
    if (m_Initialized)
    {
        WSACleanup();
    }
}

HRESULT DebugServer::Destroy()
{
    if (m_Terminate)
        return E_FAIL;

    m_spSceneGraphEditor->Destroy();

    m_Terminate = true;
    m_Worker.WaitForFinish();

    return S_OK;
}

HRESULT DebugServer::Initialize(WORD port, __in ISceneGraph *pSceneGraph, __in ISceneObjectCamera *pCamera)
{
    m_Port = port;

    int iResult;
    WSADATA wsaData = {0};
    HRESULT hr;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        iResult = WSAGetLastError();
        return HRESULT_FROM_WIN32(iResult);
    }
    else
    {
        m_Initialized = true;

		hr = CreateSceneGraphEditor(pSceneGraph, &m_spSceneGraphEditor);
        if (SUCCEEDED(hr))
        {            
			m_spCamera = pCamera;
		
            hr = m_Worker.Start(this);
        }
    }
    return hr;
}

HRESULT DebugServer::Invoke()
{
    int iResult;
    addrinfo hints, *result = NULL;
    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    char port[16];
    _itoa_s(m_Port, port, 10);
    getaddrinfo(NULL, port, &hints, &result);

    SOCKET listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    iResult = bind( listenSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        iResult = WSAGetLastError();
        return HRESULT_FROM_WIN32(iResult);
    }

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10*1000;  // 10 ms

    iResult = listen( listenSocket, SOMAXCONN );
    if (iResult == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        iResult = WSAGetLastError();
        return HRESULT_FROM_WIN32(iResult);
    }

    while (!m_Terminate)
    {
        fd_set readSet;
        readSet.fd_count = 1;
        readSet.fd_array[0] = listenSocket;
        iResult = select((int)listenSocket, &readSet, NULL, NULL, &timeout);
        if (iResult >= 1)
        {
            int clientAddressLength;
            sockaddr clientAddress;

            clientAddressLength = sizeof(clientAddress);
            SOCKET clientSocket = accept(listenSocket, &clientAddress, &clientAddressLength);
            if (clientSocket != INVALID_SOCKET)
            {
                InetNtopW(clientAddress.sa_family, clientAddress.sa_data, m_ClientAddress, sizeof(m_ClientAddress));
                HandleClientData(clientSocket);
            }

            iResult = listen(listenSocket, SOMAXCONN);
            if (iResult == SOCKET_ERROR)
            {
                closesocket(listenSocket);
                iResult = WSAGetLastError();
                return HRESULT_FROM_WIN32(iResult);
            }
        }
    }

    closesocket(listenSocket);
    return S_OK;
}

void DebugServer::HandleClientData(SOCKET socket)
{
    NetworkReadStream ns(socket, 100);

    while (!m_Terminate)
    {
        if (ns.CheckDataAvailable(10))
        {
            HRESULT hr = ProcessMessage(ns);
            if (FAILED(hr))
                break;
        }
    }    
}

HRESULT DebugServer::ReadMessageData(NetworkReadStream &ns, BYTE *pData, UINT dataSize)
{
    HRESULT hr;
    BYTE *data = pData;
    UINT dataToRead = dataSize;
    ULONG re;
    while (dataToRead > 0)
    {
        hr = ns.Read(data, dataToRead, &re);
        if (FAILED(hr))
            break;

        if (re != dataToRead)
        {
            hr = E_UNEXPECTED;
            break;
        }

        dataToRead -= re;
        data += re;
    }
    return hr; 
}

HRESULT DebugServer::GetMessageId(NetworkReadStream &ns, UINT *pMessageId)
{
    *pMessageId = 0xFFFFFFFF;
    
    // find Message start (0xFFaabbccdd) where aa > 0. In regular data stream FF is replaced with FF00
    ULONG re;
    HRESULT hr;
    BYTE b;
    for (;;)
    {
        hr = ns.Read(&b, sizeof(b), &re);
        if (FAILED(hr) || (re != 1))
            break;

        if (b == 0xFF)
        {
            hr = ns.Read(&b, sizeof(b), &re);
            if (FAILED(hr) || (re != sizeof(b)))
                break;

            if (b != 0)
            {
                UINT MessageId = 0;
                hr = ns.Read(&MessageId, 3, &re);
                if (FAILED(hr) || (re != 3))
                    break;

                *pMessageId = (MessageId << 8) + (b-1);
                return S_OK;
            }
        }
    }
    return hr;
}

HRESULT DebugServer::ProcessMessage(NetworkReadStream &ns)
{
    UINT MessageId;
    HRESULT hr;

    //get header start
    hr = GetMessageId(ns, &MessageId);
    if (SUCCEEDED(hr))
    {
        hr = ProcessMessage(ns, MessageId);
    }

    return hr;
}

HRESULT DebugServer::ProcessMessage(NetworkReadStream &ns, UINT MessageId)
{
    HRESULT hr = E_UNEXPECTED;
	
	UINT depth = 0;
	hr = ReadMessageData(ns, (BYTE*)&depth, sizeof(DWORD));

	if (SUCCEEDED(hr))
	{
		switch (MessageId)
		{
		case GraphicDebugProtocol::GDPID_CLEAR:
			hr = ClearSceneGraph(ns);
			break;

		case GraphicDebugProtocol::GDPID_CAMERA2D:
			hr = SetCameraPositionAndScale(ns);
			break;

		case GraphicDebugProtocol::GDPID_CAMERA3D:
			hr = SetCameraPositionAtUpFov(ns);
			break;

		case GraphicDebugProtocol::GDPID_LINE:
			hr = CreateLine(ns, depth);
			break;

		case GraphicDebugProtocol::GDPID_VECTOR:
			hr = CreateVector(ns, depth);
			break;

		case GraphicDebugProtocol::GDPID_TEXT_MARKER:
			hr = CreateTextMarker(ns, depth);
			break;

		case GraphicDebugProtocol::GDPID_BEGINGROUP:
			hr = BeginGroup(ns);
			break;

		case GraphicDebugProtocol::GDPID_ENDGROUP:
			hr = EndGroup(ns);
			break;

		case GraphicDebugProtocol::GDPID_TRACE:
			break;

        case GraphicDebugProtocol::GDPID_ADD_TO_POINT_CLOUD:
            hr = AddPointsToPointCloud(ns);
			break;

        case GraphicDebugProtocol::GDPID_END_POINT_CLOUD:
            hr = EndPointCloud(ns, depth);
			break;

		}
	}

    return hr;
}


HRESULT DebugServer::ClearSceneGraph(NetworkReadStream &ns)
{
	BYTE MessageData[sizeof(MessageData_Clear)]; 
	HRESULT hr = ReadMessageData(ns, MessageData, sizeof(MessageData_Clear));
	if (SUCCEEDED(hr))
	{
		m_spSceneGraphEditor->ClearSceneGraph();
	}

	return hr;
}

HRESULT DebugServer::SetCameraPositionAndScale(NetworkReadStream &ns)
{
	MessageData_Camera2D camera2DData; 
	HRESULT hr = ReadMessageData(ns, (BYTE*)&camera2DData, sizeof(MessageData_Camera2D));
	if (SUCCEEDED(hr))
	{
		XMVECTOR position = XMLoadFloat3(&XMFLOAT3(camera2DData.position.x, camera2DData.position.y, camera2DData.position.z));

		m_spCamera->SetPositionAndScale(position, camera2DData.scale);
	}

	return hr;
}		

HRESULT DebugServer::SetCameraPositionAtUpFov(NetworkReadStream &ns)
{
	MessageData_Camera3D camera3DData; 
	HRESULT hr = ReadMessageData(ns, (BYTE*)&camera3DData, sizeof(MessageData_Camera3D));
	if (SUCCEEDED(hr))
	{
		XMVECTOR position = XMLoadFloat3(&XMFLOAT3(camera3DData.position.x, camera3DData.position.y, camera3DData.position.z));
		XMVECTOR at = XMLoadFloat3(&XMFLOAT3(camera3DData.right.x, camera3DData.right.y, camera3DData.right.z));
		XMVECTOR up = XMLoadFloat3(&XMFLOAT3(camera3DData.up.x, camera3DData.up.y, camera3DData.up.z));

		m_spCamera->SetPositionAtUpFov(position, at, up, camera3DData.fov);
	}

	return hr;
}

HRESULT DebugServer::CreateLine(NetworkReadStream &ns, UINT depth)
{
	MessageData_Line lineData; 
	HRESULT hr = ReadMessageData(ns, (BYTE*)&lineData, sizeof(MessageData_Line));
	if (SUCCEEDED(hr))
	{
		XMFLOAT3 origin(lineData.v1.x, lineData.v1.y, lineData.v1.z);
		XMFLOAT3 end(lineData.v2.x, lineData.v2.y, lineData.v2.z);
		XMFLOAT4 color = DWORDColorToXMFLOAT4(lineData.color);

		m_spSceneGraphEditor->CreateLineSegment(origin, end, color, depth);
	}

	return hr;
}

HRESULT DebugServer::CreateVector(NetworkReadStream &ns, UINT depth)
{
	MessageData_Vector vectorData; 
	HRESULT hr = ReadMessageData(ns, (BYTE*)&vectorData, sizeof(MessageData_Vector));
	if (SUCCEEDED(hr))
	{
		XMFLOAT3 origin(vectorData.pos.x, vectorData.pos.y, vectorData.pos.z);
		XMFLOAT3 direction(vectorData.dir.x, vectorData.dir.y, vectorData.dir.z);
		XMFLOAT4 color = DWORDColorToXMFLOAT4(vectorData.color);

		m_spSceneGraphEditor->CreateVector(origin, direction, vectorData.scale, color, depth);
	}

	return hr;
}

HRESULT DebugServer::CreateTextMarker(NetworkReadStream &ns, UINT depth)
{
	MessageData_TextMarker textMarkerData;
	HRESULT hr = ReadMessageData(ns, (BYTE*)&textMarkerData, sizeof(MessageData_TextMarker));
	if (SUCCEEDED(hr))
	{
		WCHAR text[128] = L"";
        size_t length = 0;
        errno_t error = mbstowcs_s(&length, text, textMarkerData.text, 128);
        if (error == 0)
		{
			XMFLOAT3 position(textMarkerData.pos.x, textMarkerData.pos.y, textMarkerData.pos.z);
			m_spSceneGraphEditor->CreateTextMarker(text, position, depth); 
		}
        else
        {
            hr = E_FAIL;
        }
	}

	return hr;
}

HRESULT DebugServer::BeginGroup(NetworkReadStream &ns)
{
	MessageData_Empty emptyData;
	HRESULT hr = ReadMessageData(ns, (BYTE*)&emptyData, sizeof(MessageData_Empty));
	if (SUCCEEDED(hr))
	{
		m_spSceneGraphEditor->BeginGroup();
	}

	return hr;
}

HRESULT DebugServer::EndGroup(NetworkReadStream &ns)
{
	MessageData_Empty emptyData;
	HRESULT hr = ReadMessageData(ns, (BYTE*)&emptyData, sizeof(MessageData_Empty));
	if (SUCCEEDED(hr))
	{
		m_spSceneGraphEditor->EndGroup();
	}

	return hr;
}

HRESULT DebugServer::AddPointsToPointCloud(NetworkReadStream &ns)
{
    MessageData_PointCloud pointCloudData;
	HRESULT hr = ReadMessageData(ns, (BYTE*)&pointCloudData, sizeof(pointCloudData));

	if (SUCCEEDED(hr))
	{
        Vector<VertexType> pointList;
        for (UINT i = 0; i < pointCloudData.size; ++i)
        {
            VertexType point;
            errno_t error = memcpy_s(&point.position, sizeof(point.position), &(pointCloudData.pointList[i].position), sizeof(pointCloudData.pointList[0].position));
            
            if (error == 0)
            {
                point.color = DWORDColorToXMFLOAT4(pointCloudData.pointList[i].color);

                hr = pointList.Add(point);
            }
            else
            {
                hr = E_FAIL;
            }

            if (FAILED(hr))
            {
                break;
            }
        }

        if (SUCCEEDED(hr))
        {
            m_spSceneGraphEditor->AddPointsToPointCloud(pointList.Ptr(), pointList.Length());
        }
	}

	return hr;
}

HRESULT DebugServer::EndPointCloud(NetworkReadStream &ns, UINT depth)
{
	MessageData_Empty emptyData;
	HRESULT hr = ReadMessageData(ns, (BYTE*)&emptyData, sizeof(MessageData_Empty));
	if (SUCCEEDED(hr))
	{
        m_spSceneGraphEditor->EndPointCloud(depth);
	}

	return hr;
}

XMFLOAT4 DebugServer::DWORDColorToXMFLOAT4(__in const DWORD &dwColor)
{
	XMVECTOR colorVector = XMLoadColor(&XMCOLOR(dwColor));
	XMFLOAT4 color;
	XMStoreFloat4(&color, colorVector);

	return color;
}

HRESULT CreateDebugServer(WORD port, __in ISceneGraph *pSceneGraph, __in ISceneObjectCamera *pCamera, __deref_out IDebugServer **ppDebugServer)
{
    if (!pSceneGraph)
        return E_INVALIDARG;

	if (!pCamera)
        return E_INVALIDARG;

    if (!ppDebugServer)
        return E_POINTER;

    return DebugServer::CreateInstance(ppDebugServer, port, pSceneGraph, pCamera);
};