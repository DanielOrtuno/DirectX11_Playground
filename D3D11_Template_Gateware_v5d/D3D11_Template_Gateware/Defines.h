#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <atlcomcli.h>

using namespace DirectX;

inline XMVECTOR StorageToRegister(XMFLOAT3 vector)
{
	return XMLoadFloat3(&vector);

}

inline XMFLOAT3  RegisterToStorage(XMVECTOR vector)
{

	XMFLOAT3 result;

	XMStoreFloat3(&result, vector);

	return result;
}