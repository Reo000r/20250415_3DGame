#pragma once
#include <array>		// Matrix�p

// ���ݕϊ��̂��߂̃v���g�^�C�v�錾
namespace DxLib {
	struct tagMATRIX;
}

class Vector3;

/// <summary>
/// �s��N���X
/// �s�񐬕���DirectX�`��(�s���������̌���)
/// (m[�s(��)][��(�c)])
/// </summary>
class Matrix4x4 final {
public:
	Matrix4x4() : m({}) {};
	Matrix4x4(std::array<std::array<float, 4>, 4> m_) : m(m_) {};
	std::array<std::array<float, 4>, 4> m;

	// tagMATRIX�Ƃ̑��ݕϊ�
	operator DxLib::tagMATRIX();
	operator DxLib::tagMATRIX() const;
	Matrix4x4(const DxLib::tagMATRIX& mat);

	Matrix4x4 operator*(const Matrix4x4& mat) const;	// ��Z
	void operator*=(const Matrix4x4& mat);	// ��Z
	bool operator==(const Matrix4x4& mat);	// ������r

	/// <summary>
	/// ���g���g���x�N�g�����s���Z�����ʂ�Ԃ�
	/// </summary>
	/// <param name="vec">�x�N�g��</param>
	/// <returns></returns>
	Vector3 VecMultiple(const Vector3& vec) const;

	/// <summary>
	/// �s��������{����
	/// </summary>
	/// <param name="scale">�{��</param>
	/// <returns></returns>
	void MatScale(const float& scale);

	/// <summary>
	/// DxLib�̍s���ϊ�����֐�
	/// </summary>
	/// <param name="mat"></param>
	/// <returns></returns>
	DxLib::tagMATRIX GetMATRIX() const;
};

/// <summary>
/// �P�ʍs���Ԃ�
/// </summary>
/// <returns></returns>
Matrix4x4 MatIdentity();

/// <summary>
/// ���s�ړ��s���Ԃ�
/// </summary>
/// <param name="vec">���s�ړ���</param>
/// <returns></returns>
Matrix4x4 MatTranslate(const Vector3& vec);
/// <summary>
/// ���s�ړ��s���Ԃ�
/// </summary>
/// <param name="x">x�������s�ړ���</param>
/// <param name="y">y�������s�ړ���</param>
/// <param name="z">z�������s�ړ���</param>
/// <returns></returns>
Matrix4x4 MatTranslate(const float& x, const float& y, const float& z);

/// <summary>
/// y����]�s���Ԃ�
/// </summary>
/// <param name="angle">��]�l</param>
/// <returns></returns>
Matrix4x4 MatRotateY(const float& angle);

/// <summary>
/// ��̍s��̏�Z��Ԃ�
/// </summary>
/// <param name="lmat">���Ӓl</param>
/// <param name="rmat">�E�Ӓl</param>
/// <returns></returns>
Matrix4x4 MatMultiple(const Matrix4x4& lmat, const Matrix4x4& rmat);

/// <summary>
/// �x�N�g���ɑ΂��čs���Z�����ʂ�Ԃ�
/// </summary>
/// <param name="mat">�s��</param>
/// <param name="vec">�x�N�g��</param>
/// <returns></returns>
Vector3 VecMultiple(const Matrix4x4& mat, const Vector3& vec);

/// <summary>
/// �g��s���Ԃ�
/// </summary>
/// <param name="scale">�g�嗦</param>
/// <returns></returns>
Matrix4x4 MatGetScale(const Vector3& scale);

/// <summary>
/// �t�s��̓��o
/// </summary>
/// <param name="mat">�s��</param>
/// <returns></returns>
Matrix4x4 MatInverse(const Matrix4x4& mat);

/// <summary>
/// �]�u�s���Ԃ�
/// </summary>
/// <param name="mat">�s��</param>
/// <returns></returns>
Matrix4x4 MatTranspose(const Matrix4x4& mat);
