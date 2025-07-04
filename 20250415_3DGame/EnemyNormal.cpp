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

	// ���f���̓ǂݍ���
	_animator->Init(MV1LoadModel(L"data/model/EnemyNormal_test.mv1"));

	// �g�p����A�j���[�V������S�ē����
	_animator->SetAnimData(kAnimNameIdle, true);
	_animator->SetAnimData(kAnimNameWalk, true);
	//_animator->SetAnimData(kAnimNameAttack, true);	// �ۗ�
	//_animator->SetAnimData(kAnimNameDamage, true);
	// �ŏ��̃A�j���[�V������ݒ肷��
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

	// ��ԑJ�ڊm�F
	CheckStateTransition();

	// ���݂̃X�e�[�g�ɉ�����Update���s����
	(this->*_nowUpdateState)();
}

void EnemyNormal::Draw()
{
	// �����蔻����s���Ă��烂�f���̈ʒu��ݒ肷��
	MV1SetPosition(_animator->GetModelHandle(), GetPos());
	// ���f���̕`��
	MV1DrawModel(_animator->GetModelHandle());

#ifdef _DEBUG
	int color = 0xffffff;
	int y = 16 * 6;
#endif
}

void EnemyNormal::OnCollide(const std::weak_ptr<Collider> collider)
{
	// colider�ƏՓ�
	
	// ����̃^�O�ł͂Ȃ��ꍇreturn
	if (collider.lock()->GetTag() != PhysicsData::GameObjectTag::PlayerAttack) return;

	// (�_���[�W����)
	// ��e��Ԃ�
	if (_nowUpdateState != &EnemyNormal::UpdateDamage) {
		_nowUpdateState = &EnemyNormal::UpdateDamage;
		_animator->ChangeAnim(kAnimNameDamage, false);
		// �_���[�W���󂯂�

		return;
	}
}

void EnemyNormal::CheckStateTransition()
{
	// ���g����v���C���[�܂ł̋���
	float enemyToPlayerDist = (_player.lock()->GetPos() - GetPos()).Magnitude();

	// HP���Ȃ��Ȃ�
	// ���S��Ԃ�
	if (_nowUpdateState != &EnemyNormal::UpdateDead &&
		_hitPoint <= 0.0f) {
		if (_nowUpdateState != &EnemyNormal::UpdateDead) { // ���ݎ��S�łȂ����
			_nowUpdateState = &EnemyNormal::UpdateDead;
			_animator->ChangeAnim(kAnimNameAttack, false);
		}
		return;
	}

	// ��e���Ă��Ȃ�����
	// �v���C���[�Ƃ̋��������ȉ��Ȃ�
	// �U����Ԃ�
	if (_nowUpdateState != &EnemyNormal::UpdateDamage &&
		enemyToPlayerDist <= _transferAttackRad) {
		if (_nowUpdateState != &EnemyNormal::UpdateAttack) { // ���ݍU���łȂ����
			_nowUpdateState = &EnemyNormal::UpdateAttack;
			_animator->ChangeAnim(kAnimNameAttack, false);
		}
		return;
	}

	// (�N�����v���C���[�Ɛ퓬���Ȃ�)
	// �ҋ@��Ԃ�
	if (_nowUpdateState != &EnemyNormal::UpdateDamage) {
		if (_nowUpdateState != &EnemyNormal::UpdateIdle) {
			_nowUpdateState = &EnemyNormal::UpdateIdle;
			_animator->ChangeAnim(kAnimNameIdle, true);
		}
		return;
	}

	// ��L�̂�����ł��Ȃ���΋߂Â���Ԃ�
	if (_nowUpdateState != &EnemyNormal::UpdateDamage) {
		if (_nowUpdateState != &EnemyNormal::UpdateWalk) { // ���݋߂Â��łȂ����
			_nowUpdateState = &EnemyNormal::UpdateWalk;
			_animator->ChangeAnim(kAnimNameWalk, true);
		}
		return;
	}
}

void EnemyNormal::UpdateIdle()
{
	// �����Ȃ�
}

void EnemyNormal::UpdateWalk()
{
	// �v���C���[�Ɍ������Ĉړ�����
	
	// ���g����v���C���[�ւ̕����x�N�g��
	Vector3 dir = (_player.lock()->GetPos() - GetPos()).Normalize();

	Vector3 vel = dir * kWalkSpeed;

	// rigidbody�ɕҏW�����ړ��ʂ���
	rigidbody->SetVel(vel);
}

void EnemyNormal::UpdateAttack()
{
	// �U������

	// 
}

void EnemyNormal::UpdateDamage()
{
	// ��e����

	// ���������u�Ԃɒv���I�u�W�F�N�g(��������player)�Ɣ��Ε����Ƀm�b�N�o�b�N��������
}

void EnemyNormal::UpdateDead()
{
	// ���񂾏u�Ԃɒv���I�u�W�F�N�g(��������player)�Ɣ��Ε����ɐ�����т���

	// �A�j���[�V�������I����������
}
