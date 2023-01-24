#pragma once
#include"Vec3.h"
using namespace DirectX;

class M4
{
private:


public:
	//行列
	double m[4][4];

	//関数
	M4();
	M4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);
	M4(const float(*other)[4]);
	/*void Normalize();*/

	//自作

	void TransposeM4();

	//行列セット
	void SetScaleMatrix(const Vec3& v3);
	void SetRotationMatrix(const float& angle, const char& axis);
	void SetTranslationMatrix(const Vec3& v3);

	void SetInverseMatrix();

	//
	/*M4& operator=(const float (*other)[4]);*/
	M4 operator*(const M4& other);
	M4& operator*=(const M4& other);

	void PutinXMMATRIX(const XMMATRIX& xM);
};

Vec3 operator*(const Vec3& v, const M4& m2);


const float normalM[4][4] = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
};

const float papa[4][4] = {
		{1,1,1,1},
		{2,2,2,2},
		{3,3,3,3},
		{4,4,4,4}
};

const float papa2[4][4] = {
		{5,7,2,9},
		{8,4,3,5},
		{1,3,1,9},
		{3,8,6,2}
};

