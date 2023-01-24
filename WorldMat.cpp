#include "WorldMat.h"
#include "Util.h"

WorldMat::WorldMat()
{
	matWorld =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matScale =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matRot =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matTrans =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };
}

void WorldMat::SetScale()
{
	matScale = {
		scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1
	};

	matWorld *= matScale;
}

void WorldMat::SetRot()
{
	matRot = normalM;
	matRot *= {
		cosf(rot.z), sinf(rot.z), 0, 0,
			-sinf(rot.z), cosf(rot.z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	};
	matRot *= {
		1, 0, 0, 0,
			0, cosf(rot.x), sinf(rot.x), 0,
			0, -sinf(rot.x), cosf(rot.x), 0,
			0, 0, 0, 1
	};
	matRot *= {
		cosf(rot.y), 0, -sinf(rot.y), 0,
			0, 1, 0, 0,
			sinf(rot.y), 0, cosf(rot.y), 0,
			0, 0, 0, 1
	};
	matWorld *= matRot;
}

void WorldMat::SetTrans()
{
	matWorld *= {
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			trans.x, trans.y, trans.z, 1
	};
}

void WorldMat::SetWorld()
{
	matWorld = normalM;

	SetScale();
	SetRot();
	SetTrans();
}
