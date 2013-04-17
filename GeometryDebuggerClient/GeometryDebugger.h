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

enum GeometryDebuggerColor
{
    CF_RED = 0xFFFF0000,
    CF_GREEN = 0xFF00FF00,
    CF_BLUE = 0xFF0000FF,
};

class GeometryDebugger
{
    class GeometryDebuggerClient* m_Instance;
    bool m_Initialized;

    void Initialize();
public:
    GeometryDebugger();
    ~GeometryDebugger();

    void Clear();

    void Camera(double posx, double posy, double posz, double atx, double aty, double atz, double upx, double upy, double upz, double fov);
    void Camera(DirectX::CXMVECTOR pos, DirectX::CXMVECTOR at, DirectX::CXMVECTOR up, double fov);
   
    void Camera(double posx, double posy, double scale);
    void Camera(DirectX::CXMVECTOR pos, double scale);
    
    void Line(double x1, double y1, double z1, double x2, double y2, double z2, DWORD color);
    void Line(double x1, double y1, double x2, double y2, DWORD color);
    void Line(DirectX::CXMVECTOR v1, DirectX::CXMVECTOR v2, DWORD color);

    void Vector(double posx, double posy, double posz, double dirx, double diry, double dirz, double scale, DWORD color);
    void Vector(double posx, double posy, double dirx, double diry, double scale, DWORD color);
    void Vector(DirectX::CXMVECTOR pos, DirectX::CXMVECTOR dir, double size, DWORD color);
    
    void CoordinateSystem(DirectX::CXMVECTOR pos, DirectX::CXMVECTOR axis1, DirectX::CXMVECTOR axis2, DirectX::CXMVECTOR axis3, double scale);

    void Trace(char *szText);

    void BeginGroup();
    void EndGroup();

	void Text(DirectX::CXMVECTOR pos, char text[128]);
	void TextMarker(DirectX::CXMVECTOR pos, char text[128]);

    void AddPointToPointCloud(DirectX::CXMVECTOR pos, DWORD color);
    void EndPointCloud();
};

#pragma comment(lib, "ws2_32.lib")

extern GeometryDebugger debug;