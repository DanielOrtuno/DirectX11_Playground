#include "Camera.h"



Camera::Camera()
{
}

void Camera::InitializeCamera(float aspectRatio)
{

	mViewMatrix = MatrixRegisterToStorage(XMMatrixIdentity());
	mProjMatrix = MatrixRegisterToStorage(XMMatrixPerspectiveFovLH(90, aspectRatio, .1f, 10));

	//mPos = XMFLOAT4(0, 0, -1,0);
	//mTarget = XMFLOAT4(0, 0, 1,0);

	//mUp = VectorRegisterToStorage(VectorStorageToRegister(mPos) + VectorStorageToRegister(mTarget));
}



Camera::~Camera()
{
}
