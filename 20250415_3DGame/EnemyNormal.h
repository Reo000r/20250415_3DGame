﻿#pragma once
#include "EnemyBase.h"

/// <summary>
/// 無難な行動を行う敵
/// </summary>
class EnemyNormal final : public EnemyBase
{
public:
	EnemyNormal();
	~EnemyNormal();

	void Init(std::weak_ptr<Player> player) override;
	void Update() override;
	void Draw() override;

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

private:
	/// <summary>
	/// ステートの遷移条件を確認し、変更可能なステートがあればそれに遷移する
	/// </summary>
	void CheckStateTransition() override;

	using UpdateFunc_t = void(EnemyNormal::*)();
	UpdateFunc_t _nowUpdateState;

private:

	/// <summary>
	/// 誰かがプレイヤーを殴っていたりする場合
	/// 少し離れて様子をうかがう
	/// </summary>
	void UpdateIdle();
	/// <summary>
	/// プレイヤーを補足して近づく
	/// </summary>
	void UpdateWalk();
	/// <summary>
	/// 攻撃
	/// </summary>
	void UpdateAttack();
	/// <summary>
	/// 被弾
	/// </summary>
	void UpdateDamage();
	/// <summary>
	/// 死亡
	/// </summary>
	void UpdateDead();
};
