#pragma once
#include "Geometry.h"
#include "Collider.h"
#include <memory>

class Camera;
class Animator;

class Player final : public Collider {
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
	void OnCollide(const std::weak_ptr<Collider> colider) override;

	Matrix4x4 GetRotMtx() const { return _rotMtx; }

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

	void UpdateIdle();
	void UpdateWalk();
	void UpdateDash();
	void UpdateJump();
	void UpdateAttack();

	/// <summary>
	/// �ړ��֘A����
	/// </summary>
	void Move(const float speed);
	/// <summary>
	/// �ړ�����
	/// </summary>
	void Walk(const float speed);
	/// <summary>
	/// �i�s�����ւ̕����]������
	/// </summary>
	void Rotate();

	std::shared_ptr<Animator> _animator;

	std::weak_ptr<Camera> _camera;

	float _rotAngle;
	Matrix4x4 _rotMtx;
};

