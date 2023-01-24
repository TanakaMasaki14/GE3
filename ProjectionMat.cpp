#include "ProjectionMat.h"

ProjectionMat::ProjectionMat() :
	nearPos(0.1f), farPos(10000.0f), aspect({ WindowsApp::GetInstance().window_width , WindowsApp::GetInstance().window_height }),
	fovY(XMConvertToRadians(45.0f))
{
	SetMat();
}

ProjectionMat::ProjectionMat(float nearPos, float farPos, float aspectX, float aspectY, float fovY) :
	nearPos(nearPos), farPos(farPos), aspect({ aspectX,aspectY }), fovY(fovY)
{
	SetMat();
}

void ProjectionMat::SetMat()
{
	matProjection = XMMatrixPerspectiveFovLH(
		fovY,//fovY
		(float)aspect.x / aspect.y,//アスペクト比
		nearPos, farPos//前端、奥端
	);
}
