#include "PlayerStateIdle.h"
#include "Player.h"
#include "Input.h"
#include "Animator.h"
#include "Rigidbody.h"
#include "ProjectSettings.h"

/*
PlayerStateIdle::PlayerStateIdle()
{
}

PlayerStateIdle::~PlayerStateIdle()
{
}

void PlayerStateIdle::Enter(std::shared_ptr<Player> player)
{
}

void PlayerStateIdle::Update(std::shared_ptr<Player> player)
{
}

void PlayerStateIdle::Exit(std::shared_ptr<Player> player)
{
}

void PlayerStateIdle::CheckTransition(std::shared_ptr<Player> player)
{
	Input& input = Input::GetInstance();
	Vector3 stick = input.GetPadLeftSitck();

	// �n��ɂ��āA
	// �W�����v�{�^���������ꂽ��
	// �W�����v��Ԃ�
	if (player->GetPos().y <= PhysicsData::kTempGroundHeight && 
		input.IsTrigger("jump")) {
		SetNowUpdateState(player, &Player::UpdateJump);
		GetAnimator(player)->ChangeAnim(kAnimNameJump, true); // �A�j���[�V�����������ŕύX

		// �W�����v������^���鏈���Ȃǂ������ɏ���
		Vector3 vel = player->GetVel();
		vel.y += kJumpForce;
		GetRigidbody(player)->SetVel(vel);
		return; // ��Ԃ��ς�����̂ŁA�ȍ~�̔���͂��Ȃ�
	}

	// �n��ɂ��āA
	// �X�e�B�b�N���͂�����A
	// �_�b�V���{�^����������Ă�����
	// �_�b�V����Ԃ�
	if (player->GetPos().y <= PhysicsData::kTempGroundHeight &&
		(stick.x != 0.0f || stick.z != 0.0f) &&
		input.IsPress("dash")) {
		if (GetNowUpdateState(player) != &Player::UpdateDash) { // ���݃_�b�V���łȂ����
			SetNowUpdateState(player, &Player::UpdateDash);
			GetAnimator(player)->ChangeAnim(kAnimNameRun, true);
		}
		return;
	}

	// �n��ɂ��āA
	// �W�����v��ԂłȂ�����
	// �X�e�B�b�N���͂������
	// ������Ԃ�
	if (player->GetPos().y <= PhysicsData::kTempGroundHeight &&
		(stick.x != 0.0f || stick.z != 0.0f)) {
		if (GetNowUpdateState(player) != &Player::UpdateWalk) { // ���ݕ����łȂ����
			SetNowUpdateState(player, &Player::UpdateWalk);
			GetAnimator(player)->ChangeAnim(kAnimNameWalk, true);
		}
		return;
	}

	// ��L�̂�����ł��Ȃ���Αҋ@��Ԃ�
	if (player->GetPos().y <= PhysicsData::kTempGroundHeight) {
		if (GetNowUpdateState(player) != &Player::UpdateIdle) {
			SetNowUpdateState(player, &Player::UpdateIdle);
			GetAnimator(player)->ChangeAnim(kAnimNameIdle, true);
		}
		return;
	}
}
*/