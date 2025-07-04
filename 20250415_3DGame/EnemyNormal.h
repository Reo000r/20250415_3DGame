#pragma once
#include "EnemyBase.h"

/// <summary>
/// ����ȍs�����s���G
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
	/// �Փ˂����Ƃ��ɌĂ΂��
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

private:
	/// <summary>
	/// �X�e�[�g�̑J�ڏ������m�F���A�ύX�\�ȃX�e�[�g������΂���ɑJ�ڂ���
	/// </summary>
	void CheckStateTransition() override;

	using UpdateFunc_t = void(EnemyNormal::*)();
	UpdateFunc_t _nowUpdateState;

private:

	/// <summary>
	/// �N�����v���C���[�������Ă����肷��ꍇ
	/// ��������ėl�q����������
	/// </summary>
	void UpdateIdle();
	/// <summary>
	/// �v���C���[��⑫���ċ߂Â�
	/// </summary>
	void UpdateWalk();
	/// <summary>
	/// �U��
	/// </summary>
	void UpdateAttack();
	/// <summary>
	/// ��e
	/// </summary>
	void UpdateDamage();
	/// <summary>
	/// ���S
	/// </summary>
	void UpdateDead();
};
