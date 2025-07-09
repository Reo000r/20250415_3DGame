#include "Weapon.h"
#include "Rigidbody.h"
#include "ColliderData.h"
#include "ColliderDataCapsule.h"
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

void Weapon::Init(int modelHandle, float rad, float dist, Vector3 angle)
{
	assert(modelHandle >= 0 && "���f���n���h�����������Ȃ�");
	_modelHandle = modelHandle;

	// �����蔻��̃f�[�^��K�p
	SetColliderData(PhysicsData::ColliderKind::Capsule, true, rad, dist, angle);
}

void Weapon::Update(Matrix4x4 worldMatrix)
{
	auto capsuleData = std::static_pointer_cast<ColliderDataCapsule>(colliderData);
	Position3 start = capsuleData->GetStartPos(Vector3());
	Position3 end = capsuleData->GetEndPos(Vector3());
	Matrix4x4 localMat = MatTranslate(end - start);

	worldMatrix = MatMultiple(localMat, worldMatrix);

	// ���[���h�s���K�p
	MV1SetMatrix(_modelHandle, worldMatrix);

	// �s�񂩂�ʒu���������o���đ��
	Position3 pos = Vector3(worldMatrix.m[3][0], worldMatrix.m[3][1], worldMatrix.m[3][2]);
	rigidbody->SetPos(pos);

	// MEMO:trans, frame, scale(, rotY
}

void Weapon::Draw()
{
	// �`��
	MV1DrawModel(_modelHandle);
}

void Weapon::OnCollide(const std::weak_ptr<Collider> collider)
{
}
