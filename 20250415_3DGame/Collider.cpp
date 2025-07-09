#include "Collider.h"
#include "Physics.h"
#include "Rigidbody.h"
#include "ColliderDataSphere.h"
#include "ColliderDataCapsule.h"

#include <cassert>

Collider::Collider(PhysicsData::Priority priority_, PhysicsData::GameObjectTag tag_, PhysicsData::ColliderKind colliderKind, bool isTrigger) :
	priority(priority_),
	tag(tag_),
	rigidbody(std::make_shared<Rigidbody>()),
	colliderData(nullptr),
	nextPos()
{
	CreateColliderData(colliderKind, isTrigger);
}

Collider::~Collider()
{
}

void Collider::EntryPhysics(std::weak_ptr<Physics> physics_)
{
	physics = physics_;
	// Physicsに自身を登録
	physics.lock()->Entry(shared_from_this());
}

void Collider::ReleasePhysics()
{
	// Physicsから自身を登録解除
	physics.lock()->Release(shared_from_this());
}

Vector3 Collider::GetPos() const
{
	return rigidbody->GetPos();
}

Vector3 Collider::GetVel() const
{
	return rigidbody->GetVel();
}

Vector3 Collider::GetDir() const
{
	return rigidbody->GetDir();
}

std::shared_ptr<ColliderData> Collider::CreateColliderData(
	PhysicsData::ColliderKind kind, bool isTrigger, 
	float rad, float dist, Vector3 angle)
{
	if (colliderData != nullptr) {
		assert(false && "colliderDataは既に作られている");
		return colliderData;
	}

	// kindに応じたColliderを作成
	switch (kind) {
	case PhysicsData::ColliderKind::Sphere:		// Sphere用の初期化
		colliderData = std::make_shared<ColliderDataSphere>(isTrigger, rad);
		break;
	case PhysicsData::ColliderKind::Capsule:	// Capsule用の初期化
		colliderData = std::make_shared<ColliderDataCapsule>(isTrigger, rad, dist, angle);
		break;
	}
	return colliderData;
}

void Collider::SetColliderData(
	PhysicsData::ColliderKind kind, bool isTrigger, 
	float rad, float dist, Vector3 angle)
{
	if (colliderData == nullptr) {
		assert(false && "colliderDataが作られていない");
		return;
	}

	// 共通のデータを変更
	colliderData->isTrigger = isTrigger;

	// kindに応じたColliderを編集
	switch (kind) {
	case PhysicsData::ColliderKind::Sphere:		// Sphere用の代入
	{
		auto colliderDataSphere = std::static_pointer_cast<ColliderDataSphere>(colliderData);
		colliderDataSphere->_radius = rad;
		colliderData = colliderDataSphere;
		break;
	}
	case PhysicsData::ColliderKind::Capsule:	// Capsule用の代入
	{
		auto colliderDataCapsule = std::static_pointer_cast<ColliderDataCapsule>(colliderData);
		colliderDataCapsule->_radius = rad;
		colliderDataCapsule->_dist = dist;
		colliderDataCapsule->_angle = angle;
		colliderData = colliderDataCapsule;
		break;
	}
	}
	return;
}
