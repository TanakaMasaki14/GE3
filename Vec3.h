#pragma once
#include<DirectXMath.h>

class Vec3
{
private:


public:
	float x;
	float y;
	float z;

	Vec3()
		:x(0), y(0), z(0)
	{}
	Vec3(const float x, const float y, const float z)
		:x(x), y(y), z(z)
	{}

	//便利関数
	float GetLength() const;
	void Normalized();
	Vec3 GetNormalized() const;
	Vec3 Cross(const Vec3& other) const;
	float Dot(const Vec3& other) const;

	//オバロ演算子
	Vec3 operator+();
	Vec3& operator+=(const Vec3& other);
	Vec3 operator-();
	Vec3& operator-=(const Vec3& other);
	Vec3 operator*(const float& other);
	Vec3& operator*=(const float& other);
	Vec3 operator/(const float& other);
	Vec3& operator/=(const float& other);
};

//二項演算子オーバーロード
const Vec3 operator +(const Vec3& v1, const Vec3& v2);
const Vec3 operator -(const Vec3& v1, const Vec3& v2);
const Vec3 operator *(const Vec3& v, float s);
const Vec3 operator *(float s, const Vec3& v);
const Vec3 operator /(const Vec3& v, float s);