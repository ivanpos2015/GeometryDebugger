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

#include "SceneObject.h"

class SceneObjectCamera : public ImplementSmartObject
    <
        SceneObjectCamera, 
        ClassFlags<CF_ALIGNED_MEMORY>,
        InheritImplementation<SceneObject>, 
        ISceneObjectCamera
    >
{
    XMMATRIX m_Projection;
    float m_NearPlane, m_FarPlane;

	XMVECTOR m_Position;
    XMVECTOR m_Direction;
    XMVECTOR m_UpVector;
	XMVECTOR m_At;
    
	XMVECTOR m_RollPitchYaw;
    FLOAT m_Scale;

	FLOAT m_Fov;
	FLOAT m_AspectRation;

	void ResetRotationAndScale();

public:
    HRESULT Initialize(__in_z const wchar_t *nodeName);

    HRESULT GetProjectionMatrix(__out XMMATRIX *projection);
    HRESULT SetProjectionMatrix(CXMMATRIX projection);
	HRESULT GetViewMatrix(__out XMMATRIX *viewMatrix);

	HRESULT Look(FLOAT yawDelta, FLOAT pitchDelta, FLOAT rollDelta, FLOAT scaleDelta);
	HRESULT Move(FLOAT forwardDelta, FLOAT strideDelta, FLOAT upDelta);
	HRESULT SetPositionAtUpFov(CXMVECTOR pos, CXMVECTOR at, CXMVECTOR up, FLOAT fov);
	HRESULT SetPositionAndScale(CXMVECTOR pos, FLOAT scale);

};