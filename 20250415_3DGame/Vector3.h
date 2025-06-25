#pragma once

// �ÖٓI�^�ϊ��̂��߂̃v���g�^�C�v�錾
namespace DxLib {
	struct tagVECTOR;
}

class Vector2;
class Vector3;
typedef Vector3 Position3;

class Vector3 final {
public:
	float x, y, z;

	Vector3() : x(0.0f), y(0.0f), z(0.0f) {};
	Vector3(float x_, float y_, float z_) :
		x(x_), y(y_), z(z_) // (_x�����ƃ����o�̖����Ɣ�邽��)
	{
	};

	// VECTOR����Vector3�AVector3����VECTOR��
	// �ÖٓI�Ɍ^�ϊ����邽�߂̂���
	operator DxLib::tagVECTOR();
	operator DxLib::tagVECTOR() const;
	Vector3(const DxLib::tagVECTOR vector);

	// �I�y���[�^�I�[�o�[���[�h
	// (���Z��assert�g�p)
	Vector3 operator+(const Vector3& v) const;
	Vector3 operator-(const Vector3& v) const;
	Vector3 operator*(const float& m) const;
	Vector3 operator/(const float& d) const;
	void operator+=(const Vector3& v);
	void operator-=(const Vector3& v);
	void operator*=(const float& m);
	void operator/=(const float& d);

	void operator=(const Vector2& vec2);
	void operator+=(const Vector2& vec2);
	Vector3 operator+(const Vector2& vec2) const;

	/// <summary>
	/// �������]
	/// </summary>
	/// <returns></returns>
	Vector3 operator-() const;

	/// <summary>
	/// �x�N�g���̒�����Ԃ�
	/// </summary>
	/// <returns></returns>
	float Magnitude() const;
	/// <summary>
	/// �x�N�g���̒�����Ԃ�
	/// </summary>
	/// <returns></returns>
	float Magnitude(const Vector3& v) const;
	/// <summary>
	/// �x�N�g���̒�����Ԃ�
	/// </summary>
	/// <returns></returns>
	float Magnitude(const float& x_, const float& y_, const float& z_) const;

	/// <summary>
	/// �x�N�g���̓��̒�����Ԃ�
	/// </summary>
	/// <returns></returns>
	float SqrMagnitude() const;
	/// <summary>
	/// �x�N�g���̓��̒�����Ԃ�
	/// </summary>
	/// <returns></returns>
	float SqrMagnitude(const Vector3& v) const;
	/// <summary>
	/// �x�N�g���̓��̒�����Ԃ�
	/// </summary>
	/// <returns></returns>
	float SqrMagnitude(const float& x_, const float& y_, const float& z_) const;

	/// <summary>
	/// a��b�̊Ԃ̋�����Ԃ�
	/// (Distance(a, b) �� (a-b).magnitude�Ɠ���)
	/// </summary>
	/// <returns></returns>
	float Distance(const Vector3& a, const Vector3& b) const;

	/// <summary>
	/// ���K������
	/// </summary>
	void Normalized();
	/// <summary>
	/// ���K�������x�N�g����Ԃ�
	/// </summary>
	/// <returns></returns>
	Vector3 Normalize() const;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="v"></param>
	/// <param name="scale"></param>
	/// <returns></returns>
	void VecScale(const float& scale);
};

/// <summary>
/// a����b �� t�̊��������߂Â����_��Ԃ�
/// t�� 0-1 �͈̔�
/// t�� 0   �̏ꍇ�Aa ��Ԃ�
/// t�� 1   �̏ꍇ�Ab ��Ԃ�
/// t�� 0.5 �̏ꍇ�Aa �� b �̒��_���Ԃ����
/// t�� 0�ȉ� �̏ꍇ a ��Ԃ�(t=0�Ɠ���)
/// t�� 1�ȏ� �̏ꍇ b ��Ԃ�(t=1�Ɠ���)
/// </summary>
/// <param name="a">�n�_</param>
/// <param name="b">�I�_</param>
/// <param name="t">�͈�</param>
/// <returns></returns>
Vector3 Lerp(const Vector3& va, const Vector3& vb, const float& t);

/// <summary>
/// �X�P�[�����O�����x�N�g����Ԃ�
/// </summary>
/// <param name="v"></param>
/// <param name="scale"></param>
/// <returns></returns>
Vector3 VecScale(const Vector3& v, const float& scale);

/// <summary>
/// ���ς�Ԃ�
/// </summary>
/// <param name="va"></param>
/// <param name="vb"></param>
/// <returns></returns>
float Dot(const Vector3& va, const Vector3& vb);

/// <summary>
/// �O�ς�Ԃ�
/// (���炭�s���S)
/// </summary>
/// <param name="va"></param>
/// <param name="vb"></param>
/// <returns></returns>
//float Cross(const Vector3& va, const Vector3& vb);

/// <summary>
/// �O�ς�Ԃ�
/// </summary>
/// <param name="va"></param>
/// <param name="vb"></param>
/// <returns></returns>
Vector3 Cross(const Vector3& va, const Vector3& vb);

/// <summary>
/// ����
/// </summary>
/// <param name="va"></param>
/// <param name="vb"></param>
/// <returns></returns>
float operator*(const Vector3& va, const Vector3& vb);

/// <summary>
/// �O��
/// </summary>
/// <param name="va"></param>
/// <param name="vb"></param>
/// <returns></returns>
Vector3 operator%(const Vector3& va, const Vector3& vb);

/// <summary>
/// ���˃x�N�g�������߂�
/// </summary>
/// <param name="v">���˂�����x�N�g��</param>
/// <param name="normal">�@���x�N�g���Ȃ�</param>
/// <returns></returns>
//Vector3 Reflect(const Vector3 v, Vector3 normal);