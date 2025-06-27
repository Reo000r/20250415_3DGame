#pragma once
#include "Player.h"
#include <memory>

/*

class Player;

class PlayerState abstract {
public:
    virtual ~PlayerState() = default;

    virtual void Enter(std::shared_ptr<Player> player) abstract;
    virtual void Update(std::shared_ptr<Player> player) abstract;
    virtual void Exit(std::shared_ptr<Player> player) abstract;

    /// <summary>
    /// ��Ԃ��ω����邩�ǂ����̃`�F�b�N
    /// </summary>
    virtual void CheckTransition(std::shared_ptr<Player> player) abstract;

protected:
    // �X�e�[�g��Player�̃����o�ɃA�N�Z�X�������ꍇ�Ɏg�p����֐�
    Player::UpdateFunc_t GetNowUpdateState(std::shared_ptr<Player> player)  { return player->_nowUpdateState; };
    void SetNowUpdateState(std::shared_ptr<Player> player, Player::UpdateFunc_t state)  { player->_nowUpdateState = state; };
    std::shared_ptr<Animator> GetAnimator(std::shared_ptr<Player> player)   { return player->_animator; };
	std::weak_ptr<Camera> GetCamera(std::shared_ptr<Player> player)         { return player->_camera; };
    
    float GetRotAngle(std::shared_ptr<Player> player)                       { return player->_rotAngle; };
    Matrix4x4 GetRotMtx(std::shared_ptr<Player> player)                     { return player->_rotMtx; };
    std::shared_ptr<Rigidbody> GetRigidbody(std::shared_ptr<Player> player)  { return player->rigidbody; };
};

*/