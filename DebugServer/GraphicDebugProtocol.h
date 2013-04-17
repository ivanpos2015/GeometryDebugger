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

namespace GraphicDebugProtocol
{

const WORD DEBUG_SERVER_DEFAULT_PORT = 8318;
const size_t PAYLOAD_SIZE = 512;
const size_t POINT_CHUNK_LENGTH = 30; // The point cloud Message size must be <= PAYLOAD_SIZE

#pragma pack(push, 1)

enum MessageId
{
    GDPID_CLEAR,
    GDPID_CAMERA2D,
    GDPID_CAMERA3D,
    GDPID_LINE,
    GDPID_VECTOR,
    GDPID_COORDINATE_SYSTEM,
    GDPID_TEXT_MARKER,
    GDPID_TRACE,
    GDPID_BEGINGROUP,
    GDPID_ENDGROUP,
    GDPID_ADD_TO_POINT_CLOUD,
    GDPID_END_POINT_CLOUD,
};

struct MessageData_Header
{
    BYTE marker;
    DWORD id;
    DWORD depth;
};

struct Message
{
    MessageData_Header hdr;
    char payload[PAYLOAD_SIZE];
};

struct Vector3
{
    float x,y,z;
};

struct ColoredVertex
{
	Vector3 position;
	DWORD color;
};

struct MessageData_Empty
{
    
};

struct MessageData_Clear
{
    
};



struct MessageData_Camera2D
{
    Vector3 position;
    float scale;
};

struct MessageData_Camera3D
{
    Vector3 position;
    Vector3 right;
    Vector3 up;
    Vector3 front;
    float fov;
};

struct MessageData_Line
{
    DWORD color;
    Vector3 v1;
    Vector3 v2;
};

struct MessageData_Vector
{
    DWORD color;
    Vector3 pos;
    Vector3 dir;
    float scale;
};

struct MessageData_CoordinateSystem
{
    Vector3 axis1;
    Vector3 axis2;
    Vector3 axis3;
    float scale;
};

struct MessageData_TextMarker
{
    Vector3 pos;
    Vector3 axis1;
    Vector3 axis2;
    char text[128];
};

struct MessageData_Trace
{
    char text[128];
};

struct MessageData_PointCloud
{
    UINT size;
    ColoredVertex pointList[POINT_CHUNK_LENGTH]; 
};



#pragma pack(pop)
};