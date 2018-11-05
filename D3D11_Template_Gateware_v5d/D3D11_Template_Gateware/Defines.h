#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <d3d11.h>
#include <DirectXMath.h>
#include <atlcomcli.h>
#include "../D3D11_Template_Gateware/Models/ThorHammer.h" 
#include "../D3D11_Template_Gateware/Models/LivingRoom.h"
#include "../D3D11_Template_Gateware/Models/DC-15_Blaster.h"

using namespace DirectX;

inline XMVECTOR VectorRegister(XMFLOAT4 vector)
{
	return XMLoadFloat4(&vector);
}

inline XMFLOAT4  VectorStorage(XMVECTOR vector)
{
	XMFLOAT4 result;

	XMStoreFloat4(&result, vector);

	return result;
}

inline XMFLOAT4X4 MatrixStorage(XMMATRIX matrix)
{
	XMFLOAT4X4 result;

	XMStoreFloat4x4(&result, matrix);

	return result;
}

inline XMMATRIX MatrixRegister(XMFLOAT4X4 matrix)
{
	return XMLoadFloat4x4(&matrix);
}