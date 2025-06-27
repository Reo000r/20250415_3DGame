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

	// 地上にいて、
	// ジャンプボタンが押されたら
	// ジャンプ状態へ
	if (player->GetPos().y <= PhysicsData::kTempGroundHeight && 
		input.IsTrigger("jump")) {
		SetNowUpdateState(player, &Player::UpdateJump);
		GetAnimator(player)->ChangeAnim(kAnimNameJump, true); // アニメーションもここで変更

		// ジャンプ初速を与える処理などもここに書く
		Vector3 vel = player->GetVel();
		vel.y += kJumpForce;
		GetRigidbody(player)->SetVel(vel);
		return; // 状態が変わったので、以降の判定はしない
	}

	// 地上にいて、
	// スティック入力があり、
	// ダッシュボタンが押されていたら
	// ダッシュ状態へ
	if (player->GetPos().y <= PhysicsData::kTempGroundHeight &&
		(stick.x != 0.0f || stick.z != 0.0f) &&
		input.IsPress("dash")) {
		if (GetNowUpdateState(player) != &Player::UpdateDash) { // 現在ダッシュでなければ
			SetNowUpdateState(player, &Player::UpdateDash);
			GetAnimator(player)->ChangeAnim(kAnimNameRun, true);
		}
		return;
	}

	// 地上にいて、
	// ジャンプ状態でないかつ
	// スティック入力があれば
	// 歩き状態へ
	if (player->GetPos().y <= PhysicsData::kTempGroundHeight &&
		(stick.x != 0.0f || stick.z != 0.0f)) {
		if (GetNowUpdateState(player) != &Player::UpdateWalk) { // 現在歩きでなければ
			SetNowUpdateState(player, &Player::UpdateWalk);
			GetAnimator(player)->ChangeAnim(kAnimNameWalk, true);
		}
		return;
	}

	// 上記のいずれでもなければ待機状態へ
	if (player->GetPos().y <= PhysicsData::kTempGroundHeight) {
		if (GetNowUpdateState(player) != &Player::UpdateIdle) {
			SetNowUpdateState(player, &Player::UpdateIdle);
			GetAnimator(player)->ChangeAnim(kAnimNameIdle, true);
		}
		return;
	}
}
*/