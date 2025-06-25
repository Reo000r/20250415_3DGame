#include "Player.h"
#include "Camera.h"
#include "Animator.h"
#include "Input.h"
#include "Collider.h"
#include "Rigidbody.h"
#include <cassert>

#include <DxLib.h>

#define ANIMATION_TEST

namespace {
	constexpr float kWalkSpeed = 20.0f;
	constexpr float kDashSpeed = 40.0f;
	constexpr float kJumpForce = 20.0f;
	constexpr float kGround = 0.0f;

#ifndef ANIMATION_TEST
	const std::wstring kAnimName = L"Armature|Animation";
	const std::wstring kAnimNameJump = kAnimName + L"Jump";
#else
	const std::wstring kAnimName		= L"CharacterArmature|";
	const std::wstring kAnimNameIdle	= kAnimName + L"Idle";
	const std::wstring kAnimNameWalk	= kAnimName + L"Walk";
	const std::wstring kAnimNameRun		= kAnimName + L"Run";
	const std::wstring kAnimNameJump	= kAnimName + L"Jump_Idle";
#endif
}

Player::Player() :
	Collider(PhysicsData::Priority::High,
		PhysicsData::GameObjectTag::Player,
		PhysicsData::ColliderKind::Sphere,
		false),
	_nowUpdateState(&Player::UpdateIdle),
	_animator(std::make_shared<Animator>()),
	_rotAngle(0.0f)
{
	rigidbody->Init(true);

#ifndef ANIMATION_TEST
	// モデルの読み込み
	_animator->Init(MV1LoadModel(L"data/model/Player.mv1"));
#else
	// モデルの読み込み
	_animator->Init(MV1LoadModel(L"data/model/Player_test.mv1"));
#endif

#ifndef ANIMATION_TEST
	// 使用するアニメーションを全て入れる
	_animator->SetAnimData(kAnimNameJump, true);
	// 最初のアニメーションを設定する
	_animator->SetStartAnim(kAnimNameJump);
#else
	// 使用するアニメーションを全て入れる
	_animator->SetAnimData(kAnimNameIdle, true);
	_animator->SetAnimData(kAnimNameWalk, true);
	_animator->SetAnimData(kAnimNameRun, true);
	_animator->SetAnimData(kAnimNameJump, true);
	// 最初のアニメーションを設定する
	_animator->SetStartAnim(kAnimNameIdle);
#endif
	
}

Player::~Player() {
}

void Player::Init(std::weak_ptr<Camera> camera) {
	_camera = camera;
	MV1SetRotationXYZ(_animator->GetModelHandle(), Vector3(0, _rotAngle, 0));
}

void Player::Update() {
	_animator->Update();
	// 現在のステートに応じたUpdateが行われる
	(this->*_nowUpdateState)();
}

void Player::Draw() {
	// 当たり判定を行ってからモデルの位置を設定する
	MV1SetPosition(_animator->GetModelHandle(), GetPos());
	// モデルの描画
	MV1DrawModel(_animator->GetModelHandle());

#ifdef _DEBUG
	int color = 0xffffff;
	int y = 16 * 2;
	DrawFormatString(0, y, color, L"Player:Pos (%.3f,%.3f,%.3f)", GetPos().x, GetPos().y, GetPos().z);
	y += 16;
	DrawFormatString(0, y, color, L"Player:Vel (%.3f,%.3f,%.3f)", GetVel().x, GetVel().y, GetVel().z);
	y += 16;
	DrawFormatString(0, y, color, L"Player:RotAngle (%.3f)", _rotAngle);
#endif
}

void Player::OnCollide(const std::weak_ptr<Collider> colider)
{
	// coliderと衝突
}

void Player::UpdateIdle()
{
	Input& input = Input::GetInstance();
	Vector3 stick = input.GetPadLeftSitck();
	// 入力があればステートを変更する
	if ((int)stick.x != 0 || (int)stick.z != 0) {
		// 追加の入力があればダッシュ
		if (input.IsPress("dash")) {
			_animator->ChangeAnim(kAnimNameRun, true);
			_nowUpdateState = &Player::UpdateDash;
		}
		else {
			_animator->ChangeAnim(kAnimNameWalk, true);
			_nowUpdateState = &Player::UpdateWalk;
		}
	}
	Vector3 vel = GetVel();
	// ジャンプ処理
	if (input.IsTrigger("jump") && GetPos().y <= kGround) {
		vel.y += kJumpForce;
		_animator->ChangeAnim(kAnimNameJump, true);
		_nowUpdateState = &Player::UpdateJump;
	}
	const float cameraRot = _camera.lock()->GetRotAngleX();
	
	// カメラから見た移動に変換する
	_rotMtx = MatRotateY(cameraRot);
	vel = VecMultiple(_rotMtx, vel);

	// rigidbodyに編集した移動量を代入
	rigidbody->SetVel(vel);


//	_animator->Update();
}

void Player::UpdateWalk()
{
	// 移動関連処理
	Move(kWalkSpeed);
//	_animator->Update();
}

void Player::UpdateDash()
{
	// 移動関連処理
	Move(kDashSpeed);
//	_animator->Update();
}

void Player::UpdateJump()
{
	if (GetPos().y <= kGround) {
		Input& input = Input::GetInstance();
		Vector3 stick = input.GetPadLeftSitck();
		// 入力があればステートを変更する
		if ((int)stick.x != 0 || (int)stick.z != 0) {
			// 追加の入力があればダッシュ
			if (input.IsPress("dash")) {
				_animator->ChangeAnim(kAnimNameRun, true);
				_nowUpdateState = &Player::UpdateDash;
			}
			else {
				_animator->ChangeAnim(kAnimNameWalk, true);
				_nowUpdateState = &Player::UpdateWalk;
			}
		}
		else {
			_animator->ChangeAnim(kAnimNameIdle, true);
			_nowUpdateState = &Player::UpdateIdle;
		}
	}
	Move(kDashSpeed);

//	_animator->Update();
}

void Player::UpdateAttack()
{
}

void Player::Move(const float speed) {
	// 移動処理
	Walk(speed);
	// 進行方向への方向転換処理
	Rotate();
}

void Player::Walk(const float speed) {
	Vector3 dir = GetDir();
	Vector3 vel = GetVel();
	Position3 pos = GetPos();	// 移動予定位置
	Input& input = Input::GetInstance();
	const float cameraRot = _camera.lock()->GetRotAngleX();

	// スティックによる平面移動
	Vector3 stick = Input::GetInstance().GetPadLeftSitck();
	
	if (GetPos().y <= kGround) {
		// もし入力がないかつ移動量もないなら
		if (((int)stick.x == 0 && (int)stick.z == 0) &&
			vel.SqrMagnitude() < PhysicsData::sleepThreshold) {
			_animator->ChangeAnim(kAnimNameIdle, true);
			_nowUpdateState = &Player::UpdateIdle;
			return;
		}
		else if (input.IsPress("dash") &&
			_nowUpdateState != &Player::UpdateDash) {
			_animator->ChangeAnim(kAnimNameRun, true);
			_nowUpdateState = &Player::UpdateDash;
			return;
		}
		else if (!input.IsPress("dash") &&
			_nowUpdateState != &Player::UpdateWalk) {
			_animator->ChangeAnim(kAnimNameWalk, true);
			_nowUpdateState = &Player::UpdateWalk;
			return;
		}
	}

	// 入力が入っていない時でもxに-0.0fが入っている
	dir.x = -stick.x;
	dir.z = stick.z;

	// ジャンプ処理
	if (input.IsTrigger("jump") && GetPos().y <= kGround) {
		vel.y += kJumpForce;
		_animator->ChangeAnim(kAnimNameJump, true);
		_nowUpdateState = &Player::UpdateJump;
	}

	dir.Normalized();

	vel.x += dir.x * speed;
	vel.z += dir.z * speed;

	// カメラから見た移動に変換する
	_rotMtx = MatRotateY(cameraRot);
	vel = VecMultiple(_rotMtx, vel);

	pos += vel;
	if (pos.y < kGround) {
		pos.y = kGround;
		vel.y = 0.0f;
	}

	// rigidbodyに編集した移動量を代入
	rigidbody->SetVel(vel);
}

void Player::Rotate() {
#ifdef USE_STICK
	// スティックによる平面移動
	Vector3 stick = Input::GetInstance().GetPadLeftSitck();
	// 入力があった場合のみキャラクターの向きを変更
	if (stick.x != 0.0f || stick.z != 0.0f) {
		const float cameraRot = _camera.lock()->GetRotAngleX();
		float a = atan2f(stick.z, stick.x) + -cameraRot + DX_PI_F * 0.5f;
		MV1SetRotationXYZ(_animator->GetModelHandle(), Vector3(0, a, 0));
	}
#else
	MV1SetRotationXYZ(_modelHandle, Vector3(0, _rotAngle, 0));
#endif
}
