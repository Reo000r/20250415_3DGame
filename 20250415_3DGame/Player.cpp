#include "Player.h"
#include "Camera.h"
#include "Animator.h"
#include "WeaponPlayer.h"
#include "Input.h"
#include "Collider.h"
#include "ColliderData.h"
#include "Rigidbody.h"
#include "Calculation.h"
#include <cassert>
#include <algorithm>

#include <DxLib.h>

namespace {
	// 当たり判定のパラメータ
	constexpr float kColRadius = 70.0f; // 半径
	constexpr float kColHeight = 350.0f; // 身長
	// カプセルの始点(足元)から終点(頭頂部)までのベクトル
	const Vector3 kColOffset = Vector3Up() * (kColHeight - kColRadius * 2.0f);
	
	constexpr float kAttackPower = 100.0f;
	constexpr float kWalkSpeed = 7.0f;
	constexpr float kDashSpeed = 14.0f;
	constexpr float kJumpForce = 20.0f;
	constexpr float kGround = 0.0f;
	constexpr int kReactCooltimeFrame = 60;	// 無敵時間

	// 回転速度(ラジアン)
	constexpr float kTurnSpeed = 0.2f;
	
	constexpr float kHitPoint = 100.0f;

	const std::wstring kAnimName = L"Armature|Animation_";
	const std::wstring kAnimNameIdle =			kAnimName + L"Idle";
	const std::wstring kAnimNameWalk =			kAnimName + L"Walk";
	const std::wstring kAnimNameRun =			kAnimName + L"Run";
	const std::wstring kAnimNameAttackNormal =	kAnimName + L"Attack360High";
	const std::wstring kAnimNameAttackBack =	kAnimName + L"AttackBackhand";
	const std::wstring kAnimNameAttackCombo1 =	kAnimName + L"AttackCombo1";
	const std::wstring kAnimNameAttackCombo2 =	kAnimName + L"AttackCombo2";
	const std::wstring kAnimNameAttackCombo3 =	kAnimName + L"AttackCombo3";
	const std::wstring kAnimNameSpecialAttack1 = kAnimName + L"SpecialAttack1";
	const std::wstring kAnimNameSpecialAttack2 = kAnimName + L"SpecialAttack2";
	const std::wstring kAnimNameBlock =			kAnimName + L"Block";
	const std::wstring kAnimNameBlockReact =	kAnimName + L"BlockReact";
	const std::wstring kAnimNameReact =			kAnimName + L"React";
	const std::wstring kAnimNameBuff =			kAnimName + L"Buff";
	const std::wstring kAnimNameDead =			kAnimName + L"Dying";
	const std::wstring kAnimNameAppeal =		kAnimName + L"WinAnim";

	constexpr float kBaseAnimSpeed = 1.0f;

	constexpr float kAttackCombo1InputStart = 0.1f;
	constexpr float kAttackCombo1InputEnd	= 1.0f;
	constexpr float kAttackCombo2InputStart = 0.1f;
	constexpr float kAttackCombo2InputEnd	= 1.0f;


	// 武器データ
	constexpr float kWeaponRad = 50.0f;
	constexpr float kWeaponDist = 500.0f;

	const Vector3 kWeaponOffsetPos = Vector3Up();					// 位置補正
	const Vector3 kWeaponOffsetScale = Vector3(1.0f, 1.3f, 2.0f);	// 拡縮補正
	// 角度補正
	const Vector3 kWeaponOffsetDir = Vector3(
		Calc::ToRadian(60.0f),
		Calc::ToRadian(90.0f),
		Calc::ToRadian(50.0f));
}

Player::Player() :
	Collider(PhysicsData::Priority::Middle,
		PhysicsData::GameObjectTag::Player,
		PhysicsData::ColliderKind::Capsule,
		false, true),
	_nowUpdateState(&Player::UpdateIdle),
	_animator(std::make_unique<Animator>()),
	_weapon(std::make_unique<WeaponPlayer>()),
	_rotAngle(0.0f),
	_hasDerivedAttackInput(false),
	_hitPoint(kHitPoint),
	_attackPower(kAttackPower),
	_reactCooltime(0)
{
	rigidbody->Init(true);

	// 自身の武器とは当たり判定を行わない
	colliderData->AddThroughTag(PhysicsData::GameObjectTag::PlayerAttack);

	// モデルの読み込み
	_animator->Init(MV1LoadModel(L"data/model/character/Player.mv1"));
	MV1SetScale(_animator->GetModelHandle(), Vector3(1, 1, 1) * 2.0f);

	// 使用するアニメーションを全て入れる
	_animator->SetAnimData(kAnimNameIdle,			true,  kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameWalk,			true,  kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameRun,			true,  kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameAttackNormal,	false, kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameAttackBack,		false, kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameAttackCombo1,	false, kBaseAnimSpeed, kAttackCombo1InputStart, kAttackCombo1InputEnd);
	_animator->SetAnimData(kAnimNameAttackCombo2,	false, kBaseAnimSpeed, kAttackCombo2InputStart, kAttackCombo2InputEnd);
	_animator->SetAnimData(kAnimNameAttackCombo3,	false, kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameSpecialAttack1, false, kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameSpecialAttack2, false, kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameBlock,			true,  kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameBlockReact,		false, kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameReact,			false, kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameBuff,			false, kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameDead,			false, kBaseAnimSpeed);
	_animator->SetAnimData(kAnimNameAppeal,			false, kBaseAnimSpeed);
	// 最初のアニメーションを設定する
	_animator->SetStartAnim(kAnimNameIdle);


	// 当たり判定データ設定
	SetColliderData(
		PhysicsData::ColliderKind::Capsule,	// 種別
		false,								// isTrigger
		true,								// isCollision
		kColRadius,							// 半径
		kColOffset							// 始点から終点
	);


	// 武器初期化
	int weaponModelHandle = MV1LoadModel(L"data/model/weapon/PlayerWeapon.mv1");
	assert(weaponModelHandle >= 0 && "モデルハンドルが正しくない");
	_weapon->Init(
		weaponModelHandle,
		kWeaponRad,			// 当たり判定半径
		kWeaponDist,		// 当たり判定長さ
		kWeaponOffsetPos,	// 位置補正
		kWeaponOffsetScale,	// 拡縮補正
		kWeaponOffsetDir	// 角度補正
	);

	// 武器に自分自身と自分の攻撃力を設定
	auto thisptr = Collider::shared_from_this();
	_weapon->SetOwnerStatus(thisptr, _attackPower);
}

Player::~Player()
{
	// modelはanimator側で消している

	// 死んでいなかったら物理判定から除外する
	if (!_animator->IsEnd(kAnimNameDead)) {
		ReleasePhysics();
	}
}

void Player::Init(std::weak_ptr<Camera> camera)
{
	_camera = camera;
	MV1SetRotationXYZ(_animator->GetModelHandle(), Vector3(0, _rotAngle, 0));
}

void Player::Update()
{
	_animator->Update();

	// 死んでいるならアニメーション以外の更新を停止
	if (!IsAlive()) return;

	// 状態遷移確認
	CheckStateTransition();

	// 現在のステートに応じたUpdateが行われる
	(this->*_nowUpdateState)();

	// 無敵時間更新
	if (_reactCooltime > 0) _reactCooltime--;

	// 武器更新
	WeaponUpdate();
}

void Player::Draw()
{
	// 当たり判定を行ってからモデルの位置を設定する
	MV1SetPosition(_animator->GetModelHandle(), GetPos());
	// モデルの描画
	MV1DrawModel(_animator->GetModelHandle());

	_weapon->Draw();

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
}

void Player::TakeDamage(float damage, std::shared_ptr<Collider> attacker)
{
	// 死亡状態、または攻撃中はダメージを受け付けない
	if (_nowUpdateState == &Player::UpdateDeath ||
		_nowUpdateState == &Player::UpdateAttackFirst ||
		_nowUpdateState == &Player::UpdateAttackSecond ||
		_nowUpdateState == &Player::UpdateAttackThird) {
		return;
	}

	// 無敵時間外であれば
	if (_reactCooltime <= 0) {
		// HPを減らす
		_hitPoint -= damage;
		_reactCooltime = kReactCooltimeFrame;

		// 武器の当たり判定を無効化
		_weapon->SetCollisionState(false);

		// hpが0以下の場合は死亡
		if (_hitPoint <= 0.0f) {
			if (_nowUpdateState != &Player::UpdateDeath) {
				_nowUpdateState = &Player::UpdateDeath;
				_animator->ChangeAnim(kAnimNameDead, false);
			}
			return;
		}

		// 既に被弾状態ならアニメーションを最初から再生
		if (_nowUpdateState == &Player::UpdateDamage) {
			auto& animData = _animator->FindAnimData(kAnimNameReact);
			animData.frame = 0.0f;
			animData.isEnd = false;
		}
		// そうでなければ被弾状態へ遷移
		else {
			_nowUpdateState = &Player::UpdateDamage;
			_animator->ChangeAnim(kAnimNameReact, false);
			_hasDerivedAttackInput = false;		// 攻撃コンボをリセット
		}
	}
}

void Player::CheckStateTransition()
{
	// 死亡判定を最優先
	// hpが0以下の場合は死亡
	if (_hitPoint <= 0.0f) {
		if (_nowUpdateState != &Player::UpdateDeath) {
			_nowUpdateState = &Player::UpdateDeath;
			_animator->ChangeAnim(kAnimNameDead, false);
			// 武器の当たり判定を無効化
			_weapon->SetCollisionState(false);
		}
		return;
	}

	// 割り込み不可の状態判定
	if (_nowUpdateState == &Player::UpdateDeath) {
		return; // 死亡中は他の状態に遷移しない
	}
	if (_nowUpdateState == &Player::UpdateDamage) {
		// 被弾アニメーションが終了していなければ、他の状態に遷移しない
		if (!_animator->IsEnd(kAnimNameReact)) {
			return;
		}
	}

	Input& input = Input::GetInstance();
	Vector3 stick = input.GetPadLeftSitck();

	// 現在再生中のアニメーションが終了しているか
	bool isEndAnim = _animator->IsEnd(_animator->GetCurrentAnimName());

	// 攻撃中かどうか
	bool isAttack =
		(_nowUpdateState == &Player::UpdateAttackFirst ||
		_nowUpdateState == &Player::UpdateAttackSecond ||
		_nowUpdateState == &Player::UpdateAttackThird);

	if (isAttack) {
		// 攻撃アニメーションが終了した場合にのみ次の遷移を判断する
		if (isEndAnim) {
			// 派生入力があったか
			if (_hasDerivedAttackInput) {
				// どの攻撃からの派生か
				if (_nowUpdateState == &Player::UpdateAttackFirst) {
					_nowUpdateState = &Player::UpdateAttackSecond;
					_animator->ChangeAnim(kAnimNameAttackCombo2, false);
					_hasDerivedAttackInput = false;
					// 武器の当たり判定を有効化
					_weapon->SetCollisionState(true);
					return; // 遷移したので処理終了
				}
				else if (_nowUpdateState == &Player::UpdateAttackSecond) {
					_nowUpdateState = &Player::UpdateAttackThird;
					_animator->ChangeAnim(kAnimNameAttackCombo3, false);
					_hasDerivedAttackInput = false;
					// 武器の当たり判定を有効化
					_weapon->SetCollisionState(true);
					return; // 遷移したので処理終了
				}
			}

			// 派生入力がなければ
			// 武器の当たり判定を無効化
			_weapon->SetCollisionState(false);
			// スティックが一定以上傾いているなら
			if (stick.Magnitude() >= 1000 * 0.8f) {
				_nowUpdateState = &Player::UpdateDash;
				_animator->ChangeAnim(kAnimNameRun, true);
			}
			else if (stick.Magnitude() != 0.0f) {
				_nowUpdateState = &Player::UpdateWalk;
				_animator->ChangeAnim(kAnimNameWalk, true);
			}
			else {
				_nowUpdateState = &Player::UpdateIdle;
				_animator->ChangeAnim(kAnimNameIdle, true);
			}
			return;
		}

		// 攻撃中は何もしない
		return;
	}



	// 攻撃開始
	// 攻撃入力があったなら
	if (input.IsTrigger("action")) {
		_nowUpdateState = &Player::UpdateAttackFirst;
		_animator->ChangeAnim(kAnimNameAttackCombo1, false);
		_hasDerivedAttackInput = false;
		// 武器の当たり判定を有効化
		_weapon->SetCollisionState(true);
		return;
	}

	// ダッシュ
	// スティックが一定以上傾いているなら
	if (GetPos().y <= kGround &&
		(stick.Magnitude() >= 1000 * 0.8f)) {
		if (_nowUpdateState != &Player::UpdateDash) {
			_nowUpdateState = &Player::UpdateDash;
			_animator->ChangeAnim(kAnimNameRun, true);
			// 武器の当たり判定を無効化
			_weapon->SetCollisionState(false);
		}
		return;
	}

	// 歩行
	// スティックの入力があるなら
	else if (GetPos().y <= kGround &&
		(stick.Magnitude() != 0.0f)) {
		if (_nowUpdateState != &Player::UpdateWalk) {
			_nowUpdateState = &Player::UpdateWalk;
			_animator->ChangeAnim(kAnimNameWalk, true);
			// 武器の当たり判定を無効化
			_weapon->SetCollisionState(false);
		}
		return;
	}

	// 待機
	// それ以外なら
	else if (GetPos().y <= kGround) {
		if (_nowUpdateState != &Player::UpdateIdle) {
			_nowUpdateState = &Player::UpdateIdle;
			_animator->ChangeAnim(kAnimNameIdle, true);
			// 武器の当たり判定を無効化
			_weapon->SetCollisionState(false);
		}
		return;
	}
}

void Player::WeaponUpdate()
{
	// 手の行列を武器のワールド行列とする
	std::wstring handName = L"mixamorig:RightHandThumb3";
	// 武器をアタッチするフレームの番号を検索
	int frameIndex = MV1SearchFrame(_animator->GetModelHandle(), handName.c_str());
	// インデックスが有効かチェック
	if (frameIndex < 0) {
		assert(false && "指定されたフレームが見つからなかった");
		return;
	}
	// 手のフレームのワールド行列を取得
	Matrix4x4 handWorldMatrix = MV1GetFrameLocalWorldMatrix(
		_animator->GetModelHandle(),frameIndex);

	// 手のワールド行列を渡す
	_weapon->Update(handWorldMatrix);
}

void Player::UpdateIdle()
{
	// 移動を停止する
	rigidbody->SetVel(Vector3());
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

void Player::UpdateAttackFirst()
{
	// アニメーションの現在のフレームを取得
	float currentFrame = _animator->GetCurrentAnimFrame();
	// 現在のアニメーションデータから入力受付期間を取得
	const Animator::AnimData& currentAnimData = _animator->FindAnimData(_animator->
		GetCurrentAnimName());
	
	// 入力受付期間内かつ
	// 再生された瞬間ではないかつ
	// 攻撃ボタンが押されたら
	// 次の攻撃へ派生可能にする
	if (currentAnimData.animName == kAnimNameAttackCombo1 &&
		currentAnimData.frame != 0.0f &&
		currentFrame >= currentAnimData.inputAcceptanceStartFrame &&
		currentFrame <= currentAnimData.inputAcceptanceEndFrame &&
		Input::GetInstance().IsTrigger("action"))
	{
		_hasDerivedAttackInput = true;
	}
}

void Player::UpdateAttackSecond()
{
	// アニメーションの現在のフレーム
	float currentFrame = _animator->GetCurrentAnimFrame();
	// 現在のアニメーションデータ(入力受付期間取得用)
	const Animator::AnimData& currentAnimData = 
		_animator->FindAnimData(_animator->GetCurrentAnimName());

	// 入力受付期間内かつ、攻撃ボタンが押されたら次の攻撃へ派生可能にする
	if (currentAnimData.animName == kAnimNameAttackCombo2 &&
		currentFrame >= currentAnimData.inputAcceptanceStartFrame &&
		currentFrame <= currentAnimData.inputAcceptanceEndFrame &&
		Input::GetInstance().IsTrigger("action"))
	{
		_hasDerivedAttackInput = true;
	}
}

void Player::UpdateAttackThird()
{
	// 派生は不要
}

void Player::UpdateDamage()
{
	// 移動を停止する
	rigidbody->SetVel(Vector3());
}

void Player::UpdateDeath()
{
	// 死亡アニメーションが終了したら更新を止める
	if (_animator->IsEnd(kAnimNameDead)) {
		// 物理判定から除外する
		ReleasePhysics();
	}
}

void Player::Move(const float speed)
{
	Vector3 dir = {};
	Vector3 vel = GetVel();
	Position3 pos = GetPos();	// 移動予定位置
	Input& input = Input::GetInstance();
	const float cameraRot = _camera.lock()->GetRotAngleY() * -1;

	// スティックによる平面移動
	Vector3 stick = Input::GetInstance().GetPadLeftSitck();

	// 入力が入っていない時でもxに-0.0fが入っている
	dir.x = -stick.x;
	dir.z = stick.z;

	dir.Normalized();

	// カメラから見た移動方向に変換する
	_quaternion = AngleAxis(Vector3Up(), cameraRot);	// Y軸回転Qを作成
	dir = RotateVector3(_quaternion, dir);				// 回転Qを適用

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
	// スティックによる平面移動
	Vector3 stick = Input::GetInstance().GetPadLeftSitck();
	// 入力があった場合のみキャラクターの向きを変更
	if (stick.x != 0.0f || stick.z != 0.0f) {
		// カメラの向きを考慮しつつ目標の角度を計算
		const float cameraRot = _camera.lock()->GetRotAngleY();
		float targetAngle = atan2f(stick.z, stick.x) + -cameraRot + DX_PI_F * 0.5f;

		// 現在の角度から目標角度までの最短差分を計算
		float diff = targetAngle - _rotAngle;

		// 角度の正規化
		Calc::RadianNormalize(diff);
		// 1フレームで回転できる最大量に制限する
		float turnAmount = std::clamp(diff, -kTurnSpeed, kTurnSpeed);

		// 更新
		_rotAngle += turnAmount;

		// 現在の角度も正規化しておく
		Calc::RadianNormalize(_rotAngle);
		// 適用
		MV1SetRotationXYZ(_animator->GetModelHandle(), Vector3(0, _rotAngle, 0));
	}
}
