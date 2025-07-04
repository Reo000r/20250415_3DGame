#include "Camera.h"
#include "Player.h"
#include "Input.h"

#include <DxLib.h>

#define USE_QUATERNION

namespace {

	constexpr float kRotSpeedX = 0.03f;
	constexpr float kRotSpeedY = 0.05f;

	// 
	constexpr float kCameraFollowLerpFactor = 0.1f;

	// 
	const Vector3 kPlayerToCamera = { 0.0f, 520.0f, 860.0f };
	const Vector3 kPlayerToTarget = { 0.0f, 220.0f, 0.0f };
	// �����ʒu
	const Vector3 kDefaultPos = { -300, 500, -700 };
	const Vector3 kTargetOffset = { 0, 200, 0 };
}

Camera::Camera() :
	_pos(0, 520, -860),
	_player(),
	_targetPos(0, 200, 0),
#ifndef USE_QUATERNION
	_rotAngle(Vector3(0.0f, DX_PI_F / 180.0f * 180.0f, 0.0f)),
#else
	_quaternion(ConvEulerToQuaternion(Vector3(0.0f, DX_PI_F / 180.0f * 180.0f, 0.0f))),
#endif // !USE_QUATERNION
	_near(10.0f),
	_far(10000.0f),
	_viewAngle(DX_PI_F / 180.0f * 60.0f)
{
}

Camera::~Camera() {
}

void Camera::Init(std::weak_ptr<Player> player) {
	_player = player;
	_targetPos = _player.lock()->GetPos() + kPlayerToTarget;
	//SetCameraPositionAndTarget_UpVecY(pos, target);
	SetCameraPositionAndTarget_UpVecY(_pos, _targetPos);
}

void Camera::Update() {
	auto& input = Input::GetInstance();

#ifndef USE_QUATERNION
#ifdef USE_STICK
	// �X�e�B�b�N�ɂ�镽�ʈړ�
	Vector3 stick = Input::GetInstance().GetPadRightSitck();
	// Y����]
	_rotAngle.y += stick.x * 0.001f * kRotSpeedY;
#else
	//LR�ŃJ��������]������
	if (input.IsPress("Rbutton")) {
		_rotAngle.y -= 0.05f;
	}
	if (input.IsPress("Lbutton")) {
		_rotAngle.y += 0.05f;
	}
#endif // USE_STICK

#else

	// ���t���[���̉�]�� �����ɍ�荇������
	Quaternion rotDelta = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	Quaternion rotDeltaX = rotDelta;
	Quaternion rotDeltaY = rotDelta;

#ifdef USE_STICK
	// �X�e�B�b�N�ɂ�镽�ʈړ�
	Vector3 stick = Input::GetInstance().GetPadRightSitck();
	// Y����]
	// ���̓��͂��[�Ă�
	rotDeltaY = AngleAxis(Vector3Up(), stick.x * 0.001f * kRotSpeedY);
	// X����]
	// �c�̓��͂��[�Ă�
	rotDeltaX = AngleAxis(Vector3Right(), stick.z * 0.001f * kRotSpeedX);

	rotDelta = rotDeltaY * rotDeltaX;	// 
#else
	float rotSpeed = 0.0f;
	//LR�ŃJ��������]������
	if (input.IsPress("Rbutton")) {
		rotSpeed -= 0.05f;
	}
	if (input.IsPress("Lbutton")) {
		rotSpeed += 0.05f;
	}
	rotDelta = AngleAxis(Vector3Up(), rotSpeed);
#endif // USE_STICK
#endif // !USE_QUATERNION


	Matrix4x4 rotMtx = MatIdentity();
	

#ifndef USE_QUATERNION
	Matrix4x4 playerRotMtx = MatIdentity();
	// ����ɃJ����������]����������
	Matrix4x4 cameraRotMtx = MatRotateY(_rotAngle.y);
	// �v���C���[������]���ƃJ����������]�������킹
	// �ŏI�I�ȉ�]���𐶐�����
	rotMtx = playerRotMtx * cameraRotMtx;
#else
	// �N�H�[�^�j�I�������]�s��𐶐�
	// �v���C���[���l�����Ă��Ȃ�
	rotMtx = ConvQuaternionToMatrix4x4(_quaternion * rotDelta);
#endif // !USE_QUATERNION

	// �v���C���[�̈ړ��ɍ��킹�Ĉړ����s��
	Vector3 playerPos = _player.lock()->GetPos();
	// �v���C���[����J�����Ɍ������x�N�g����
	// �v���C���[�̉�]���ɍ��킹�ĉ�]������
	Vector3 toCamera = rotMtx.VecMultiple(kPlayerToCamera);
	Vector3 toTarget = rotMtx.VecMultiple(kPlayerToTarget);

	// �㉺�����̓J������x�点�Ēǔ�
	// �J�������ŏI�I�ɂ��Ăق����ʒu
	Vector3 finalPos = playerPos + toCamera;
	Vector3 finalTargetPos = playerPos + toTarget;

	// ���݈ʒu�ƍŏI�I�ɂ��Ăق����ʒu�̒��_�����߂�
	_pos = _pos * (1.0f - kCameraFollowLerpFactor) + finalPos * kCameraFollowLerpFactor;
	_targetPos = _targetPos * (1.0f - kCameraFollowLerpFactor) + finalTargetPos * kCameraFollowLerpFactor;

	// �J�����̈ʒu�A�`�拗���A��p���X�V
	SetCameraPositionAndTarget_UpVecY(_pos, _targetPos);
	SetCameraNearFar(_near, _far);
	SetupCamera_Perspective(_viewAngle);
}

void Camera::Draw() {
#ifdef _DEBUG
	int color = 0xffffff;
	int y = 16 * 6;
	DrawFormatString(0, y, color, L"Camera:Pos (%.3f,%.3f,%.3f)", _pos.x, _pos.y, _pos.z);
	y += 16;
	Vector3 rotAngle = ConvQuaternionToEuler(_quaternion);
	DrawFormatString(0, y, color, L"Camera:RotAngle (%.3f,%.3f,%.3f)", rotAngle.x, rotAngle.y, rotAngle.z);
#endif
}
