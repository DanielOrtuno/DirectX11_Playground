#pragma once

#include "defines.h"
#include <random>
#include <time.h>

using namespace DirectX;

class Camera
{
private:

	XMFLOAT4 mUp;
	XMFLOAT4 mPos;
	XMFLOAT4 mTarget;

public:
	XMFLOAT4X4 mViewMatrix;
	XMFLOAT4X4 mProjMatrix;

	Camera();

	void InitializeCamera(float aspectRatio);


	~Camera();
};

