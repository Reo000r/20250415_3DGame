#include "EnemyBase.h"
#include "Player.h"
#include "Camera.h"
#include "Animator.h"
#include "Input.h"
#include "Collider.h"
#include "Rigidbody.h"
#include <cassert>

#include <DxLib.h>
/*
namespace {
	constexpr float kWalkSpeed = 20.0f;
	constexpr float kDashSpeed = 40.0f;
	constexpr float kJumpForce = 20.0f;
	constexpr float kGround = 0.0f;

	const std::wstring kAnimName = L"CharacterArmature|";
	const std::wstring kAnimNameIdle = kAnimName + L"Idle";
	const std::wstring kAnimNameWalk = kAnimName + L"Walk";
	const std::wstring kAnimNameRun = kAnimName + L"Run";
	const std::wstring kAnimNameJump = kAnimName + L"Jump_Idle";
}

EnemyBase::EnemyBase() :
	Collider(PhysicsData::Priority::High,
		PhysicsData::GameObjectTag::Player,
		PhysicsData::ColliderKind::Sphere,
		false),
	_nowUpdateState(&EnemyBase::UpdateIdle),
	_animator(std::make_shared<Animator>()),
	_rotAngle(0.0f)
{
	rigidbody->Init(true);

	// モデルの読み込み
	_animator->Init(MV1LoadModel(L"data/model/Player.mv1"));

	// 使用するアニメーションを全て入れる
	_animator->SetAnimData(kAnimNameIdle, true);
	_animator->SetAnimData(kAnimNameWalk, true);
	_animator->SetAnimData(kAnimNameRun, true);
	_animator->SetAnimData(kAnimNameJump, true);
	// 最初のアニメーションを設定する
	_animator->SetStartAnim(kAnimNameIdle);
{
}
*/