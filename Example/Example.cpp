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


// Example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace DirectX;

#define COLOR_GREEN 0xff00ff55

int _tmain(int argc, _TCHAR* argv[])
{
    //
    // Clear whole scene graph
    //
	debug.Clear();

    //
    // Try debugging the Cube construction ;-)
    //
    debug.Line(-20.0f, 0.0f, 50.0f, 20.0f, 0.0f, 50.0f, COLOR_GREEN);
	debug.Line(-20.0f, 20.0f, 50.0f, 20.0f, 20.0f, 50.0f, COLOR_GREEN);
    debug.Line(-20.0f, 0.0f, 50.0f, -20.0f, 20.0f, 50.0f, COLOR_GREEN);
	debug.Line(20.0f, 0.0f, 50.0f, 20.0f, 20.0f, 50.0f, COLOR_GREEN);

    debug.Line(-20.0f, 0.0f, 30.0f, 20.0f, 0.0f, 30.0f, COLOR_GREEN);
	debug.Line(-20.0f, 20.0f, 30.0f, 20.0f, 20.0f, 30.0f, COLOR_GREEN);
    debug.Line(-20.0f, 0.0f, 30.0f, -20.0f, 20.0f, 30.0f, COLOR_GREEN);
	debug.Line(20.0f, 0.0f, 30.0f, 20.0f, 20.0f, 30.0f, COLOR_GREEN);

    debug.Line(20.0f, 0.0f, 30.0f, 20.0f, 0.0f, 50.0f, COLOR_GREEN);
    debug.Line(20.0f, 20.0f, 30.0f, 20.0f, 20.0f, 50.0f, COLOR_GREEN);

    debug.Line(-20.0f, 0.0f, 30.0f, -20.0f, 0.0f, 50.0f, COLOR_GREEN);
    debug.Line(-20.0f, 20.0f, 30.0f, -20.0f, 20.0f, 50.0f, COLOR_GREEN);
	
    //
    // Point cloud generation
    //
    const FLOAT dist = 50.0f;
    for (UINT i = 0; i < 5000; i++) 
    {
        FLOAT randNumX = ((FLOAT)rand() / RAND_MAX);
        FLOAT randNumY = ((FLOAT)rand() / RAND_MAX);
        FLOAT randNumZ = ((FLOAT)rand() / RAND_MAX);
		
        debug.AddPointToPointCloud(XMLoadFloat3(&XMFLOAT3(25.0f + (randNumX * dist), 0.0f + (randNumY * dist), -25.0f + (randNumZ *dist))), COLOR_GREEN);
	}
    debug.EndPointCloud();


    //
    // Draw an text marker
    //
    XMVECTOR origin = XMLoadFloat3(&XMFLOAT3(-50.0f, 6.0f, 50.0f));
	debug.TextMarker(origin, "Hello there!");
	

    //
    // Change the camera position
    //
	debug.Camera(XMLoadFloat3(&XMFLOAT3(0.0f, 5.0f, -20.0f)), 5.0);
	debug.Camera(XMLoadFloat3(&XMFLOAT3(0.0f, 10.0f, -20.0f)), 5.0);
	debug.Camera(XMLoadFloat3(&XMFLOAT3(0.0f, 15.0f, -20.0f)), 5.0);
	debug.Camera(XMLoadFloat3(&XMFLOAT3(0.0f, 20.0f, -20.0f)), 5.0);

	return 0;
}

