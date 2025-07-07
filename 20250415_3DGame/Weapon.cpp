#include "Weapon.h"
#include "Rigidbody.h"
#include <cassert>

#include <DxLib.h>

Weapon::Weapon() :
	Collider(PhysicsData::Priority::Static, 
		PhysicsData::GameObjectTag::PlayerAttack, 
		PhysicsData::ColliderKind::Capsule, 
		true),
	_modelHandle(-1)
{
	rigidbody->Init(false);
}

Weapon::~Weapon()
{
}

void Weapon::Init(int modelHandle, float rad, Vector3 start, Vector3 end)
{
	assert(modelHandle >= 0 && "���f���n���h�����������Ȃ�");
	_modelHandle = modelHandle;

	// �����蔻��̃f�[�^��K�p
	SetColliderData(PhysicsData::ColliderKind::Capsule, true, rad, start, end);
}

void Weapon::Update(Matrix4x4 worldMatrix)
{
	// ���[���h�s���K�p
	MV1SetMatrix(_modelHandle, worldMatrix);
}

void Weapon::Draw()
{
	// �`��
	MV1DrawModel(_modelHandle);
}
