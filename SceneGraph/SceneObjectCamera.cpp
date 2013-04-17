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
#include "SceneObjectCamera.h"

HRESULT SceneObjectCamera::Initialize(__in_z const wchar_t *nodeName)
{
    m_Scale = 0.0f;
    m_RollPitchYaw = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));

	// Projection initial settings
	m_Fov = 1.22f;
	m_AspectRation = 1.333333f;
    m_NearPlane = 1.0f;
    m_FarPlane = 1000.0f;

	// View initial settings
	m_Position = XMLoadFloat3(&XMFLOAT3(0.0f, 7.0f, -1.0f));
    m_Direction = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 1.0f));
    m_UpVector = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_At = XMVectorAdd(m_Position, m_Direction);

	// Set projection matrix
	m_Projection = XMMatrixPerspectiveFovLH(m_Fov, m_AspectRation, m_NearPlane, m_FarPlane); 
	
	// Set view matrix
	HRESULT hr = SetLocalTransform(XMMatrixLookAtLH(m_Position, m_At, m_UpVector));
	
	if (SUCCEEDED(hr))
	{
		m_WorldTransform = m_LocalTransform;
		hr = SetName(nodeName);
	}
   
    return hr;
}

HRESULT SceneObjectCamera::GetProjectionMatrix(XMMATRIX *projection)
{
   if (!projection)
        return E_POINTER;

   *projection = m_Projection;
   return S_OK;
}

HRESULT SceneObjectCamera::SetProjectionMatrix(CXMMATRIX projection)
{
    m_Projection = projection;
    return Invalidate(SOCT_PROJECTION);
}

HRESULT SceneObjectCamera::GetViewMatrix(XMMATRIX *viewMatrix)
{
	*viewMatrix = m_WorldTransform;
	return S_OK;
}

HRESULT SceneObjectCamera::Look(FLOAT yawDelta, FLOAT pitchDelta, FLOAT rollDelta, FLOAT scaleDelta)
{
    FLOAT yaw = XMVectorGetY(m_RollPitchYaw) + yawDelta;
    if (yaw > XM_2PI)
    {
        yaw = yaw - XM_2PI;
    }

    FLOAT roll = XMVectorGetZ(m_RollPitchYaw) + rollDelta;
    if (roll > XM_2PI)
    {
        roll = roll - XM_2PI;
    }

    FLOAT pitch = XMVectorGetX(m_RollPitchYaw) + pitchDelta;
    if (pitch > XM_2PI)
    {
        pitch = pitch - XM_2PI;
    }

    m_RollPitchYaw = XMVectorSet(pitch, yaw, roll, 1.0f);
    
    m_Scale += scaleDelta;

	// Camera Rotation
    XMVECTOR quaternionRotation = XMQuaternionRotationRollPitchYawFromVector(m_RollPitchYaw);
    XMVECTOR direction = XMVector3Rotate(m_Direction, quaternionRotation);
    direction = XMVector3Normalize(direction);
    XMVECTOR upVector = XMVector3Rotate(m_UpVector, quaternionRotation);

	XMVECTOR position = XMVectorSubtract(m_Position, XMVectorScale(direction, m_Scale));
	XMVECTOR at = XMVectorAdd(position, direction);

    // Set ViewMatrix
	return SetLocalTransform(XMMatrixLookAtLH(position, at, upVector));
}

HRESULT SceneObjectCamera::Move(FLOAT forwardDelta, FLOAT strideDelta, FLOAT upDelta)
{
    // Camera Rotation
	XMVECTOR quaternionRotation = XMQuaternionRotationRollPitchYawFromVector(m_RollPitchYaw);
	XMVECTOR quaternionRotationYaw = XMQuaternionRotationRollPitchYaw(0.0f, XMVectorGetY(m_RollPitchYaw), 0.0f);
	
	XMVECTOR directionYaw = XMVector3Rotate(m_Direction, quaternionRotationYaw);
    directionYaw = XMVector3Normalize(directionYaw);
	XMVECTOR direction = XMVector3Rotate(m_Direction, quaternionRotation);
    direction = XMVector3Normalize(direction);
	XMVECTOR upVector = XMVector3Rotate(m_UpVector, quaternionRotation);

	// Forward
	m_Position = XMVectorAdd(m_Position, XMVectorScale(directionYaw, forwardDelta));

    // Stride
    m_Position = XMVectorAdd(m_Position, XMVectorScale(XMVector3Normalize(XMVector3Cross(directionYaw, m_UpVector)), strideDelta));
    
	// Up
    m_Position = XMVectorAdd(m_Position, XMVectorScale(m_UpVector, upDelta));

	XMVECTOR position = XMVectorSubtract(m_Position, XMVectorScale(direction, m_Scale));
	XMVECTOR at = XMVectorAdd(position, direction);

    // Set ViewMatrix
	return SetLocalTransform(XMMatrixLookAtLH(position, at, upVector));
}

void SceneObjectCamera::ResetRotationAndScale()
{
	m_RollPitchYaw = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_Scale = 0.0f;
}

HRESULT SceneObjectCamera::SetPositionAtUpFov(CXMVECTOR pos, CXMVECTOR at, CXMVECTOR up, FLOAT fov)
{
	ResetRotationAndScale();

	// Projection
	m_Fov = fov;	
	m_Projection = XMMatrixPerspectiveFovLH(m_Fov, m_AspectRation, m_NearPlane, m_FarPlane); 

	// View
	m_Position = pos;
	m_At = at;
	m_UpVector = up;
	m_Direction = XMVectorSubtract(m_At, m_Position);
	
	// Set ViewMatrix
	return SetLocalTransform(XMMatrixLookAtLH(m_Position, m_At, m_UpVector));
}

HRESULT SceneObjectCamera::SetPositionAndScale(CXMVECTOR pos, FLOAT scale)
{
	ResetRotationAndScale();

	m_Position = pos;
	m_Scale = scale;

	XMVECTOR position = XMVectorSubtract(m_Position, XMVectorScale(m_Direction, m_Scale));
	
	// Set ViewMatrix
	return SetLocalTransform(XMMatrixLookAtLH(position, m_At, m_UpVector));
}



HRESULT CreateCamera(__in_z const wchar_t *nodeName,__deref_out ISceneObjectCamera **ppResult)
{
    return SceneObjectCamera::CreateInstance(ppResult, nodeName);
}