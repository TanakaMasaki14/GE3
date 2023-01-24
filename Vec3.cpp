#include "Vec3.h"

float Vec3::GetLength() const
{
    return sqrtf(x * x + y * y + z * z);
}

void Vec3::Normalized()
{
    *this /= GetLength();
}

Vec3 Vec3::GetNormalized() const
{
    Vec3 v;
    v = *this / GetLength();

    return v;
}

Vec3 Vec3::Cross(const Vec3& other)const
{
    Vec3 v;

    v.x = y * other.z - z * other.y;
    v.y = z * other.x - x * other.z;
    v.z = x * other.y - y * other.x;

    return v;
}

float Vec3::Dot(const Vec3& other)const
{
    return x * other.x + y * other.y + z * other.z;
}
//--------------------------------------------------------------


Vec3 Vec3::operator+()
{
    return Vec3(*this);
}

Vec3& Vec3::operator+=(const Vec3& other)
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;

    return *this;
}

Vec3 Vec3::operator-()
{
    return Vec3({ -x,-y,-z });
}

Vec3& Vec3::operator-=(const Vec3& other)
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;

    return *this;
}

Vec3 Vec3::operator*(const float& other)
{
    Vec3 v = *this;
    v.x *= other;
    v.y *= other;
    v.z *= other;

    return Vec3(v);
}

Vec3& Vec3::operator*=(const float& other)
{
    this->x *= other;
    this->y *= other;
    this->z *= other;

    return *this;
}

Vec3 Vec3::operator/(const float& other)
{
    Vec3 v = *this;
    v.x /= other;
    v.y /= other;
    v.z /= other;

    return Vec3(v);
}

Vec3& Vec3::operator/=(const float& other)
{
    this->x /= other;
    this->y /= other;
    this->z /= other;

    return *this;
}


//-------------------------------------

const Vec3 operator+(const Vec3& v1, const Vec3& v2)
{
    Vec3 temp(v1);
    return temp += v2;
}

const Vec3 operator-(const Vec3& v1, const Vec3& v2)
{
    Vec3 temp(v1);
    return temp -= v2;
}

const Vec3 operator*(const Vec3& v, float s)
{
    Vec3 temp(v);
    return temp *= s;
}

const Vec3 operator*(float s, const Vec3& v)
{
    Vec3 temp(v);
    return temp *= s;
}

const Vec3 operator/(const Vec3& v, float s)
{
    Vec3 temp(v);
    return temp /= s;
}
