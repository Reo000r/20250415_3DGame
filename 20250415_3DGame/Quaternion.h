#pragma once

class Vector3;
class Matrix4x4;

/// <summary>
/// �N�H�[�^�j�I��(�l����)
/// </summary>
class Quaternion final {
public:
	float w, x, y, z;

	Quaternion();
	/// <summary>
	/// ���Ԓ���
	/// </summary>
	Quaternion(float _w, float _x, float _y, float _z);

	Quaternion operator*(const float& s) const;	// ��]����
	void operator*=(const float& s);			// ��]����
	bool operator==(const Quaternion& q) const;	// ������r

	/// <summary>
	/// ���K������
	/// </summary>
	void Normalized();
	/// <summary>
	/// ���K�������l��Ԃ�
	/// </summary>
	Quaternion Normalize() const;

	/// <summary>
	/// �t�l����(�C���o�[�X)��Ԃ�
	/// </summary>
	Quaternion Inverse() const;
};

/// <summary>
/// �N�H�[�^�j�I�����m�̉�]����
/// </summary>
/// <param name="lQ"></param>
/// <param name="rQ"></param>
/// <returns></returns>
Quaternion operator*(const Quaternion lQ, const Quaternion rQ);

/// <summary>
/// �x�N�g���ɑ΂��N�H�[�^�j�I���̉�]��K�p����
/// ���Z�q�I�[�o�[���[�h
/// </summary>
/// <param name="qRot"></param>
/// <param name="right"></param>
/// <returns>��]��̃x�N�g��</returns>
Vector3 operator*(const Quaternion qRot, const Vector3 right);

/// <summary>
/// �x�N�g���ɑ΂��N�H�[�^�j�I���̉�]��K�p����
/// </summary>
/// <param name="qRot"></param>
/// <param name="right"></param>
/// <returns>��]��̃x�N�g��</returns>
Vector3 RotateVector3(const Quaternion qRot, const Vector3 right);

/// <summary>
/// ��]���Ɖ�]�p�����]�N�H�[�^�j�I����Ԃ�
/// </summary>
/// <param name="axis">��]���x�N�g��</param>
/// <param name="angle">��]�p(���W�A��)</param>
/// <returns>��]�N�H�[�^�j�I��</returns>
Quaternion AngleAxis(const Vector3& axis, const float& angle);

/// <summary>
/// ��̎l�����Ԃ̕��
/// </summary>
/// <param name="a">start</param>
/// <param name="b">end</param>
/// <param name="t">�͈�(0.0-1.0)</param>
/// <returns>�ۊǂ��ꂽ�N�H�[�^�j�I��</returns>
Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

/// <summary>
/// �s�񂩂�N�H�[�^�j�I���̕ϊ�
/// �����ȉ�]�s���n���Ȃ��ƕs����o��
/// </summary>
/// <returns></returns>
Quaternion ConvMatrix4x4ToQuaternion(const Matrix4x4 mat);

/// <summary>
/// �N�H�[�^�j�I������s��̕ϊ�
/// �Q�l:http://marupeke296.com/DXG_No58_RotQuaternionTrans.html
/// </summary>
/// <returns></returns>
Matrix4x4 ConvQuaternionToMatrix4x4(const Quaternion q);

/// <summary>
/// �N�H�[�^�j�I������I�C���[�p(Vector3)�ւ̕ϊ����s��
/// �W���o�����b�N�������������ȕt�߂͒���
/// </summary>
/// <param name="q"></param>
/// <returns></returns>
Vector3 ConvQuaternionToEuler(const Quaternion& q);

/// <summary>
/// �I�C���[�p(Vector3)����N�H�[�^�j�I���ւ̕ϊ����s��
/// </summary>
/// <param name="q"></param>
/// <returns></returns>
Quaternion ConvEulerToQuaternion(const Vector3& euler);
