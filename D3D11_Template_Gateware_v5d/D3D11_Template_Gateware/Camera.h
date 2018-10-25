#pragma once

#include "defines.h"

using namespace DirectX;

class Camera
{
private:
	XMFLOAT4X4 mViewMatrix;
	XMFLOAT4X4 mProjMatrix;
	XMFLOAT3 mUp;
	XMFLOAT3 mPos;
	XMFLOAT3 mTarget;

public:
	Camera();

	void InitializeCamera()
	{
		XMStoreFloat4x4(&mViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&mProjMatrix, XMMatrixIdentity());

		mPos = XMFLOAT3(0, 0, -1);
		mTarget = XMFLOAT3(0, 0, 1);

		mUp = RegisterToStorage(StorageToRegister(mPos) + StorageToRegister(mTarget));

	};

	~Camera();
};

