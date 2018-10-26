#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <atlcomcli.h>

using namespace DirectX;

inline XMVECTOR VectorStorageToRegister(XMFLOAT4 vector)
{
	return XMLoadFloat4(&vector);
}

inline XMFLOAT4  VectorRegisterToStorage(XMVECTOR vector)
{
	XMFLOAT4 result;

	XMStoreFloat4(&result, vector);

	return result;
}

inline XMFLOAT4X4 MatrixRegisterToStorage(XMMATRIX matrix)
{
	XMFLOAT4X4 result;

	XMStoreFloat4x4(&result, matrix);

	return result;
}

inline XMMATRIX MatrixStorageToRegister(XMFLOAT4X4 matrix)
{
	return XMLoadFloat4x4(&matrix);
}