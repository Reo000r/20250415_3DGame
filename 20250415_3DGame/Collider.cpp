#include "Collider.h"
#include "Physics.h"
#include "Rigidbody.h"
#include "ColliderDataSphere.h"

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

std::shared_ptr<ColliderData> Collider::CreateColliderData(PhysicsData::ColliderKind kind, bool isTrigger)
{
	if (colliderData != nullptr) {
		assert(false && "colliderDataは既に作られている");
		return colliderData;
	}

	// kindに応じたColliderを作成
	switch (kind) {
	case PhysicsData::ColliderKind::Cube:
		// Cube用の初期化
		break;
	case PhysicsData::ColliderKind::Sphere:
		colliderData = std::make_shared<ColliderDataSphere>(isTrigger);
		break;
	case PhysicsData::ColliderKind::Capsule:
		// Capsule用の初期化
		break;
	}
	return colliderData;
}
