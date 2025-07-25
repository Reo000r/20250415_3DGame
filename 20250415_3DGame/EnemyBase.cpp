﻿#include "EnemyBase.h"
#include "Player.h"
#include "Camera.h"
#include "Animator.h"
#include "Input.h"
#include "Collider.h"
#include "ColliderData.h"
#include "Rigidbody.h"
#include <cassert>

#include <DxLib.h>

EnemyBase::EnemyBase(float hitPoint, float transferAttackRad, float attackPower) :
	Collider(PhysicsData::Priority::Middle,
		PhysicsData::GameObjectTag::Enemy,
		PhysicsData::ColliderKind::Capsule,
		false, true),
	_animator(std::make_unique<Animator>()),
	_rotAngle(0.0f),
	_rotMtx(),
	_quaternion(),
	_hitPoint(hitPoint),
	_transferAttackRad(transferAttackRad),
	_attackPower(attackPower),
	_state(State::Spawning),
	_reactCooltime(0)
{
	// 自身の武器やほかの敵の武器とは当たり判定を行わない
	colliderData->AddThroughTag(PhysicsData::GameObjectTag::EnemyAttack);
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::SetPos(const Vector3& pos)
{
	rigidbody->SetPos(pos);
}
