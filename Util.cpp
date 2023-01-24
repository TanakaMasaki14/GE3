#include "Util.h"
#include "WindowsApp.h"

void Vec4xM4(Vec4& v, const M4& m4)
{
	float v4[2][4] = {
		{ v.x,v.y,v.z,v.w },
		{0,0,0,0}
	};

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			v4[1][i] += v4[0][j] * (float)m4.m[j][i];
		}
	}

	v = { v4[1][0],v4[1][1] ,v4[1][2] ,v4[1][3] };
}

void Vec3xM4(Vec3& v, const M4& m4, const bool w)
{
	float v4[2][4] = {
		{ v.x,v.y,v.z,(float)w },
		{0,0,0,0}
	};

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			v4[1][i] += v4[0][j] * (float)m4.m[j][i];
		}
	}

	v = { v4[1][0],v4[1][1] ,v4[1][2] };
}

void Vec3xM4andDivisionW(Vec3& v, const M4& m4, const bool w)
{
	float v4[2][4] = {
		{ v.x,v.y,v.z,(float)w },
		{0,0,0,0}
	};

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			v4[1][i] += v4[0][j] * (float)m4.m[j][i];
		}
	}

	v = { v4[1][0],v4[1][1] ,v4[1][2] };

	if (w == true && v4[1][3] != 0)
	{
		v /= v4[1][3];
	}
	else if (v.z != 0)
	{
		float W = v.z;

		v /= W;
	}
}


//---------------------------------------------

float AngletoRadi(const float& angle)
{
	return angle * pi / 180;
}

float RaditoAngle(const float& radian)
{
	return radian / pi * 180;
}

float GetRadianVec3(const Vec3& v1, const Vec3& v2)
{
	return acos(v1.Dot(v2) / (v1.GetLength() * v2.GetLength()));
}

//----------------------------------------------------------------------
Vec3 LerpVec3(const Vec3& v1, const Vec3& v2, float t)
{
	return v1 + t * (v2 - v1);
}

Vec3 SlerpVec3(const Vec3& v1, const Vec3& v2, float t)
{
	float radian = GetRadianVec3(v1, v2);

	Vec3 v = ((1 - t) * v1.GetLength() + t * v2.GetLength()) *
		(sinf((1 - t) * radian) / sinf(radian) * v1.GetNormalized() + sinf(t * radian) / sinf(radian) * v2.GetNormalized());

	return v;
}

bool CollisionCircleCircle(const Vec3& pos1, const float& r1, const Vec3& pos2, const float& r2)
{
	if (pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2) + pow(pos2.z - pos1.z, 2)
		<= pow(r1 + r2, 2))
	{
		return true;
	}

	return false;
}

bool CollisionRayCircle(const Vec3& sv, const Vec3& ev, const float r, const Vec3& pos, const float r2)
{
	//レイとの当たり判定
	Vec3 rayLength = ev - sv;
	/*Vec3 ev = { worldTransform2_.translation_.x + rayLength.x,
		worldTransform2_.translation_.y + rayLength.y,
		worldTransform2_.translation_.z + rayLength.z / 2 };
	Vec3 sv = { worldTransform2_.translation_.x,
		worldTransform2_.translation_.y - rayLength.y,
		worldTransform2_.translation_.z - rayLength.z / 2 };*/
	rayLength.Normalized();
	Vec3 objLength = pos - sv;
	Vec3 dotPos = sv + rayLength * rayLength.Dot(objLength);
	Vec3 dotVec = pos - dotPos;
	float dotLength = dotVec.GetLength();
	if (dotLength <= r + r2
		/*&& (worldTransform_.translation_.z + worldTransform_.scale_.z > sv.z
			&& worldTransform_.translation_.z - worldTransform_.scale_.z < ev.z)*/)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Vec2 Vec3toVec2(const Vec3& v, const XMMATRIX& view, const XMMATRIX& projection)
{
	//view,projection,viewport行列を掛ける
	XMMATRIX viewPort = {
		WindowsApp::GetInstance().window_width / 2.0f,0,0,0,
		0,-WindowsApp::GetInstance().window_height / 2.0f,0,0,
		0,0,1,0,

		WindowsApp::GetInstance().window_width / 2.0f + WindowsApp::GetInstance().viewport.TopLeftX
		,WindowsApp::GetInstance().window_height / 2.0f + WindowsApp::GetInstance().viewport.TopLeftY,0,1
	};

	XMMATRIX mVPVp = view * projection * viewPort;


	Vec3 vec3 = v;

	/*M4 m4 = {
		(float)mVPVp.r[0].m128_f32[0],(float)mVPVp.r[0].m128_f32[1],(float)mVPVp.r[0].m128_f32[2],(float)mVPVp.r[0].m128_f32[3],
		(float)mVPVp.r[1].m128_f32[0],(float)mVPVp.r[1].m128_f32[1],(float)mVPVp.r[1].m128_f32[2],(float)mVPVp.r[1].m128_f32[3],
		(float)mVPVp.r[2].m128_f32[0],(float)mVPVp.r[2].m128_f32[1],(float)mVPVp.r[2].m128_f32[2],(float)mVPVp.r[2].m128_f32[3],
		(float)mVPVp.r[3].m128_f32[0],(float)mVPVp.r[3].m128_f32[1],(float)mVPVp.r[3].m128_f32[2],(float)mVPVp.r[3].m128_f32[3]
	};*/
	M4 m4;
	m4.PutinXMMATRIX(mVPVp);

	Vec3xM4andDivisionW(vec3, m4, 1);

	return Vec2(vec3.x, vec3.y);
}

Vec3 Vec2toVec3(const Vec2& v, const XMMATRIX& view, const XMMATRIX& projection, const float distance)
{
	XMMATRIX viewPort = {
		WindowsApp::GetInstance().window_width / 2.0f,0,0,0,
		0,-WindowsApp::GetInstance().window_height / 2.0f,0,0,
		0,0,1,0,

		WindowsApp::GetInstance().window_width / 2.0f + WindowsApp::GetInstance().viewport.TopLeftX
		,WindowsApp::GetInstance().window_height / 2.0f + WindowsApp::GetInstance().viewport.TopLeftY,0,1
	};

	//合成行列
	XMMATRIX mVPVp = view * projection * viewPort;
	M4 m4;
	//m4.PutinXMMATRIX(mVPVp);

	//逆行列計算
	XMMATRIX mInverseVPVp = XMMatrixInverse(nullptr, mVPVp);
	//m4.SetInverseMatrix();

	m4.PutinXMMATRIX(mInverseVPVp);

	//スクリーン座標
	Vec3 posNear = { v.x,v.y,0 };
	Vec3 posFar = { v.x,v.y,1 };

	//スクリーン座標->ワールド座標
	Vec3xM4andDivisionW(posNear, m4, 1);
	Vec3xM4andDivisionW(posFar, m4, 1);

	//nearからfarへの線分
	Vec3 nearFarDirection = posFar - posNear;
	nearFarDirection.Normalized();

	//カメラから照準オブジェクトの距離
	const float distanceTestObject = distance;//仮

	Vec3 pos = posNear + nearFarDirection * distanceTestObject;

	return Vec3(pos);
}

void Vec2toNearFarPos(const Vec2& pos, Vec3& returnNearPos, Vec3& returnFarPos, const XMMATRIX& view, const XMMATRIX& projection)
{
	XMMATRIX viewPort = {
		WindowsApp::GetInstance().window_width / 2.0f,0,0,0,
		0,-WindowsApp::GetInstance().window_height / 2.0f,0,0,
		0,0,1,0,

		WindowsApp::GetInstance().window_width / 2.0f + WindowsApp::GetInstance().viewport.TopLeftX
		,WindowsApp::GetInstance().window_height / 2.0f + WindowsApp::GetInstance().viewport.TopLeftY,0,1
	};

	//合成行列
	XMMATRIX mVPVp = view * projection * viewPort;
	M4 m4;
	//m4.PutinXMMATRIX(mVPVp);

	//逆行列計算
	XMMATRIX mInverseVPVp = XMMatrixInverse(nullptr, mVPVp);
	//m4.SetInverseMatrix();

	m4.PutinXMMATRIX(mInverseVPVp);

	//スクリーン座標
	Vec3 posNear = { pos.x,pos.y,0 };
	Vec3 posFar = { pos.x,pos.y,1 };

	//スクリーン座標->ワールド座標
	Vec3xM4andDivisionW(posNear, m4, 1);
	Vec3xM4andDivisionW(posFar, m4, 1);

	returnNearPos = posNear;
	returnFarPos = posFar;
}
