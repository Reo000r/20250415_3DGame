#include "Player.h"
#include "Camera.h"
#include "Animator.h"
#include "Weapon.h"
#include "Input.h"
#include "Collider.h"
#include "ColliderData.h"
#include "Rigidbody.h"
#include <cassert>

#include <DxLib.h>

//#define ANIMATION_TEST
// Quaternionを使用した回転
#define USE_QUATERNION
// 
//#define OLD_STATETRANSITION

namespace {
	constexpr float kWalkSpeed = 7.0f;
	constexpr float kDashSpeed = 14.0f;
	constexpr float kJumpForce = 20.0f;
	constexpr float kGround = 0.0f;

#ifndef ANIMATION_TEST
	const std::wstring kAnimName = L"Armature|Animation_";
	const std::wstring kAnimNameIdle = kAnimName + L"Idle";
	const std::wstring kAnimNameWalk = kAnimName + L"Walk";
	const std::wstring kAnimNameRun = kAnimName + L"Run";
	//const std::wstring kAnimNameAttack = kAnimName + L"Attack360High";
	const std::wstring kAnimNameAttackCombo1 = kAnimName + L"AttackCombo1";
	const std::wstring kAnimNameAttackCombo2 = kAnimName + L"AttackCombo2";
	const std::wstring kAnimNameAttackCombo3 = kAnimName + L"AttackCombo3";
	const std::wstring kAnimNameDamage = kAnimName + L"React";
	const std::wstring kAnimNameDead = kAnimName + L"Dead";

	constexpr float kAttackCombo1InputStart = 0.0f;
	constexpr float kAttackCombo1InputEnd	= 1.0f;
	constexpr float kAttackCombo2InputStart = 0.0f;
	constexpr float kAttackCombo2InputEnd	= 1.0f;
	constexpr float kAttackCombo3InputStart = 0.0f;
	constexpr float kAttackCombo3InputEnd	= 1.0f;

#else
	const std::wstring kAnimName		= L"CharacterArmature|";
	const std::wstring kAnimNameIdle	= kAnimName + L"Idle";
	const std::wstring kAnimNameWalk	= kAnimName + L"Walk";
	const std::wstring kAnimNameRun		= kAnimName + L"Run";
	const std::wstring kAnimNameJump	= kAnimName + L"Jump_Idle";
	const std::wstring kAnimNameAttackFirst		= kAnimName + L"Attack_First";
	const std::wstring kAnimNameAttackSecond	= kAnimName + L"Attack_Second";
	const std::wstring kAnimNameAttackThird		= kAnimName + L"Attack_Third";
	const std::wstring kAnimNameDamage	= kAnimName + L"Damage";
	const std::wstring kAnimNameDead	= kAnimName + L"Dead";
#endif
}

Player::Player() :
	Collider(PhysicsData::Priority::High,
		PhysicsData::GameObjectTag::Player,
		PhysicsData::ColliderKind::Sphere,
		false),
	_nowUpdateState(&Player::UpdateIdle),
	_animator(std::make_shared<Animator>()),
	_weapon(std::make_shared<Weapon>()),
	_rotAngle(0.0f),
	_hasDerivedAttackInput(false)
{
	rigidbody->Init(true);

	// 自身の武器とは当たり判定を行わない
	colliderData->AddThroughTag(PhysicsData::GameObjectTag::PlayerAttack);

#ifndef ANIMATION_TEST
	// モデルの読み込み
	_animator->Init(MV1LoadModel(L"data/model/character/Player.mv1"));
#else
	// モデルの読み込み
	_animator->Init(MV1LoadModel(L"data/model/Player_test.mv1"));
#endif

#ifndef ANIMATION_TEST
	// 使用するアニメーションを全て入れる
	_animator->SetAnimData(kAnimNameIdle, true);
	_animator->SetAnimData(kAnimNameWalk, true);
	_animator->SetAnimData(kAnimNameRun, true);
	_animator->SetAnimData(kAnimNameAttackCombo1, false);
	_animator->SetAnimData(kAnimNameAttackCombo2, false);
	_animator->SetAnimData(kAnimNameAttackCombo3, false);
	_animator->SetAnimData(kAnimNameDamage, false);
	_animator->SetAnimData(kAnimNameDead, false);
	// 最初のアニメーションを設定する
	_animator->SetStartAnim(kAnimNameIdle);
#else
	// 使用するアニメーションを全て入れる
	_animator->SetAnimData(kAnimNameIdle, true);
	_animator->SetAnimData(kAnimNameWalk, true);
	_animator->SetAnimData(kAnimNameRun, true);
	_animator->SetAnimData(kAnimNameJump, true);

	_animator->SetAnimData(kAnimNameAttackFirst, false);
	_animator->SetAnimData(kAnimNameAttackSecond, false);
	_animator->SetAnimData(kAnimNameAttackThird, false);
	_animator->SetAnimData(kAnimNameDamage, false);
	_animator->SetAnimData(kAnimNameDead, false);
	
	// 最初のアニメーションを設定する
	_animator->SetStartAnim(kAnimNameIdle);
#endif

	// 武器初期化
	int weaponModelHandle = MV1LoadModel(L"data/model/weapon/PlayerWeapon.mv1");
	assert(weaponModelHandle >= 0 && "モデルハンドルが正しくない");
	_weapon->Init(weaponModelHandle, 100.0f,
		500, Vector3Up());
}

Player::~Player() {
}

void Player::Init(std::weak_ptr<Camera> camera) {
	_camera = camera;
	MV1SetRotationXYZ(_animator->GetModelHandle(), Vector3(0, _rotAngle, 0));
}

void Player::Update() {
	_animator->Update();

#ifndef OLD_STATETRANSITION
	// 状態遷移確認
	CheckStateTransition();
#endif

	// 現在のステートに応じたUpdateが行われる
	(this->*_nowUpdateState)();

	// 武器更新
	WeaponUpdate();
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

void Player::OnCollide(const std::weak_ptr<Collider> collider)
{
	// coliderと衝突

	// 特定のタグではない場合return
	if (collider.lock()->GetTag() != PhysicsData::GameObjectTag::PlayerAttack) return;

	// (ダメージを受け)
	// 被弾状態へ
	if (_nowUpdateState != &Player::UpdateDamage) {
		_nowUpdateState = &Player::UpdateDamage;
		_animator->ChangeAnim(kAnimNameDamage, false);
		// ダメージを受ける


		return;
	}
}

void Player::CheckStateTransition()
{
	Input& input = Input::GetInstance();
	Vector3 stick = input.GetPadLeftSitck();
	
	bool isEndAnim = _animator->IsEnd(_animator->GetCurrentAnimName());

	// 現在のアニメーションが終わっているかつ現在のステートが攻撃関連ならtrue
	bool isEndAttack = (
		isEndAnim &&
		(_nowUpdateState == &Player::UpdateAttackFirst ||
		_nowUpdateState == &Player::UpdateAttackSecond ||
		_nowUpdateState == &Player::UpdateAttackThird)
		);

	// 攻撃中に派生入力があった
	// もしくは現在攻撃関連のステートでないなら
	// 一段目の攻撃状態へ
	if (isEndAttack ||
		(_nowUpdateState != &Player::UpdateAttackFirst &&
			_nowUpdateState != &Player::UpdateAttackSecond &&
			_nowUpdateState != &Player::UpdateAttackThird)) {
		_nowUpdateState = &Player::UpdateAttackFirst;
		_animator->ChangeAnim(kAnimNameAttackCombo1, false);
		_hasDerivedAttackInput = false;
	}
	// アニメーションが終わっているかつ
	// 現在攻撃の一段目なら
	// 二段目の攻撃状態へ
	if (_hasDerivedAttackInput && (isEndAnim &&
		(_nowUpdateState == &Player::UpdateAttackFirst))) {
		_nowUpdateState = &Player::UpdateAttackSecond;
		_animator->ChangeAnim(kAnimNameAttackCombo2, false);
		_hasDerivedAttackInput = false;
	}
	// アニメーションが終わっているかつ
	// 現在攻撃の二段目なら
	// 三段目の攻撃状態へ
	if (_hasDerivedAttackInput && (isEndAnim &&
		(_nowUpdateState == &Player::UpdateAttackSecond))) {
		_nowUpdateState = &Player::UpdateAttackThird;
		_animator->ChangeAnim(kAnimNameAttackCombo3, false);
		_hasDerivedAttackInput = false;
	}



	//// 地上にいて、
	//// ジャンプボタンが押されたら
	//// ジャンプ状態へ
	//if (input.IsTrigger("jump") && GetPos().y <= kGround) {
	//	_nowUpdateState = &Player::UpdateJump;
	//	_animator->ChangeAnim(kAnimNameJump, true); // アニメーションもここで変更
	//	// ジャンプ初速を与える処理などもここに書く
	//	Vector3 vel = GetVel();
	//	vel.y += kJumpForce;
	//	rigidbody->SetVel(vel);
	//	return; // 状態が変わったので、以降の判定はしない
	//}

	// 地上にいるかつ
	// 攻撃が終わっているかつ
	// 一定以上のスティック入力があるなら
	// ダッシュ状態へ
	if (GetPos().y <= kGround && 
		isEndAttack &&
		(stick.Normalize().Magnitude() >= 0.8f) &&
		input.IsPress("dash")) {
		if (_nowUpdateState != &Player::UpdateDash) { // 現在ダッシュでなければ
			_nowUpdateState = &Player::UpdateDash;
			_animator->ChangeAnim(kAnimNameRun, true);
		}
		return;
	}

	// 地上にいるかつ
	// 攻撃が終わっているかつ
	// ジャンプ状態でないかつ
	// スティック入力があれば
	// 歩き状態へ
	if (GetPos().y <= kGround && 
		(stick.Magnitude() != 0.0f)) {
		if (_nowUpdateState != &Player::UpdateWalk) { // 現在歩きでなければ
			_nowUpdateState = &Player::UpdateWalk;
			_animator->ChangeAnim(kAnimNameWalk, true);
		}
		return;
	}

	// 上記のいずれでもなければ待機状態へ
	if (GetPos().y <= kGround) {
		if (_nowUpdateState != &Player::UpdateIdle) {
			_nowUpdateState = &Player::UpdateIdle;
			_animator->ChangeAnim(kAnimNameIdle, true);
		}
		return;
	}
}

void Player::WeaponUpdate()
{
	// 手の行列を武器のワールド行列とする

	// 武器をアタッチするフレームの番号を検索
	int frameIndex = MV1SearchFrame(_animator->GetModelHandle(), L"FrameName");
	// 武器をアタッチするフレームのローカル→ワールド変換行列を取得する
	Matrix4x4 frameMatrix = MV1GetFrameLocalWorldMatrix(_animator->GetModelHandle(), frameIndex);

	// フレームの行列を入れる
	_weapon->Update(frameMatrix);
}

void Player::UpdateIdle()
{
	Input& input = Input::GetInstance();
	Vector3 stick = input.GetPadLeftSitck();

#ifdef OLD_STATETRANSITION
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
#endif // OLD_STATETRANSITION

	Vector3 vel = GetVel();

#ifdef OLD_STATETRANSITION
	// ジャンプ処理
	if (input.IsTrigger("jump") && GetPos().y <= kGround) {
		vel.y += kJumpForce;
		_animator->ChangeAnim(kAnimNameJump, true);
		_nowUpdateState = &Player::UpdateJump;
	}
#endif // OLD_STATETRANSITION

	const float cameraRot = _camera.lock()->GetRotAngleY();

#ifndef USE_QUATERNION
	// カメラから見た移動に変換する
	_rotMtx = MatRotateY(cameraRot);
	vel = VecMultiple(_rotMtx, vel);
#else
	_quaternion = AngleAxis(Vector3Up(), _rotAngle);
	vel = VecMultiple(ConvQuaternionToMatrix4x4(_quaternion), vel);
#endif

	// rigidbodyに編集した移動量を代入
	rigidbody->SetVel(vel);
}

void Player::UpdateWalk()
{
	// 移動処理
	Move(kWalkSpeed);
	// 進行方向への方向転換処理
	Rotate();
}

void Player::UpdateDash()
{
	// 移動処理
	Move(kDashSpeed);
	// 進行方向への方向転換処理
	Rotate();
}

void Player::UpdateJump()
{
#ifdef OLD_STATETRANSITION
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
#endif // OLD_STATETRANSITION

	
	// 移動処理
	Move(kDashSpeed);
	// 進行方向への方向転換処理
	Rotate();
}

void Player::UpdateAttackFirst()
{
	// 攻撃処理

	// 入力受付範囲内で攻撃入力が入れば
	// 攻撃secondを予約する
	Input& input = Input::GetInstance();
	bool isEndAnim = _animator->IsEnd(_animator->GetCurrentAnimName());
	if (!isEndAnim && input.IsTrigger("action")) {
		_hasDerivedAttackInput = true;
	}
}

void Player::UpdateAttackSecond()
{
	// 入力受付範囲内で攻撃入力が入れば
	// 攻撃secondを予約する
	Input& input = Input::GetInstance();
	bool isEndAnim = _animator->IsEnd(_animator->GetCurrentAnimName());
	if (!isEndAnim && input.IsTrigger("action")) {
		_hasDerivedAttackInput = true;
	}
}

void Player::UpdateAttackThird()
{
}

void Player::UpdateDamage()
{
}

void Player::UpdateDead()
{
}

void Player::Move(const float speed)
{
	Vector3 dir = {};
	Vector3 vel = GetVel();
	Position3 pos = GetPos();	// 移動予定位置
	Input& input = Input::GetInstance();
	const float cameraRot = _camera.lock()->GetRotAngleY();

	// スティックによる平面移動
	Vector3 stick = Input::GetInstance().GetPadLeftSitck();
	

#ifdef OLD_STATETRANSITION
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
#endif // OLD_STATETRANSITION


	// 入力が入っていない時でもxに-0.0fが入っている
	dir.x = -stick.x;
	dir.z = stick.z;


#ifdef OLD_STATETRANSITION
	// ジャンプ処理
	if (input.IsTrigger("jump") && GetPos().y <= kGround) {
		vel.y += kJumpForce;
		_animator->ChangeAnim(kAnimNameJump, true);
		_nowUpdateState = &Player::UpdateJump;
	}
#endif // OLD_STATETRANSITION

	
	dir.Normalized();

	// カメラから見た移動方向に変換する
	_quaternion = AngleAxis(Vector3Up(), cameraRot);	// Y軸回転Qを作成
	dir = RotateVector3(_quaternion, dir);				// 回転Qを適用
	//_quaternion = ConvMatrix4x4ToQuaternion(MatRotateY(cameraRot));
	//dir = VecMultiple(ConvQuaternionToMatrix4x4(_quaternion), dir);

	vel.x += dir.x * speed;
	vel.z += dir.z * speed;

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
		const float cameraRot = _camera.lock()->GetRotAngleY();
		float a = atan2f(stick.z, stick.x) + -cameraRot + DX_PI_F * 0.5f;
		MV1SetRotationXYZ(_animator->GetModelHandle(), Vector3(0, a, 0));
	}
#else
	MV1SetRotationXYZ(_modelHandle, Vector3(0, _rotAngle, 0));
#endif
}
