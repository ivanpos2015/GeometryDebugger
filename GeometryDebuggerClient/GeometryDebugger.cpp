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
#include "GeometryDebugger.h"
#include "GeometryDebuggerClient.h"

using namespace DirectX;

GeometryDebugger debug;


GeometryDebugger::GeometryDebugger()
{
    m_Instance = NULL;
    m_Initialized = false;
}

GeometryDebugger::~GeometryDebugger()
{
    delete m_Instance;
}

void GeometryDebugger::Initialize()
{
    if (!m_Instance && !m_Initialized)
    {
        m_Initialized = true;
        HRESULT hr;
        m_Instance = new GeometryDebuggerClient();
        if (m_Instance)
        {
            hr = m_Instance->Initialize();
            if (FAILED(hr))
            {
                delete m_Instance;
                m_Instance = NULL;
            }
        }
        else 
        {
            hr = E_OUTOFMEMORY;
        }
    }
}

void GeometryDebugger::Clear()
{
    Initialize();
    if (m_Instance)
    {
        m_Instance->Clear();
    }
}

void GeometryDebugger::Camera(double posx, double posy, double posz, double atx, double aty, double atz, double upx, double upy, double upz, double fov)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 pos, at, up;
        pos.x = (float)posx; pos.y = (float)posy; pos.z = (float)posz;
        at.x = (float)atx; at.y = (float)aty; at.z = (float)atz;
        up.x = (float)upx; up.y = (float)upy; upz = (float)upz;
        m_Instance->Camera(pos, at, up, (float)fov);
    }

}

void GeometryDebugger::Camera(CXMVECTOR pos, CXMVECTOR at, CXMVECTOR up, double fov)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 tpos, tat, tup;
        tpos.x = XMVectorGetX(pos); tpos.y = XMVectorGetY(pos); tpos.z = XMVectorGetZ(pos);
        tat.x = XMVectorGetX(at); tat.y = XMVectorGetY(at); tat.z = XMVectorGetZ(at);
        tup.x = XMVectorGetX(up); tup.y = XMVectorGetY(up); tup.z = XMVectorGetZ(up);
        m_Instance->Camera(tpos, tat, tup, (float)fov);
    }
}

   
void GeometryDebugger::Camera(double posx, double posy, double scale)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 pos;
        pos.x = (float)posx;
        pos.y = (float)posy;
        pos.z = 1;
        m_Instance->Camera(pos, (float)(scale));
    }
}

void GeometryDebugger::Camera(CXMVECTOR pos, double scale)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 tpos;
        tpos.x = XMVectorGetX(pos);
        tpos.y = XMVectorGetY(pos);
        tpos.z = XMVectorGetZ(pos);
        m_Instance->Camera(tpos, (float)(scale));
    }
}
    
void GeometryDebugger::Line(double x1, double y1, double z1, double x2, double y2, double z2, DWORD color)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 v1, v2;
        v1.x = (float)x1;
        v1.y = (float)y1;
        v1.z = (float)z1;
        v2.x = (float)x2;
        v2.y = (float)y2;
        v2.z = (float)z2;
        m_Instance->Line(v1, v2, color);
    }
}

void GeometryDebugger::Line(double x1, double y1, double x2, double y2, DWORD color)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 v1, v2;
        v1.x = (float)x1;
        v1.y = (float)y1;
        v1.z = 0;
        v2.x = (float)x2;
        v2.y = (float)y2;
        v2.z = 0;
        m_Instance->Line(v1, v2, color);
    }
}

void GeometryDebugger::Line(CXMVECTOR v1, CXMVECTOR v2, DWORD color)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 tv1, tv2;
        tv1.x = XMVectorGetX(v1);
        tv1.y = XMVectorGetY(v1);
        tv1.z = XMVectorGetZ(v1);
        tv2.x = XMVectorGetX(v2);
        tv2.y = XMVectorGetY(v2);
        tv2.z = XMVectorGetZ(v2);
        m_Instance->Line(tv1, tv2, color);
    }
}


void GeometryDebugger::Vector(double posx, double posy, double posz, double dirx, double diry, double dirz, double scale, DWORD color)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 pos, dir;
        pos.x = (float)posx;
        pos.y = (float)posy;
        pos.z = (float)posz;
        dir.x = (float)dirx;
        dir.y = (float)diry;
        dir.z = (float)dirz;
        m_Instance->Vector(pos, dir, (float)(scale), color);
    }
}

void GeometryDebugger::Vector(double posx, double posy, double dirx, double diry, double scale, DWORD color)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 tpos, tdir;
        tpos.x = (float)posx;
        tpos.y = (float)posy;
        tpos.z = 0;
        tdir.x = (float)dirx;
        tdir.y = (float)diry;
        tdir.z = 0;
        m_Instance->Vector(tpos, tdir, (float)(scale), color);
    }
}

void GeometryDebugger::Vector(CXMVECTOR pos, CXMVECTOR dir, double size, DWORD color)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 tpos, tdir;
        tpos.x = XMVectorGetX(pos);
        tpos.y = XMVectorGetY(pos);
        tpos.z = XMVectorGetZ(pos);
        tdir.x = XMVectorGetX(dir);
        tdir.y = XMVectorGetY(dir);
        tdir.z = XMVectorGetZ(dir);
        m_Instance->Vector(tpos, tdir, (float)(size), color);
    }
}

void GeometryDebugger::CoordinateSystem(CXMVECTOR pos, CXMVECTOR axis1, CXMVECTOR axis2, CXMVECTOR axis3, double scale)
{
    BeginGroup();
    Vector(pos, axis1, scale, CF_RED);
    Vector(pos, axis2, scale, CF_GREEN);
    Vector(pos, axis3, scale, CF_BLUE);
    EndGroup();
}

void GeometryDebugger::Text(CXMVECTOR pos, char text[128])
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::Vector3 tpos;
        tpos.x = XMVectorGetX(pos);
        tpos.y = XMVectorGetY(pos);
        tpos.z = XMVectorGetZ(pos);

		m_Instance->Text(tpos, text);
    }
}

void GeometryDebugger::TextMarker(CXMVECTOR pos, char text[128])
{
	FLOAT scale = 0.2f;

	XMVECTOR axis1 = XMVectorScale(XMLoadFloat3(&XMFLOAT3(1.0f, 0.0f, 0.0f)), scale);
	XMVECTOR pos1 = XMVectorAdd(pos, XMVectorScale(axis1, -0.5f));

	XMVECTOR axis2 = XMVectorScale(XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)), scale);
	XMVECTOR pos2 = XMVectorAdd(pos, XMVectorScale(axis2, -0.5f));

	XMVECTOR axis3 = XMVectorScale(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 1.0f)), scale);
	XMVECTOR pos3 = XMVectorAdd(pos, XMVectorScale(axis3, -0.5f));

    BeginGroup();

	Vector(pos1, axis1, 1.0f,CF_BLUE);
    Vector(pos2, axis2, 1.0f, CF_BLUE);
	Vector(pos3, axis3, 1.0f, CF_BLUE);

	Text(pos, text);

    EndGroup();
}

void GeometryDebugger::Trace(char *szText)
{
    Initialize();
    if (m_Instance)
    {
        m_Instance->Trace(szText);
    }
}

void GeometryDebugger::AddPointToPointCloud(CXMVECTOR pos, DWORD color)
{
    Initialize();
    if (m_Instance)
    {
        GraphicDebugProtocol::ColoredVertex coloredVertex;
        coloredVertex.position.x = XMVectorGetX(pos);
        coloredVertex.position.y = XMVectorGetY(pos);
        coloredVertex.position.z = XMVectorGetZ(pos);
        coloredVertex.color = color;

        m_Instance->AddPointToPointCloud(coloredVertex);
    }
}

void GeometryDebugger::EndPointCloud()
{
    Initialize();
    if (m_Instance)
    {
        m_Instance->EndPointCloud();
    }
}

void GeometryDebugger::BeginGroup()
{
    Initialize();
    if (m_Instance)
    {
        m_Instance->BeginGroup();
    }
}

void GeometryDebugger::EndGroup()
{
    if (m_Instance)
    {
        m_Instance->EndGroup();
    }
}

