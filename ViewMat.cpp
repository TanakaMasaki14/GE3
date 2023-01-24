#include "ViewMat.h"

ViewMat::ViewMat(float eyeX, float eyeY, float eyeZ,
	float targetX, float targetY, float targetZ,
	float upX, float upY, float upZ) :
	eye({ eyeX,eyeY,eyeZ }), target({ targetX,targetY,targetZ }), up({ upX,upY,upZ })
{
	SetMat();
}

ViewMat::ViewMat() :
	eye({ 0,0,-100.0f }), target({ 0,0,0 }), up({ 0,1.f,0 })
{
	SetMat();
}

void ViewMat::Update()
{
	SetMat();
}

void ViewMat::SetMat()
{
	XMFLOAT3 eye2 = { eye.x,eye.y ,eye.z };
	XMFLOAT3 target2 = { target.x,target.y ,target.z };
	XMFLOAT3 up2 = { up.x,up.y ,up.z };
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye2), XMLoadFloat3(&target2), XMLoadFloat3(&up2));
}
