#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix4x4.h"

#include <cmath>
#include <cassert>

namespace {
	constexpr float kPI = 3.14159265358979;
}

Quaternion::Quaternion() :
	w(0),
	x(0),
	y(0),
	z(0)
{
}

Quaternion::Quaternion(float _w, float _x, float _y, float _z) :
	w(_w),
	x(_x),
	y(_y),
	z(_z)
{
}

Quaternion Quaternion::operator*(const float& s) const
{
    return Quaternion(w*s, x*s, y*s, z*s);
}

void Quaternion::operator*=(const float& s)
{
    *this = (*this) * s;
}

bool Quaternion::operator==(const Quaternion& q) const
{
    return (
        w == q.w && 
        x == q.x && 
        y == q.y && 
        z == q.z);
}

void Quaternion::Normalized()
{
    *this = Normalize();
}

Quaternion Quaternion::Normalize() const
{
    float mag = std::sqrt(w * w + x * x + y * y + z * z);
    if (mag == 0.0f) return *this;  //0���Z���
    return Quaternion(w / mag, x / mag, y / mag, z / mag);
}

Quaternion Quaternion::Inverse() const
{
    return Quaternion(w, -x, -y, -z);
}

Quaternion operator*(const Quaternion lQ, const Quaternion rQ)
{
	Quaternion ret;

	// �N�H�[�^�j�I�����m�̌v�Z
	ret.w = lQ.w * rQ.w - lQ.x * rQ.x - lQ.y * rQ.y - lQ.z * rQ.z;
	ret.x = lQ.w * rQ.x + lQ.x * rQ.w + lQ.y * rQ.z - lQ.z * rQ.y;
	ret.y = lQ.w * rQ.y + lQ.y * rQ.w + lQ.z * rQ.x - lQ.x * rQ.z;
	ret.z = lQ.w * rQ.z + lQ.z * rQ.w + lQ.x * rQ.y - lQ.y * rQ.x;

	return ret;
}

Vector3 operator*(const Quaternion qRot, const Vector3 right)
{
    return RotateVector3(qRot, right);
}

Vector3 RotateVector3(const Quaternion qRot, const Vector3 right)
{
    Quaternion qPos, qInv;

    Vector3 vPos;

    // �O�������W���N�H�[�^�j�I���ɕϊ�
    qPos.w = 0.0f;
    qPos.x = right.x;
    qPos.y = right.y;
    qPos.z = right.z;

    // �C���o�[�X
    qInv = qRot.Inverse();

    // ��]��̃N�H�[�^�j�I�����쐬
    qPos = qRot * qPos * qInv;

    // �O�������W�ɖ߂�
    vPos.x = qPos.x;
    vPos.y = qPos.y;
    vPos.z = qPos.z;

    return vPos;
}

Quaternion AngleAxis(const Vector3& axis, const float& angle)
{
	// ���K��
	Vector3 ax = axis.Normalize();
	Quaternion q;
	// ���x�N�g�����N�H�[�^�j�I���ɓ��Ă�
	q.w = cos(angle * 0.5f);
	q.x = ax.x * sin(angle * 0.5f);
	q.y = ax.y * sin(angle * 0.5f);
	q.z = ax.z * sin(angle * 0.5f);

    // ���K��
    q.Normalized();

	return q;
}

Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t)
{
    // t��0-1�ł͂Ȃ��Ȃ瑊�������l��Ԃ�
    if (t < 0.0f) return a; // �������ꍇ
    if (t > 1.0f) return b; // �傫���ꍇ

    // �N�H�[�^�j�I���̓��ς��v�Z
    float dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
    Quaternion end = b;
    // ���ς����̏ꍇ�Ab�𔽓]���čŒZ�o�H�ŕ�Ԃ���
    if (dot < 0.0f) {
        dot = -dot;
        end = Quaternion(-b.w, -b.x, -b.y, -b.z);
    }
    // �N�H�[�^�j�I�����قړ����ꍇ�͐��`���(Lerp)�ŋߎ�
    if (dot > 0.9995f) {
        return Quaternion(
            a.w + t * (end.w - a.w),
            a.x + t * (end.x - a.x),
            a.y + t * (end.y - a.y),
            a.z + t * (end.z - a.z)
        ).Normalize();
    }
    // ���ʐ��`��ԁiSlerp�j�̌v�Z
    float theta0 = std::acos(dot);  // 2�̃N�H�[�^�j�I���Ԃ̊p�x
    float theta = theta0 * t;       // t�ɉ������p�x
    float sinTheta = std::sin(theta);
    float sinTheta0 = std::sin(theta0);

    float s0 = std::cos(theta) - dot * sinTheta / sinTheta0;    // a�̌W��
    float s1 = sinTheta / sinTheta0;                            // b�̌W��

    // ��Ԍ��ʂ𐳋K�����ĕԂ�
    return Quaternion(
        a.w * s0 + end.w * s1,
        a.x * s0 + end.x * s1,
        a.y * s0 + end.y * s1,
        a.z * s0 + end.z * s1
    ).Normalize();
}

Quaternion ConvMatrix4x4ToQuaternion(const Matrix4x4 mat)
{
    // �e�������ƂɃN�H�[�^�j�I����x, y, z, w�����߂邽�߂̒l���v�Z
    float elem[4];  // 0:x, 1:y, 2:z, 3:w
    elem[0] =  mat.m[0][0] - mat.m[1][1] - mat.m[2][2] + 1.0f;
    elem[1] = -mat.m[0][0] + mat.m[1][1] - mat.m[2][2] + 1.0f;
    elem[2] = -mat.m[0][0] - mat.m[1][1] + mat.m[2][2] + 1.0f;
    elem[3] =  mat.m[0][0] + mat.m[1][1] + mat.m[2][2] + 1.0f;

    // ���l�덷�΍�̂��߁A�ő�l�̃C���f�b�N�X������
    unsigned int biggestIndex = 0;
    for (int i = 1; i < 4; ++i) {
        if (elem[i] > elem[biggestIndex])
            biggestIndex = i;
    }

    // �ő�l��0�����̏ꍇ�ُ͈�ȍs��Ȃ̂�assert
    if (elem[biggestIndex] < 0.0f) {
        assert(false && "�����̍s��ɊԈႢ������");
        return Quaternion();
    }

    // �ő�v�f����N�H�[�^�j�I���̒l���v�Z
    Quaternion ret;
    float v = sqrtf(elem[biggestIndex]) * 0.5f; // �ő听���̒l
    float mult = 0.25f / v;                     // �������v�Z�p�̌W��

    // �ő听���ɉ����Ċe�l���v�Z
    switch (biggestIndex) {
    case 0: // x
        ret.x = v;
        ret.y = (mat.m[0][1] + mat.m[1][0]) * mult;
        ret.z = (mat.m[2][0] + mat.m[0][2]) * mult;
        ret.w = (mat.m[1][2] - mat.m[2][1]) * mult;
        break;

    case 1: // y
        ret.x = (mat.m[0][1] + mat.m[1][0]) * mult;
        ret.y = v;
        ret.z = (mat.m[1][2] + mat.m[2][1]) * mult;
        ret.w = (mat.m[2][0] - mat.m[0][2]) * mult;
        break;

    case 2: // z
        ret.x = (mat.m[2][0] + mat.m[0][2]) * mult;
        ret.y = (mat.m[1][2] + mat.m[2][1]) * mult;
        ret.z = v;
        ret.w = (mat.m[0][1] - mat.m[1][0]) * mult;
        break;

    case 3: // w
        ret.x = (mat.m[1][2] - mat.m[2][1]) * mult;
        ret.y = (mat.m[2][0] - mat.m[0][2]) * mult;
        ret.z = (mat.m[0][1] - mat.m[1][0]) * mult;
        ret.w = v;
        break;
    }

    // ���K��
    ret.Normalized();

    return ret;
}

Matrix4x4 ConvQuaternionToMatrix4x4(const Quaternion q)
{
    Matrix4x4 ret = MatIdentity();

    // ��]�s�񐬕���ݒ�
    ret.m[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
    ret.m[0][1] =        2.0f * q.x * q.y + 2.0f * q.w * q.z;
    ret.m[0][2] =        2.0f * q.x * q.z - 2.0f * q.w * q.y;

    ret.m[1][0] =        2.0f * q.x * q.y - 2.0f * q.w * q.z;
    ret.m[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
    ret.m[1][2] =        2.0f * q.y * q.z + 2.0f * q.w * q.x;

    ret.m[2][0] =        2.0f * q.x * q.z + 2.0f * q.w * q.y;
    ret.m[2][1] =        2.0f * q.y * q.z - 2.0f * q.w * q.x;
    ret.m[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;

    return ret;
}

Vector3 ConvQuaternionToEuler(const Quaternion& q)
{
    float qw = q.w, qx = q.x, qy = q.y, qz = q.z;
    float sqw = qw * qw;
    float sqx = qx * qx;
    float sqy = qy * qy;
    float sqz = qz * qz;

    // �s�b�`�iX����]�j
    float sinp = 2.0f * (qw * qx + qy * qz);
    float cosp = 1.0f - 2.0f * (sqx + sqy);
    float pitch = std::atan2(sinp, cosp);

    // ���[�iY����]�j
    float siny = 2.0f * (qw * qy - qz * qx);
    // �͈͊O�΍�
    if      (siny >  1.0f)  siny =  1.0f;
    else if (siny < -1.0f)  siny = -1.0f;
    float yaw = std::asin(siny);

    // ���[���iZ����]�j
    float sinr = 2.0f * (qw * qz + qx * qy);
    float cosr = 1.0f - 2.0f * (sqy + sqz);
    float roll = std::atan2(sinr, cosr);

    return Vector3(pitch, yaw, roll);
}

Quaternion ConvEulerToQuaternion(const Vector3& euler)
{
    // �I�C���[�p�i���W�A���j
    float pitch = euler.x * 0.5f;   // X��
    float yaw = euler.y * 0.5f;     // Y��
    float roll = euler.z * 0.5f;    // Z��

    float sinPitch = std::sin(pitch);
    float cosPitch = std::cos(pitch);
    float sinYaw = std::sin(yaw);
    float cosYaw = std::cos(yaw);
    float sinRoll = std::sin(roll);
    float cosRoll = std::cos(roll);

    Quaternion q;

    // Yaw-Pitch-Roll(Y��X��Z)�̏��ō���
    q.w = cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll;
    q.x = cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll;
    q.y = sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll;
    q.z = cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll;

    q.Normalized();

    return q;
}
