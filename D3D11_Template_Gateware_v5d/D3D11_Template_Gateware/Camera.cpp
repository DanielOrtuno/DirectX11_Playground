#include "Camera.h"



Camera::Camera()
{
}

void Camera::InitializeCamera(float aspectRatio)
{

	mViewMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(0,1,-2));
	mProjMatrix = MatrixStorage(XMMatrixPerspectiveFovLH(65, aspectRatio, .1f, 10000));

	//mPos = XMFLOAT4(0, 0, -1,0);
	//mTarget = XMFLOAT4(0, 0, 1,0);

	//mUp = VectorRegisterToStorage(VectorStorageToRegister(mPos) + VectorStorageToRegister(mTarget));
}



Camera::~Camera()
{
}
