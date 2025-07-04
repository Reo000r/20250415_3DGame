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
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="hitPoint">HP</param>
	/// <param name="transferAttackRad">�U���ڍs�͈�</param>
	/// <param name="attackMul">�U���͕␳�l(����Ɋ|����l)</param>
	EnemyBase(float hitPoint, float transferAttackRad, float attackMul);
	virtual ~EnemyBase();

	virtual void Init(std::weak_ptr<Player> player) abstract;
	virtual void Update() abstract;
	virtual void Draw() abstract;

	Matrix4x4 GetRotMtx() const { return _rotMtx; }

protected:
	/// <summary>
	/// �X�e�[�g�̑J�ڏ������m�F���A�ύX�\�ȃX�e�[�g������΂���ɑJ�ڂ���
	/// </summary>
	virtual void CheckStateTransition() abstract;

	std::shared_ptr<Animator> _animator;

	std::weak_ptr<Player> _player;

	float _rotAngle;
	Matrix4x4 _rotMtx;
	Quaternion _quaternion;

	// HP
	float _hitPoint;

	// �U���ڍs�͈�
	float _transferAttackRad;
	// �U���͕␳�l(����Ɋ|����l)
	float _attackMul;
};
