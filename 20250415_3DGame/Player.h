#pragma once
#include "Geometry.h"
#include "Collider.h"
#include <memory>

class Camera;
class Animator;
class Weapon;

/// <summary>
/// 
/// </summary>
class Player final : public Collider, public std::enable_shared_from_this<Player> {
public:
	Player();
	~Player();

	void Init(/*int modelHandle, */std::weak_ptr<Camera> camera);
	void Update();
	void Draw();

	/// <summary>
	/// �Փ˂����Ƃ��ɌĂ΂��
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

	float GetHitPoint() { return _hitpoint; }
	
private:
	// Update��State�p�^�[��
	// _nowUpdateState���ϐ��ł��邱�Ƃ𕪂���₷�����Ă���
	using UpdateFunc_t = void(Player::*)();
	UpdateFunc_t _nowUpdateState;

private:
	/// <summary>
	/// �X�e�[�g�̑J�ڏ������m�F���A�ύX�\�ȃX�e�[�g������΂���ɑJ�ڂ���
	/// </summary>
	void CheckStateTransition();

	/// <summary>
	/// ����X�V
	/// </summary>
	void WeaponUpdate();

	void UpdateIdle();
	void UpdateWalk();
	void UpdateDash();
	void UpdateJump();
	void UpdateAttackFirst();
	void UpdateAttackSecond();
	void UpdateAttackThird();
	void UpdateDamage();
	void UpdateDead();

	/// <summary>
	/// �ړ�����
	/// </summary>
	void Move(const float speed);
	/// <summary>
	/// �i�s�����ւ̕����]������
	/// </summary>
	void Rotate();

	std::shared_ptr<Animator> _animator;

	std::weak_ptr<Camera> _camera;

	std::shared_ptr<Weapon> _weapon;

	float _rotAngle;
	//Matrix4x4 _rotMtx;
	Quaternion _quaternion;

	// �U���̔h�����͂����������ǂ���
	bool  _hasDerivedAttackInput;

	float _hitpoint;
};

