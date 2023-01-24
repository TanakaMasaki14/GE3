#include "Vec4.h"

float Vec4::GetLength() const
{
    return sqrtf(x * x + y * y + z * z);
}

void Vec4::Normalized()
{
    *this /= GetLength();
}

const Vec4 Vec4::GetNormalized()
{
    Vec4 v;
    v = *this / GetLength();

    return v;
}

Vec4 Vec4::Cross(const Vec4& other)const
{
    Vec4 v;

    v.x = y * other.z - z * other.y;
    v.y = z * other.x - x * other.z;
    v.z = x * other.y - y * other.x;

    return v;
}

float Vec4::Dot(const Vec4& other)const
{
    return x * other.x + y * other.y + z * other.z;
}
//--------------------------------------------------------------


Vec4 Vec4::operator+(const Vec4& other)
{
    Vec4 v = *this;
    v.x += other.x;
    v.y += other.y;
    v.z += other.z;

    return Vec4(v);
}

Vec4& Vec4::operator+=(const Vec4& other)
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;

    return *this;
}

Vec4 Vec4::operator-(const Vec4& other)
{
    Vec4 v = *this;
    v.x -= other.x;
    v.y -= other.y;
    v.z -= other.z;

    return Vec4(v);
}

Vec4& Vec4::operator-=(const Vec4& other)
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;

    return *this;
}

Vec4 Vec4::operator*(const float& other)
{
    Vec4 v = *this;
    v.x *= other;
    v.y *= other;
    v.z *= other;

    return Vec4(v);
}

Vec4& Vec4::operator*=(const float& other)
{
    this->x *= other;
    this->y *= other;
    this->z *= other;

    return *this;
}

Vec4 Vec4::operator/(const float& other)
{
    Vec4 v = *this;
    v.x /= other;
    v.y /= other;
    v.z /= other;

    return Vec4(v);
}

Vec4& Vec4::operator/=(const float& other)
{
    this->x /= other;
    this->y /= other;
    this->z /= other;

    return *this;
}


//-------------------------------------

const Vec4 operator+(const Vec4& v1, const Vec4& v2)
{
    Vec4 temp(v1);
    return temp + v2;
}

const Vec4 operator-(const Vec4& v1, const Vec4& v2)
{
    Vec4 temp(v1);
    return temp - v2;
}

const Vec4 operator*(const Vec4& v, float s)
{
    Vec4 temp(v);
    return temp * s;
}

const Vec4 operator*(float s, const Vec4& v)
{
    Vec4 temp(v);
    return temp * s;
}

const Vec4 operator/(const Vec4& v, float s)
{
    Vec4 temp(v);
    return temp / s;
}
