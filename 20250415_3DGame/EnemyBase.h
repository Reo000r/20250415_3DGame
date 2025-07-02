#pragma once
#include "Geometry.h"
#include "Collider.h"
#include <memory>

class Player;
class Animator;

/*

class EnemyBase abstract : public Collider
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	virtual void Init(std::weak_ptr<Player> player) abstract;
	virtual void Update() abstract;
	virtual void Draw() abstract;

	Matrix4x4 GetRotMtx() const { return _rotMtx; }

private:
	using UpdateFunc_t = void(EnemyBase::*)();
	UpdateFunc_t _nowUpdateState;

private:
	void UpdateIdle();
	void UpdateWalk();
	void UpdateDash();
	void UpdateJump();
	void UpdateAttack();

	std::shared_ptr<Animator> _animator;

	std::weak_ptr<Player> _player;

	float _rotAngle;
	Matrix4x4 _rotMtx;
};

*/