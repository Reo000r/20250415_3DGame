#include "Arena.h"
#include "Camera.h"
#include "Animator.h"
#include "WeaponPlayer.h"
#include "Input.h"
#include "Collider.h"
#include "ColliderData.h"
#include "Rigidbody.h"
#include "Physics.h"

#include <cassert>
#include <DxLib.h>

namespace {
	Vector3 kStartToEnd = Vector3(0.0f, 100.0f, 0.0f);
	int kColInnerRadius = 700.0f;
	int kColOuterRadius = 2000.0f;
}

Arena::Arena() :
	Collider(PhysicsData::Priority::Static, 
		PhysicsData::GameObjectTag::SystemWall, 
		PhysicsData::ColliderKind::InvertedCylinder, 
		false, true)
{
	rigidbody->Init(false);

	// 当たり判定データ設定
	InvertedCylinderColliderDesc desc;
	desc.startToEnd = kStartToEnd;
	desc.innerRadius = kColInnerRadius;
	desc.outerRadius = kColOuterRadius;
	colliderData = CreateColliderData(
		desc,	// 種別
		false,	// isTrigger
		true	// isCollision
	);

	// 武器とは当たり判定を行わない
	colliderData->AddThroughTag(PhysicsData::GameObjectTag::PlayerAttack);
	colliderData->AddThroughTag(PhysicsData::GameObjectTag::EnemyAttack);

	// モデルの読み込み
	_modelHandle = MV1LoadModel(L"data/model/field/Arena.mv1");
	MV1SetPosition(_modelHandle, Vector3(0, 0, 0));
	MV1SetScale(_modelHandle, Vector3(1, 1, 1) * 0.5f);
}

Arena::~Arena()
{
	MV1DeleteModel(_modelHandle);
}

void Arena::Init(std::weak_ptr<Physics> physics)
{
	// physicsに登録
	EntryPhysics(physics);
}

void Arena::Draw()
{
	MV1DrawModel(_modelHandle);
}

void Arena::OnCollide(const std::weak_ptr<Collider> collider)
{
}
