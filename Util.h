#pragma once

#include "Vec3.h"
#include "Vec2.h"
#include "Vec4.h"
#include "M4.h"
#include"TextureManager.h"

//自作
void Vec4xM4(Vec4& v, const M4& m4);
void Vec3xM4(Vec3& v, const M4& m4, const bool w);
void Vec3xM4andDivisionW(Vec3& v, const M4& m4, const bool w);

//ラジアン系
const float pi = 3.141592f;
float AngletoRadi(const float& angle);
float RaditoAngle(const float& radian);
float GetRadianVec3(const Vec3& v1, const Vec3& v2);

//線形補完
Vec3 LerpVec3(const Vec3& v1, const Vec3& v2, float t);
Vec3 SlerpVec3(const Vec3& v1, const Vec3& v2, float t);

//衝突
bool CollisionCircleCircle(const Vec3& pos1, const float& r1,
	const Vec3& pos2, const float& r2);

bool CollisionRayCircle(const Vec3& sv, const Vec3& ev, const float r, const Vec3& pos, const float r2);

//変換
Vec2 Vec3toVec2(const Vec3& v, const XMMATRIX& view, const XMMATRIX& projection);
/// <summary>
/// distanceは、projectionのnearPosからの位置!!
/// </summary>
/// <param name="v"></param>
/// <param name="view"></param>
/// <param name="projection"></param>
/// <param name="distance"></param>
/// <returns></returns>
Vec3 Vec2toVec3(const Vec2& v, const XMMATRIX& view, const XMMATRIX& projection, const float distance);

/// <summary>
/// スクリーン座標をprojectionのnear,farの位置に変換してreturn...Posに代入
/// </summary>
/// <param name="v"></param>
/// <param name="returnNearPos"></param>
/// <param name="returnFarPos"></param>
/// <param name="view"></param>
/// <param name="projection"></param>
void Vec2toNearFarPos(const Vec2& pos, Vec3& returnNearPos, Vec3& returnFarPos, const XMMATRIX& view, const XMMATRIX& projection);

