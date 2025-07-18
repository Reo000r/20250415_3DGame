#include "EnemyBase.h"
#include "Player.h"
#include "Camera.h"
#include "Animator.h"
#include "Input.h"
#include "Collider.h"
#include "Rigidbody.h"
#include <cassert>

#include <DxLib.h>

EnemyBase::EnemyBase(float hitPoint, float transferAttackRad, float attackMul) :
	Collider(PhysicsData::Priority::Middle,
		PhysicsData::GameObjectTag::Enemy,
		PhysicsData::ColliderKind::Sphere,
		false),
	_animator(std::make_shared<Animator>()),
	_rotAngle(0.0f),
	_rotMtx(),
	_quaternion(),
	_hitPoint(hitPoint),
	_transferAttackRad(transferAttackRad),
	_attackMul(attackMul)
{
	
}

EnemyBase::~EnemyBase()
{
}
