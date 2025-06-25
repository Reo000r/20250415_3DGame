#pragma once

#include "Vector3.h"

/// <summary>
/// �����A�Փ˔���ɕK�v�ȃf�[�^�̉�
/// </summary>
class Rigidbody
{
public:
	Rigidbody();
	void Init(bool useGravity_ = false);	// ������

	// getter
	const Vector3& GetPos() const { return pos; }
	const Vector3& GetDir() const { return dir; }
	const Vector3& GetVel() const { return vel; }
	bool UseGravity() const { return useGravity; }
	// setter
	void SetPos(const Vector3& set) { pos = set; }
	void SetVel(const Vector3& set);
	void SetUseGravity(bool set) { useGravity = set; }

private:
	Vector3	pos;
	Vector3	dir;
	Vector3	vel;
	bool	useGravity;
};

