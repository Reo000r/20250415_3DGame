#pragma once
#include "Geometry.h"
#include "Collider.h"
#include <memory>

class Player;
class Animator;

class EnemyBase abstract : public Collider
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="hitPoint">HP</param>
	/// <param name="transferAttackRad">攻撃移行範囲</param>
	/// <param name="attackMul">攻撃力補正値(武器に掛ける値)</param>
	EnemyBase(float hitPoint, float transferAttackRad, float attackMul);
	virtual ~EnemyBase();

	virtual void Init(std::weak_ptr<Player> player) abstract;
	virtual void Update() abstract;
	virtual void Draw() abstract;

	bool IsAlive() { return (_hitPoint > 0.0f); }

	Matrix4x4 GetRotMtx() const { return _rotMtx; }

protected:
	/// <summary>
	/// ステートの遷移条件を確認し、変更可能なステートがあればそれに遷移する
	/// </summary>
	virtual void CheckStateTransition() abstract;

	std::shared_ptr<Animator> _animator;

	std::weak_ptr<Player> _player;

	float _rotAngle;
	Matrix4x4 _rotMtx;
	Quaternion _quaternion;

	// HP
	float _hitPoint;

	// 攻撃移行範囲
	float _transferAttackRad;
	// 攻撃力補正値(武器に掛ける値)
	float _attackMul;
};
