#include "EnemyNormal.h"
#include "Player.h"
#include "Camera.h"
#include "Animator.h"
#include "Input.h"
#include "Collider.h"
#include "Rigidbody.h"
#include <cassert>

#include <DxLib.h>

namespace {
	constexpr float kWalkSpeed = 5.0f;
	constexpr float kGround = 0.0f;

	const std::wstring kAnimName = L"CharacterArmature|";
	const std::wstring kAnimNameIdle = kAnimName + L"Idle";
	const std::wstring kAnimNameWalk = kAnimName + L"Walk";
	const std::wstring kAnimNameAttack = kAnimName + L"Attack";
	const std::wstring kAnimNameDamage = kAnimName + L"Damage";
}

EnemyNormal::EnemyNormal() : 
	EnemyBase(10.0f, 100.0f, 1.0f),
	_nowUpdateState(&EnemyNormal::UpdateIdle)

{
	rigidbody->Init(true);

	// モデルの読み込み
	_animator->Init(MV1LoadModel(L"data/model/EnemyNormal_test.mv1"));

	// 使用するアニメーションを全て入れる
	_animator->SetAnimData(kAnimNameIdle, true);
	_animator->SetAnimData(kAnimNameWalk, true);
	//_animator->SetAnimData(kAnimNameAttack, true);	// 保留
	//_animator->SetAnimData(kAnimNameDamage, true);
	// 最初のアニメーションを設定する
	_animator->SetStartAnim(kAnimNameIdle);
}

EnemyNormal::~EnemyNormal()
{
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
	
	// 特定のタグではない場合return
	if (collider.lock()->GetTag() != PhysicsData::GameObjectTag::PlayerAttack) return;

	// (ダメージを受け)
	// 被弾状態へ
	if (_nowUpdateState != &EnemyNormal::UpdateDamage) {
		_nowUpdateState = &EnemyNormal::UpdateDamage;
		_animator->ChangeAnim(kAnimNameDamage, false);
		// ダメージを受ける

		return;
	}
}

void EnemyNormal::CheckStateTransition()
{
	// 自身からプレイヤーまでの距離
	float enemyToPlayerDist = (_player.lock()->GetPos() - GetPos()).Magnitude();

	// HPがないなら
	// 死亡状態へ
	if (_nowUpdateState != &EnemyNormal::UpdateDead &&
		_hitPoint <= 0.0f) {
		if (_nowUpdateState != &EnemyNormal::UpdateDead) { // 現在死亡でなければ
			_nowUpdateState = &EnemyNormal::UpdateDead;
			_animator->ChangeAnim(kAnimNameAttack, false);
		}
		return;
	}

	// 被弾していないかつ
	// プレイヤーとの距離が一定以下なら
	// 攻撃状態へ
	if (_nowUpdateState != &EnemyNormal::UpdateDamage &&
		enemyToPlayerDist <= _transferAttackRad) {
		if (_nowUpdateState != &EnemyNormal::UpdateAttack) { // 現在攻撃でなければ
			_nowUpdateState = &EnemyNormal::UpdateAttack;
			_animator->ChangeAnim(kAnimNameAttack, false);
		}
		return;
	}

	// (誰かがプレイヤーと戦闘中なら)
	// 待機状態へ
	if (_nowUpdateState != &EnemyNormal::UpdateDamage) {
		if (_nowUpdateState != &EnemyNormal::UpdateIdle) {
			_nowUpdateState = &EnemyNormal::UpdateIdle;
			_animator->ChangeAnim(kAnimNameIdle, true);
		}
		return;
	}

	// 上記のいずれでもなければ近づく状態へ
	if (_nowUpdateState != &EnemyNormal::UpdateDamage) {
		if (_nowUpdateState != &EnemyNormal::UpdateWalk) { // 現在近づきでなければ
			_nowUpdateState = &EnemyNormal::UpdateWalk;
			_animator->ChangeAnim(kAnimNameWalk, true);
		}
		return;
	}
}

void EnemyNormal::UpdateIdle()
{
	// 処理なし
}

void EnemyNormal::UpdateWalk()
{
	// プレイヤーに向かって移動する
	
	// 自身からプレイヤーへの方向ベクトル
	Vector3 dir = (_player.lock()->GetPos() - GetPos()).Normalize();

	Vector3 vel = dir * kWalkSpeed;

	// rigidbodyに編集した移動量を代入
	rigidbody->SetVel(vel);
}

void EnemyNormal::UpdateAttack()
{
	// 攻撃処理

	// 
}

void EnemyNormal::UpdateDamage()
{
	// 被弾処理

	// 当たった瞬間に致死オブジェクト(もしくはplayer)と反対方向にノックバックさせたい
}

void EnemyNormal::UpdateDead()
{
	// 死んだ瞬間に致死オブジェクト(もしくはplayer)と反対方向に吹き飛びたい

	// アニメーションが終わったら消滅
}
