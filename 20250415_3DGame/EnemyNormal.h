#pragma once
#include "EnemyBase.h"

/*

class EnemyNormal : public EnemyBase
{
public:
	EnemyNormal();
	~EnemyNormal();

	void Init(std::weak_ptr<Player> player) override;
	void Update() override;
	void Draw() override;

	/// <summary>
	/// è’ìÀÇµÇΩÇ∆Ç´Ç…åƒÇŒÇÍÇÈ
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> colider) override;

	Matrix4x4 GetRotMtx() const { return _rotMtx; }

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