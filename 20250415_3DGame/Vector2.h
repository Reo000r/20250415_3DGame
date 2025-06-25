#pragma once

class Vector2;
typedef Vector2 Position2;
class Vector3;

class Vector2 final {
public:
	float x, y;

public:
	Vector2() : x(0.0f), y(0.0f)
	{
	}

	Vector2(float _x, float _y) : x(_x), y(_y)
	{
	}

	void operator+=(const Vector2& vec);
	void operator-=(const Vector2& vec);
	void operator*=(float scale);
	void operator/=(float scale);
	Vector2 operator+(const Vector2& vec) const;
	Vector2 operator-(const Vector2& vec) const;
	Vector2 operator*(float scale) const;
	Vector2 operator/(float scale) const;

	void operator=(const Vector3& vec3);
	void operator+=(const Vector3& vec3);
	void operator-=(const Vector3& vec3);
	Vector3 operator+(const Vector3& vec3) const;

	/// <summary>
	/// �������]
	/// </summary>
	/// <returns></returns>
	Vector2 operator-() const;

	// �֗��Ȋ֐��Q

	/// <summary>
	/// ���̃x�N�g����2��̒�����Ԃ�
	/// </summary>
	/// <returns></returns>
	float SqrMagnitude() const;

	/// <summary>
	/// �x�N�g����2��̒�����Ԃ�
	/// </summary>
	/// <param name="a"></param>
	/// <returns></returns>
	float SqrMagnitude(const Vector2& a)const;

	/// <summary>
	/// �x�N�g����2��̒�����Ԃ�
	/// </summary>
	/// <param name="_x"></param>
	/// <param name="_y"></param>
	/// <returns></returns>
	float SqrMagnitude(float _x, float _y)const;

	/// <summary>
	/// ���̃x�N�g���̒�����Ԃ�
	/// </summary>
	/// <returns></returns>
	float Magnitude() const;

	/// <summary>
	/// �x�N�g���̒�����Ԃ�
	/// </summary>
	/// <param name="a"></param>
	/// <returns></returns>
	float Magnitude(const Vector2& a) const;

	/// <summary>
	/// �x�N�g���̒�����Ԃ�
	/// </summary>
	/// <param name="_x"></param>
	/// <param name="_y"></param>
	/// <returns></returns>
	float Magnitude(float _x, float _y) const;

	/// <summary>
	/// a��b�̊Ԃ̋�����Ԃ�
	/// (Distance(a, b) �� (a-b).magnitude�Ɠ���)
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns></returns>
	float Distance(const Vector2& a, const Vector2& b) const;

	/// <summary>
	/// ���K��(normalized)���ꂽ�Ƃ��A�������ێ������܂܂�
	/// ������ 1.0 �̃x�N�g����Ԃ�
	/// </summary>
	/// <param name="a"></param>
	/// <returns></returns>
	void Normalized();

	/// <summary>
	/// ���K��(normalized)���ꂽ�Ƃ��A�������ێ������܂܂�
	/// ������ 1.0 �̃x�N�g����Ԃ�
	/// </summary>
	/// <param name="a"></param>
	/// <returns></returns>
	Vector2 Normalize() const;

	/// <summary>
	/// a����b �� t�̊��������߂Â����_��Ԃ�
	/// t�� 0-1 �͈̔�
	/// t�� 0   �̏ꍇ�Aa ��Ԃ�
	/// t�� 1   �̏ꍇ�Ab ��Ԃ�
	/// t�� 0.5 �̏ꍇ�Aa �� b �̒��_���Ԃ����
	/// t�� 0�ȉ� �̏ꍇ a ��Ԃ�(t=0�Ɠ���)
	/// t�� 1�ȏ� �̏ꍇ b ��Ԃ�(t=1�Ɠ���)
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	Vector2 Lerp(const Vector2& a, const Vector2& b, const float& t) const;
};