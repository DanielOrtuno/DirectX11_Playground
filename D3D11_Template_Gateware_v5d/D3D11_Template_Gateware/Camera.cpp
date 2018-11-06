#include "Camera.h"



Camera::Camera()
{
	FOV = 65;
	nearPlane = .1f;
	farPlane = 300;
}

void Camera::InitializeCamera(float aspectRatio)
{
	
	mViewMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(0,1,-2));
	mProjMatrix = MatrixStorage(XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nearPlane, farPlane));

	//mPos = XMFLOAT4(0, 0, -1,0);
	//mTarget = XMFLOAT4(0, 0, 1,0);

	//mUp = VectorRegisterToStorage(VectorStorageToRegister(mPos) + VectorStorageToRegister(mTarget));
}



Camera::~Camera()
{
}
