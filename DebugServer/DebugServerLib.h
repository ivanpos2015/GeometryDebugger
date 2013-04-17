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

const WORD DEBUG_SERVER_DEFAULT_PORT = 8318;

DECLAREINTERFACE(IDebugServer, IUnknown, "{A80F80ED-82A9-493A-B8CD-168866035A29}")
{
    HRESULT Destroy();
};

HRESULT CreateDebugServer(WORD port, __in ISceneGraph *pSceneGraph, __in ISceneObjectCamera *pCamera, __deref_out IDebugServer **ppDebugServer);