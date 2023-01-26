#include "Matrix4.h"
#include "Vector3.h"
#include <cmath>

Matrix4 Matrix4::identity()
{
	static const Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	*this = result;
	return *this;
}

Matrix4 Matrix4::scale(const Vector3& s)
{
	Matrix4 result{
		s.x,0.0f,0.0f,0.0f,
		0.0f,s.y,0.0f,0.0f,
		0.0f,0.0f,s.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4 Matrix4::rotateX(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,cos,sin,0.0f,
		0.0f,-sin,cos,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4 Matrix4::rotateY(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
	   cos,0.0f,-sin,0.0f,
	   0.0f,1.0f,0.0f,0.0f,
	   sin,0.0f,cos,0.0f,
	   0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4 Matrix4::rotateZ(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
			cos,sin,0.0f,0.0f,
			-sin,cos,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4 Matrix4::translate(const Vector3& t)
{
	Matrix4 result{
	1.0f,0.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,0.0f,
	0.0f,0.0f,1.0f,0.0f,
	t.x,t.y,t.z,1.0f
	};

	return result;
}

Vector3 Matrix4::transform(const Vector3& v, const Matrix4& m)
{
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];


	Vector3 result{
		(v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
		(v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
		(v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w
	};

	return result;
}
//
//Matrix4& operator*=(Matrix4& m1, const  Matrix4& m2)
//{
//	Matrix4 result{ 0 };
//
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			for (int k = 0; k < 4; k++) {
//				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
//			}
//		}
//	}
//
//	m1 = result;
//	return m1;
//}

Matrix4 Matrix4::Inverse()
{
	Matrix4 mat, result, identity;
	mat = *this;

	identity.identity();
	float sweep[4][8];

	//�|���o���@�Ŏg���s��̍����ɂ͌��̍s��A�E�ɂ͒P�ʍs�����
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			sweep[i][j] = mat.m[i][j];
			sweep[i][(j + 4)] = identity.m[i][j];
		}
	}

	//�t�s��v�Z�J�n
	for (int k = 0; k < 4; k++) {

		//�[�����Z�΍�
		double max = fabs(sweep[k][k]);
		int maxI = k;
		for (int i = k + 1; i < 4; i++) {
			if (fabs(sweep[i][k]) > max) {
				max = fabs(sweep[i][k]);
				maxI = i;
			}
		}

		//��Βl�̍ő�l��0�Ȃ�t�s�񖳂��B�P�ʍs���Ԃ�
		if (fabs(sweep[maxI][k]) <= 0.000001f) {
			return identity;
		}

		//�Y���s�Ƃ̓���ւ�
		if (k != maxI) {
			for (int i = 0; i < 8; i++) {
				float temp = sweep[maxI][i];
				sweep[maxI][i] = sweep[k][i];
				sweep[k][i] = temp;
			}
		}


		//[k][k]�Ɋ|������1�ɂȂ�l�����߁Ak�s�Ɋ|���Z
		float x = 1 / sweep[k][k];
		for (int i = 0; i < 8; i++) {
			sweep[k][i] *= x;
		}

		//���̎��_��sweep[k][k]��1�ɂȂ��Ă���
		//���ɁA[i][k]��0�ɂȂ�悤��k�s�� -i�{
		for (int i = 0; i < 4; i++) {
			//i = k�Ȃ��΂�
			if (i == k) {
				continue;
			}

			float num = -sweep[i][k];

			for (int j = 0; j < 8; j++) {
				sweep[i][j] += sweep[k][j] * num;
			}

		}
	}

	//�|���o���I���Bsweep�̉E�l������U���g�ɓ����
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = sweep[i][j + 4];
		}
	}

	*this = result;

	return *this;
}

Matrix4& Matrix4::operator*=(const Matrix4& mat)
{
	Matrix4 result{};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m[i][k] + mat.m[k][j];
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] = result.m[i][j];
		}
	}

	*this = result;
	return *this;

}



const  Matrix4 operator*(const Matrix4& m1, const Matrix4& m2)
{
	Matrix4 result = m1;

	return result *= m2;
}

const Vector3 operator*(const Vector3& v, const Matrix4& m)
{
	Matrix4 result;

	return result.transform(v, m);
}
