#include "Rigidbody.h"

Rigidbody::Rigidbody() :
	pos(),
	dir(),
	vel(),
	useGravity(false)
{
}

void Rigidbody::Init(bool useGravity_)
{
	pos = {};
	dir = {};
	vel = {};
	useGravity = useGravity_;
}

void Rigidbody::SetVel(const Vector3& set)
{
	vel = set;
	// ����������Ȃ�
	if (vel.SqrMagnitude() > 0)
	{
		// ���K�������x�N�g���������Ƃ��đ��
		dir = vel.Normalize();
	}
}
