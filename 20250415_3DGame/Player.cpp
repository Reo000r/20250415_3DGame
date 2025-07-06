#include "Player.h"
#include "Camera.h"
#include "Animator.h"
#include "Input.h"
#include "Collider.h"
#include "Rigidbody.h"
#include <cassert>

#include <DxLib.h>

#define ANIMATION_TEST
// Quaternion���g�p������]
#define USE_QUATERNION
// 
//#define OLD_STATETRANSITION

namespace {
	constexpr float kWalkSpeed = 7.0f;
	constexpr float kDashSpeed = 14.0f;
	constexpr float kJumpForce = 20.0f;
	constexpr float kGround = 0.0f;

#ifndef ANIMATION_TEST
	const std::wstring kAnimName = L"Armature|Animation_";
	const std::wstring kAnimNameJump = kAnimName + L"Jump";
	const std::wstring kAnimNameJump = kAnimName + L"Attack360High";
#else
	const std::wstring kAnimName		= L"CharacterArmature|";
	const std::wstring kAnimNameIdle	= kAnimName + L"Idle";
	const std::wstring kAnimNameWalk	= kAnimName + L"Walk";
	const std::wstring kAnimNameRun		= kAnimName + L"Run";
	const std::wstring kAnimNameJump	= kAnimName + L"Jump_Idle";
	const std::wstring kAnimNameAttackFirst		= kAnimName + L"Attack_First";
	const std::wstring kAnimNameAttackSecond	= kAnimName + L"Attack_Second";
	const std::wstring kAnimNameAttackThird		= kAnimName + L"Attack_Third";
	const std::wstring kAnimNameDamage	= kAnimName + L"Damage";
	const std::wstring kAnimNameDead	= kAnimName + L"Dead";
#endif
}

Player::Player() :
	Collider(PhysicsData::Priority::High,
		PhysicsData::GameObjectTag::Player,
		PhysicsData::ColliderKind::Sphere,
		false),
	_nowUpdateState(&Player::UpdateIdle),
	_animator(std::make_shared<Animator>()),
	_rotAngle(0.0f)
{
	rigidbody->Init(true);

#ifndef ANIMATION_TEST
	// ���f���̓ǂݍ���
	_animator->Init(MV1LoadModel(L"data/model/Player.mv1"));
#else
	// ���f���̓ǂݍ���
	_animator->Init(MV1LoadModel(L"data/model/Player_test.mv1"));
#endif

#ifndef ANIMATION_TEST
	// �g�p����A�j���[�V������S�ē����
	_animator->SetAnimData(kAnimNameJump, true);
	// �ŏ��̃A�j���[�V������ݒ肷��
	_animator->SetStartAnim(kAnimNameJump);
#else
	// �g�p����A�j���[�V������S�ē����
	_animator->SetAnimData(kAnimNameIdle, true);
	_animator->SetAnimData(kAnimNameWalk, true);
	_animator->SetAnimData(kAnimNameRun, true);
	_animator->SetAnimData(kAnimNameJump, true);

	_animator->SetAnimData(kAnimNameAttackFirst, false);
	_animator->SetAnimData(kAnimNameAttackSecond, false);
	_animator->SetAnimData(kAnimNameAttackThird, false);
	_animator->SetAnimData(kAnimNameDamage, false);
	_animator->SetAnimData(kAnimNameDead, false);
	
	// �ŏ��̃A�j���[�V������ݒ肷��
	_animator->SetStartAnim(kAnimNameIdle);
#endif
	
}

Player::~Player() {
}

void Player::Init(std::weak_ptr<Camera> camera) {
	_camera = camera;
	MV1SetRotationXYZ(_animator->GetModelHandle(), Vector3(0, _rotAngle, 0));
}

void Player::Update() {
	_animator->Update();

#ifndef OLD_STATETRANSITION
	// ��ԑJ�ڊm�F
	CheckStateTransition();
#endif

	// ���݂̃X�e�[�g�ɉ�����Update���s����
	(this->*_nowUpdateState)();
}

void Player::Draw() {
	// �����蔻����s���Ă��烂�f���̈ʒu��ݒ肷��
	MV1SetPosition(_animator->GetModelHandle(), GetPos());
	// ���f���̕`��
	MV1DrawModel(_animator->GetModelHandle());

#ifdef _DEBUG
	int color = 0xffffff;
	int y = 16 * 2;
	DrawFormatString(0, y, color, L"Player:Pos (%.3f,%.3f,%.3f)", GetPos().x, GetPos().y, GetPos().z);
	y += 16;
	DrawFormatString(0, y, color, L"Player:Vel (%.3f,%.3f,%.3f)", GetVel().x, GetVel().y, GetVel().z);
	y += 16;
	DrawFormatString(0, y, color, L"Player:RotAngle (%.3f)", _rotAngle);
#endif
}

void Player::OnCollide(const std::weak_ptr<Collider> collider)
{
	// colider�ƏՓ�

	// ����̃^�O�ł͂Ȃ��ꍇreturn
	if (collider.lock()->GetTag() != PhysicsData::GameObjectTag::PlayerAttack) return;

	// (�_���[�W����)
	// ��e��Ԃ�
	if (_nowUpdateState != &Player::UpdateDamage) {
		_nowUpdateState = &Player::UpdateDamage;
		_animator->ChangeAnim(kAnimNameDamage, false);
		// �_���[�W���󂯂�

		return;
	}
}

void Player::CheckStateTransition()
{
	Input& input = Input::GetInstance();
	Vector3 stick = input.GetPadLeftSitck();
	// ���݂̃A�j���[�V�������I����Ă��邩���݂̃X�e�[�g���U���֘A�Ȃ�true
	bool isEndAttack = (
		_animator->IsEnd(_animator->GetCurrentAnimName()) &&
		(_nowUpdateState == &Player::UpdateAttackFirst ||
		_nowUpdateState == &Player::UpdateAttackSecond ||
		_nowUpdateState == &Player::UpdateAttackThird)
		);
	// �U���\�񂪓����Ă��邩


	// �n��ɂ��āA
	// �W�����v�{�^���������ꂽ��
	// �W�����v��Ԃ�
	if (input.IsTrigger("jump") && GetPos().y <= kGround) {
		_nowUpdateState = &Player::UpdateJump;
		_animator->ChangeAnim(kAnimNameJump, true); // �A�j���[�V�����������ŕύX

		// �W�����v������^���鏈���Ȃǂ������ɏ���
		Vector3 vel = GetVel();
		vel.y += kJumpForce;
		rigidbody->SetVel(vel);
		return; // ��Ԃ��ς�����̂ŁA�ȍ~�̔���͂��Ȃ�
	}

	// �n��ɂ��āA
	// �X�e�B�b�N���͂�����A
	// �_�b�V���{�^����������Ă�����
	// �_�b�V����Ԃ�
	if (GetPos().y <= kGround && 
		(stick.x != 0.0f || stick.z != 0.0f) && 
		input.IsPress("dash")) {
		if (_nowUpdateState != &Player::UpdateDash) { // ���݃_�b�V���łȂ����
			_nowUpdateState = &Player::UpdateDash;
			_animator->ChangeAnim(kAnimNameRun, true);
		}
		return;
	}

	// �n��ɂ��āA
	// �W�����v��ԂłȂ�����
	// �X�e�B�b�N���͂������
	// ������Ԃ�
	if (GetPos().y <= kGround && 
		(stick.x != 0.0f || stick.z != 0.0f)) {
		if (_nowUpdateState != &Player::UpdateWalk) { // ���ݕ����łȂ����
			_nowUpdateState = &Player::UpdateWalk;
			_animator->ChangeAnim(kAnimNameWalk, true);
		}
		return;
	}

	// ��L�̂�����ł��Ȃ���Αҋ@��Ԃ�
	if (GetPos().y <= kGround) {
		if (_nowUpdateState != &Player::UpdateIdle) {
			_nowUpdateState = &Player::UpdateIdle;
			_animator->ChangeAnim(kAnimNameIdle, true);
		}
		return;
	}
}

void Player::UpdateIdle()
{
	Input& input = Input::GetInstance();
	Vector3 stick = input.GetPadLeftSitck();

#ifdef OLD_STATETRANSITION
	// ���͂�����΃X�e�[�g��ύX����
	if ((int)stick.x != 0 || (int)stick.z != 0) {
		// �ǉ��̓��͂�����΃_�b�V��
		if (input.IsPress("dash")) {
			_animator->ChangeAnim(kAnimNameRun, true);
			_nowUpdateState = &Player::UpdateDash;
		}
		else {
			_animator->ChangeAnim(kAnimNameWalk, true);
			_nowUpdateState = &Player::UpdateWalk;
		}
	}
#endif // OLD_STATETRANSITION

	Vector3 vel = GetVel();

#ifdef OLD_STATETRANSITION
	// �W�����v����
	if (input.IsTrigger("jump") && GetPos().y <= kGround) {
		vel.y += kJumpForce;
		_animator->ChangeAnim(kAnimNameJump, true);
		_nowUpdateState = &Player::UpdateJump;
	}
#endif // OLD_STATETRANSITION

	const float cameraRot = _camera.lock()->GetRotAngleY();

#ifndef USE_QUATERNION
	// �J�������猩���ړ��ɕϊ�����
	_rotMtx = MatRotateY(cameraRot);
	vel = VecMultiple(_rotMtx, vel);
#else
	_quaternion = AngleAxis(Vector3Up(), _rotAngle);
	vel = VecMultiple(ConvQuaternionToMatrix4x4(_quaternion), vel);
#endif

	// rigidbody�ɕҏW�����ړ��ʂ���
	rigidbody->SetVel(vel);
}

void Player::UpdateWalk()
{
	// �ړ�����
	Move(kWalkSpeed);
	// �i�s�����ւ̕����]������
	Rotate();
}

void Player::UpdateDash()
{
	// �ړ�����
	Move(kDashSpeed);
	// �i�s�����ւ̕����]������
	Rotate();
}

void Player::UpdateJump()
{
#ifdef OLD_STATETRANSITION
	if (GetPos().y <= kGround) {
		Input& input = Input::GetInstance();
		Vector3 stick = input.GetPadLeftSitck();
		// ���͂�����΃X�e�[�g��ύX����
		if ((int)stick.x != 0 || (int)stick.z != 0) {
			// �ǉ��̓��͂�����΃_�b�V��
			if (input.IsPress("dash")) {
				_animator->ChangeAnim(kAnimNameRun, true);
				_nowUpdateState = &Player::UpdateDash;
			}
			else {
				_animator->ChangeAnim(kAnimNameWalk, true);
				_nowUpdateState = &Player::UpdateWalk;
			}
		}
		else {
			_animator->ChangeAnim(kAnimNameIdle, true);
			_nowUpdateState = &Player::UpdateIdle;
		}
	}
#endif // OLD_STATETRANSITION

	
	// �ړ�����
	Move(kDashSpeed);
	// �i�s�����ւ̕����]������
	Rotate();
}

void Player::UpdateAttackFirst()
{
	// �U������

	// ���͎�t�͈͓��ōU�����͂������
	// �U��second��\�񂷂�
}

void Player::UpdateAttackSecond()
{
}

void Player::UpdateAttackThird()
{
}

void Player::UpdateDamage()
{
}

void Player::UpdateDead()
{
}

void Player::Move(const float speed)
{
	Vector3 dir = {};
	Vector3 vel = GetVel();
	Position3 pos = GetPos();	// �ړ��\��ʒu
	Input& input = Input::GetInstance();
	const float cameraRot = _camera.lock()->GetRotAngleY();

	// �X�e�B�b�N�ɂ�镽�ʈړ�
	Vector3 stick = Input::GetInstance().GetPadLeftSitck();
	

#ifdef OLD_STATETRANSITION
	if (GetPos().y <= kGround) {
		// �������͂��Ȃ����ړ��ʂ��Ȃ��Ȃ�
		if (((int)stick.x == 0 && (int)stick.z == 0) &&
			vel.SqrMagnitude() < PhysicsData::sleepThreshold) {
			_animator->ChangeAnim(kAnimNameIdle, true);
			_nowUpdateState = &Player::UpdateIdle;
			return;
		}
		else if (input.IsPress("dash") &&
			_nowUpdateState != &Player::UpdateDash) {
			_animator->ChangeAnim(kAnimNameRun, true);
			_nowUpdateState = &Player::UpdateDash;
			return;
		}
		else if (!input.IsPress("dash") &&
			_nowUpdateState != &Player::UpdateWalk) {
			_animator->ChangeAnim(kAnimNameWalk, true);
			_nowUpdateState = &Player::UpdateWalk;
			return;
		}
	}
#endif // OLD_STATETRANSITION


	// ���͂������Ă��Ȃ����ł�x��-0.0f�������Ă���
	dir.x = -stick.x;
	dir.z = stick.z;


#ifdef OLD_STATETRANSITION
	// �W�����v����
	if (input.IsTrigger("jump") && GetPos().y <= kGround) {
		vel.y += kJumpForce;
		_animator->ChangeAnim(kAnimNameJump, true);
		_nowUpdateState = &Player::UpdateJump;
	}
#endif // OLD_STATETRANSITION

	
	dir.Normalized();

	// �J�������猩���ړ������ɕϊ�����
	_quaternion = AngleAxis(Vector3Up(), cameraRot);	// Y����]Q���쐬
	dir = RotateVector3(_quaternion, dir);				// ��]Q��K�p
	//_quaternion = ConvMatrix4x4ToQuaternion(MatRotateY(cameraRot));
	//dir = VecMultiple(ConvQuaternionToMatrix4x4(_quaternion), dir);

	vel.x += dir.x * speed;
	vel.z += dir.z * speed;

	pos += vel;
	if (pos.y < kGround) {
		pos.y = kGround;
		vel.y = 0.0f;
	}

	// rigidbody�ɕҏW�����ړ��ʂ���
	rigidbody->SetVel(vel);
}

void Player::Rotate() {
#ifdef USE_STICK
	// �X�e�B�b�N�ɂ�镽�ʈړ�
	Vector3 stick = Input::GetInstance().GetPadLeftSitck();
	// ���͂��������ꍇ�̂݃L�����N�^�[�̌�����ύX
	if (stick.x != 0.0f || stick.z != 0.0f) {
		const float cameraRot = _camera.lock()->GetRotAngleY();
		float a = atan2f(stick.z, stick.x) + -cameraRot + DX_PI_F * 0.5f;
		MV1SetRotationXYZ(_animator->GetModelHandle(), Vector3(0, a, 0));
	}
#else
	MV1SetRotationXYZ(_modelHandle, Vector3(0, _rotAngle, 0));
#endif
}
