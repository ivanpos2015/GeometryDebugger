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

class GeometryDebuggerClient
{
    SOCKET m_Socket;
    DWORD m_Depth;
    bool m_WsaInitialized;
    bool m_Connected;
    Vector<GraphicDebugProtocol::ColoredVertex> m_PointChunk;
    UINT m_PointChunksSentCounter;


    HRESULT SendMessageData(void *data, UINT size, UINT hdr);

public:
    GeometryDebuggerClient();
    ~GeometryDebuggerClient();

    HRESULT Initialize();

    HRESULT Flush();

    HRESULT Clear();

    HRESULT Camera(GraphicDebugProtocol::Vector3 pos, GraphicDebugProtocol::Vector3 right, GraphicDebugProtocol::Vector3 up, float fov);
   
    HRESULT Camera(GraphicDebugProtocol::Vector3 pos, float scale);
    
    HRESULT Line(GraphicDebugProtocol::Vector3 v1, GraphicDebugProtocol::Vector3 v2, DWORD color);

    HRESULT Vector(GraphicDebugProtocol::Vector3 pos, GraphicDebugProtocol::Vector3 dir, float size, DWORD color);
    
    HRESULT Trace(char *szText);

    HRESULT BeginGroup();

    HRESULT EndGroup();

	HRESULT Text(GraphicDebugProtocol::Vector3 pos, char text[128]);

    HRESULT AddPointToPointCloud(GraphicDebugProtocol::ColoredVertex point);

    HRESULT EndPointCloud();
};

