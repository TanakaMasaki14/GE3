#include "Vec2.h"
#include <DirectXMath.h>


float Vec2::GetLength() const
{
	return sqrtf(x * x + y * y);
}

void Vec2::Normalized()
{
	*this /= GetLength();
}

Vec2 Vec2::GetNormalize()
{
	float len = GetLength();
	if (len > 0) return *this / len;

	return *this;
}

float Vec2::Cross(const Vec2& other)const
{
	return x * other.y - y * other.x;
}

float Vec2::Dot(const Vec2& other)const
{
	return x * other.x + y * other.y;
}



//-----------------------------------------------------------------
//演算子オーバーロード
Vec2 Vec2::operator-()
{
	return { -x,-y };
}

Vec2& Vec2::operator-=(const Vec2& other)
{
	this->x -= other.x;
	this->y -= other.y;

	return *this;
}


Vec2 Vec2::operator+()
{
	return *this;
}

Vec2& Vec2::operator+=(const Vec2& other)
{
	this->x += other.x;
	this->y += other.y;

	return *this;
}



Vec2 Vec2::operator*(const float& other)
{
	Vec2 v;
	v.x = x * other;
	v.y = y * other;

	return v;
}

Vec2& Vec2::operator*=(const float& other)
{
	*this = *this * other;//thisはアドレスなので*つける

	return *this;
}



Vec2 Vec2::operator/(const float& other)
{
	Vec2 v;
	v.x = x / other;
	v.y = y / other;

	return v;
}

Vec2& Vec2::operator/=(const float& other)
{
	*this = *this / other;//thisはアドレスなので*つける

	return *this;
}




const Vec2 operator+(const Vec2& v1, const Vec2& v2)
{
	Vec2 temp(v1);
	return temp += v2;
}

const Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
	Vec2 temp(v1);
	return temp -= v2;
}

const Vec2 operator*(const Vec2& v, float s)
{
	Vec2 temp(v);
	return temp *= s;
}

const Vec2 operator*(float s, const Vec2& v)
{
	Vec2 temp(v);
	return temp *= s;
}

const Vec2 operator/(const Vec2& v, float s)
{
	Vec2 temp(v);
	return temp /= s;
}