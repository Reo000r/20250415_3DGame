#include "EnemyNormal.h"
#include "Player.h"
#include "Camera.h"
#include "Animator.h"
#include "Input.h"
#include "Collider.h"
#include "Rigidbody.h"
#include <cassert>

#include <DxLib.h>
#include <algorithm>

namespace {
	constexpr float kHitPoint = 3.0f;		// HP
	constexpr float kTempDamage = 1.0f;		// 仮の被弾時ダメージ
	constexpr float kChaseSpeed = 5.0f;		// 追いかける速度
	constexpr float kTurnSpeed = 0.05f;		// 回転速度(ラジアン)
	constexpr float kAttackRange = 150.0f;	// 攻撃に移行する距離
	constexpr float kGround = 0.0f;			// (地面の高さ)

	const std::wstring kAnimName = L"Armature|Animation_";
	const std::wstring kAnimNameChase = kAnimName + L"Idle";
	const std::wstring kAnimNameAttack = kAnimName + L"Attack";
	const std::wstring kAnimNameDamage = kAnimName + L"Damage";
	const std::wstring kAnimNameDeath = kAnimName + L"Death";
}

EnemyNormal::EnemyNormal() : 
	EnemyBase(kHitPoint, kAttackRange, 1.0f),
	_nowUpdateState(&EnemyNormal::UpdateChase)

{
	rigidbody->Init(true);

	// モデルの読み込み
	_animator->Init(MV1LoadModel(L"data/model/EnemyNormal_test.mv1"));

	// 使用するアニメーションを全て入れる
	_animator->SetAnimData(kAnimNameChase, true);
	_animator->SetAnimData(kAnimNameAttack, false);
	_animator->SetAnimData(kAnimNameDamage, false);
	_animator->SetAnimData(kAnimNameDeath, false);
	// 最初のアニメーションを設定する
	_animator->SetStartAnim(kAnimNameChase);
}

EnemyNormal::~EnemyNormal()
{
	// modelはanimator側で消している
}

void EnemyNormal::Init(std::weak_ptr<Player> player)
{
	_player = player;
	MV1SetRotationXYZ(_animator->GetModelHandle(), Vector3(0, _rotAngle, 0));
}

void EnemyNormal::Update()
{
	_animator->Update();

	// 状態遷移確認
	CheckStateTransition();

	// 現在のステートに応じたUpdateが行われる
	(this->*_nowUpdateState)();
}

void EnemyNormal::Draw()
{
	// 当たり判定を行ってからモデルの位置を設定する
	MV1SetPosition(_animator->GetModelHandle(), GetPos());
	// モデルの描画
	MV1DrawModel(_animator->GetModelHandle());

#ifdef _DEBUG
	int color = 0xffffff;
	int y = 16 * 6;
#endif
}

void EnemyNormal::OnCollide(const std::weak_ptr<Collider> collider)
{
	// coliderと衝突
	
	// 死亡状態ではダメージを受けない
	if (_nowUpdateState == &EnemyNormal::UpdateDeath) return;
	
	// プレイヤーの攻撃であれば
	if (collider.lock()->GetTag() == PhysicsData::GameObjectTag::PlayerAttack) {
		// (固定ダメージ)
		_hitPoint -= kTempDamage;
		// 既に被弾状態ならアニメーションを最初から再生
		if (_nowUpdateState == &EnemyNormal::UpdateDamage) {
			auto& animData = _animator->FindAnimData(kAnimNameDamage);
			animData.frame = 0.0f;
			animData.isEnd = false;
		}
		// そうでなければ被弾状態へ遷移
		else {
			_nowUpdateState = &EnemyNormal::UpdateDamage;
			_animator->ChangeAnim(kAnimNameDamage, false);
		}
	}
}

void EnemyNormal::CheckStateTransition()
{
	// 死亡判定(最優先)
	if (_hitPoint <= 0.0f) {
		if (_nowUpdateState != &EnemyNormal::UpdateDeath) {
			_nowUpdateState = &EnemyNormal::UpdateDeath;
			_animator->ChangeAnim(kAnimNameDeath, false);
		}
		return;
	}

	// 割り込み不可の状態判定
	// 死亡または被弾アニメーション中は他の状態に遷移しない
	if (_nowUpdateState == &EnemyNormal::UpdateDeath) {
		return;
	}
	if (_nowUpdateState == &EnemyNormal::UpdateDamage) {
		// 被弾アニメーションが終了していない場合はこのまま
		if (!_animator->IsEnd(kAnimNameDamage)) {
			return;
		}
	}

	// プレイヤー情報の確認
	if (_player.expired()) {
		// もしプレイヤーがいない場合、追跡状態に戻る
		if (_nowUpdateState != &EnemyNormal::UpdateChase) {
			_nowUpdateState = &EnemyNormal::UpdateChase;
			_animator->ChangeAnim(kAnimNameChase, true);
		}
		return;
	}

	// プレイヤーとの距離
	float distance = (_player.lock()->GetPos() - GetPos()).Magnitude();

	// 攻撃状態
	// プレイヤーとの距離が攻撃移行範囲よりも近かったら
	if (distance <= _transferAttackRad) {
		if (_nowUpdateState != &EnemyNormal::UpdateAttack) {
			_nowUpdateState = &EnemyNormal::UpdateAttack;
			_animator->ChangeAnim(kAnimNameAttack, false);
		}
		return; // 攻撃状態に決定
	}

	// 追跡状態
	// 上記のいずれでもなければ
	if (_nowUpdateState != &EnemyNormal::UpdateChase) {
		_nowUpdateState = &EnemyNormal::UpdateChase;
		_animator->ChangeAnim(kAnimNameChase, true);
	}
}

void EnemyNormal::UpdateChase()
{
	// プレイヤーの方向を向く
	RotateToPlayer();

	// 前方に移動
	Vector3 vel = rigidbody->GetDir() * kChaseSpeed;
	rigidbody->SetVel(vel);
}

void EnemyNormal::UpdateAttack()
{
	// 攻撃中は移動を止める
	rigidbody->SetVel(Vector3());
}

void EnemyNormal::UpdateDamage()
{
	// 移動を停止する
	rigidbody->SetVel(Vector3());
}

void EnemyNormal::UpdateDeath()
{
	// 死亡アニメーションが終了したら、更新を止める
	if (_animator->IsEnd(kAnimNameDeath)) {
		// 物理判定から除外する
		ReleasePhysics();
	}
}

void EnemyNormal::RotateToPlayer()
{
	if (_player.expired()) return;

	// プレイヤーへの方向ベクトル
	Vector3 dirToPlayer = (_player.lock()->GetPos() - GetPos());
	if (dirToPlayer.SqrMagnitude() == 0.0f) return; // 距離がゼロなら何もしない
	dirToPlayer.Normalized();

	// Y軸回転角度を計算
	float targetAngle = atan2f(dirToPlayer.x, dirToPlayer.z);

	// 現在の角度から目標角度までの差分を計算
	float diff = targetAngle - _rotAngle;
	// 角度が-180度以下,180度以上にならないように正規化
	while (diff > DX_PI_F)  diff -= DX_PI_F * 2.0f;
	while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

	// 回転量を制限
	float turnAmount = std::clamp<float>(diff, -kTurnSpeed, kTurnSpeed);
	_rotAngle += turnAmount;

	// モデルに回転を適用
	MV1SetRotationXYZ(_animator->GetModelHandle(), Vector3(0, _rotAngle, 0));

	// Rigidbodyの向きも更新
	Vector3 newDir = Vector3(sinf(_rotAngle), 0.0f, cosf(_rotAngle)).Normalize();
	// 速度は維持しない
	// この時点では向き情報が入っている
	rigidbody->SetVel(newDir);
}
