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
#include "GeometryDebuggerClient.h"

using namespace GraphicDebugProtocol;

GeometryDebuggerClient::GeometryDebuggerClient()
{
    m_WsaInitialized = false;
    m_Connected = false;
    m_Socket = INVALID_SOCKET;
    m_PointChunksSentCounter = 0;
}

GeometryDebuggerClient::~GeometryDebuggerClient()
{
	if (m_Socket != INVALID_SOCKET)
    {
        shutdown(m_Socket, SD_SEND);
    }

	if (!m_Connected)
    {
        closesocket(m_Socket);
    }
    
    if (m_WsaInitialized)
    {
        WSACleanup();
    }
}

HRESULT GeometryDebuggerClient::Initialize()
{
	m_Depth = 0;

    HRESULT hr = E_FAIL;
    int iResult;
    WSADATA wsaData;

    m_Socket = INVALID_SOCKET;
    struct sockaddr_in clientService; 

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult == NO_ERROR) 
    {
        m_WsaInitialized = true;

        m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_Socket != INVALID_SOCKET) 
        {
            clientService.sin_family = AF_INET;
            clientService.sin_addr.s_addr = inet_addr( "127.0.0.1" );
            clientService.sin_port = htons( DEBUG_SERVER_DEFAULT_PORT );

			// 
			int flag = 0;
			int iResult = setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (const char*)&flag, sizeof(int));

			if (iResult != SOCKET_ERROR)
			{
				iResult = connect( m_Socket, (SOCKADDR*) &clientService, sizeof(clientService) );
				if (iResult != SOCKET_ERROR)
				{
					hr = S_OK;
					m_Connected = true;
				}
				else
				{
					iResult = WSAGetLastError();
				}
			}
			else
			{
				iResult = WSAGetLastError();
			}		
        }
    }
    return hr;
}

HRESULT GeometryDebuggerClient::SendMessageData(void *data, UINT size, UINT hdr)
{
    Message d;
    d.hdr.marker = 0xFF;
    d.hdr.id = hdr + 1;
	d.hdr.depth = m_Depth;

    int wr = 0;
    char *pData = (char*)data;
    for (UINT i = 0; i < size; i++)
    {
        d.payload[wr++] = pData[i];
        if (pData[i] == 255)
        {
            d.payload[wr++] = 0;
        }
    }
    
    wr += sizeof(MessageData_Header);

    int iResult = send(m_Socket, (char*)(&d), wr, 0);
    if (iResult == SOCKET_ERROR)
    {
        iResult = WSAGetLastError();
        return HRESULT_FROM_WIN32(iResult);
    }

    // Flush the connection, otherwise can't be sure if the message has been recieved
    Flush();
}

HRESULT GeometryDebuggerClient::Flush()
{
    if (m_Socket != INVALID_SOCKET)
    {
        shutdown(m_Socket, SD_SEND);
    }

	if (!m_Connected)
    {
        closesocket(m_Socket);
    }

    return Initialize();
}

HRESULT GeometryDebuggerClient::Clear()
{
    MessageData_Clear p;
    return SendMessageData(&p, sizeof(p), GDPID_CLEAR);
}

HRESULT GeometryDebuggerClient::Camera(Vector3 pos, Vector3 right, Vector3 up, float fov)
{
    MessageData_Camera3D p;
    p.position = pos;
    p.right = right;
    p.up = up;
    p.fov = fov;
    return SendMessageData(&p, sizeof(p), GDPID_CAMERA3D);
}
   
HRESULT GeometryDebuggerClient::Camera(Vector3 pos, float scale)
{
    MessageData_Camera2D p;
    p.position = pos;
    p.scale = scale;
    return SendMessageData(&p, sizeof(p), GDPID_CAMERA2D);
}
    
HRESULT GeometryDebuggerClient::Line(Vector3 v1, Vector3 v2, DWORD color)
{
    MessageData_Line p;
    p.v1 = v1;
    p.v2 = v2;
    p.color = color;
    return SendMessageData(&p, sizeof(p), GDPID_LINE);
}

HRESULT GeometryDebuggerClient::Vector(Vector3 pos, Vector3 dir, float scale, DWORD color)
{
    MessageData_Vector p;
    p.pos = pos;
    p.dir = dir;
    p.scale = scale;
    p.color = color;
    return SendMessageData(&p, sizeof(p), GDPID_VECTOR);
}

HRESULT GeometryDebuggerClient::Text(Vector3 pos, char text[128])
{
	MessageData_TextMarker p;
	p.pos = pos;
	p.axis1.x = p.axis1.y = p.axis1.z = 0;
	p.axis2.x = p.axis2.y = p.axis2.z = 0;
	strcpy_s(p.text, 128, text);

	return SendMessageData(&p, sizeof(p), GDPID_TEXT_MARKER);
}
    
HRESULT GeometryDebuggerClient::Trace(char *szText)
{
    MessageData_Trace p = {0};
    strcpy_s(p.text, szText);
    return SendMessageData(&p, sizeof(p), GDPID_TRACE);
}

HRESULT GeometryDebuggerClient::BeginGroup()
{
    MessageData_Empty p;
    HRESULT hr = SendMessageData(&p, sizeof(p), GDPID_BEGINGROUP);
    m_Depth++;
    return hr;
}

HRESULT GeometryDebuggerClient::EndGroup()
{    
    MessageData_Empty p;
    HRESULT hr = SendMessageData(&p, sizeof(p), GDPID_ENDGROUP);
    m_Depth--;
    return hr;
};

HRESULT GeometryDebuggerClient::AddPointToPointCloud(GraphicDebugProtocol::ColoredVertex point)
{    
    HRESULT hr = m_PointChunk.Add(point);

    if (m_PointChunk.Length() == POINT_CHUNK_LENGTH)
    {
        MessageData_PointCloud pointCloudChunk;
        pointCloudChunk.size = POINT_CHUNK_LENGTH;
        memcpy_s(&(pointCloudChunk.pointList), sizeof(pointCloudChunk.pointList), m_PointChunk.Ptr(), m_PointChunk.Length() * sizeof(m_PointChunk[0])); 

        hr = SendMessageData(&pointCloudChunk, sizeof(pointCloudChunk), GDPID_ADD_TO_POINT_CLOUD);

        m_PointChunksSentCounter++;

        m_PointChunk.Clear();
    }

    return hr;
};

HRESULT GeometryDebuggerClient::EndPointCloud()
{
    if (m_PointChunk.Length() >= POINT_CHUNK_LENGTH)
    {
        return E_FAIL;
    }

    HRESULT hr = S_OK;

    if (m_PointChunk.Length() > 0)
    {
        
        // Send last chunk of data
        MessageData_PointCloud pointCloudChunk;
        pointCloudChunk.size = m_PointChunk.Length();
        memcpy_s(&(pointCloudChunk.pointList), sizeof(pointCloudChunk.pointList), m_PointChunk.Ptr(), m_PointChunk.Length() * sizeof(m_PointChunk[0])); 

        hr = SendMessageData(&pointCloudChunk, sizeof(pointCloudChunk), GDPID_ADD_TO_POINT_CLOUD);

        m_PointChunksSentCounter++;

        m_PointChunk.Clear();
    }
    
    if (SUCCEEDED(hr))
    {
        MessageData_Empty p;
        hr = SendMessageData(&p, sizeof(p), GDPID_END_POINT_CLOUD);

        m_PointChunksSentCounter = 0;
    }

    return hr;
};