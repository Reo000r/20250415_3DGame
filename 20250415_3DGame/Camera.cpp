#include "Camera.h"
#include "Player.h"
#include "Input.h"

#include <DxLib.h>

namespace {
	// �ǔ��͈�
	constexpr float kTargetHormingRad = 1000;
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
	_rotAngle(Vector3(DX_PI_F / 180.0f * 180.0f, 0.0f, 0.0f)),
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
	//float _cameraRot = 0.0f;
#ifdef USE_STICK
	// �X�e�B�b�N�ɂ�镽�ʈړ�
	Vector3 stick = Input::GetInstance().GetPadRightSitck();
	_rotAngle.x += stick.x * 0.00005f;
#else
	//LR�ŃJ��������]������
	if (input.IsPress("Rbutton")) {
		_rotAngle.x -= 0.05f;
	}
	if (input.IsPress("Lbutton")) {
		_rotAngle.x += 0.05f;
	}
#endif // USE_STICK

	// �v���C���[�̈ړ��ɍ��킹�Ĉړ����s��
	Vector3 playerPos = _player.lock()->GetPos();		// �Ƃ肠�������_
	//// �v���C���[�̉�]��񂩂��Ɍ�납�猩���J�����ɂ���
	Matrix4x4 playerRotMtx = MatIdentity();//_player.lock()->GetRotMtx();
	// ����ɃJ����������]����������
	Matrix4x4 cameraRotMtx = MatRotateY(_rotAngle.x);
	// �v���C���[������]���ƃJ����������]�������킹
	// �ŏI�I�ȉ�]���𐶐�����
	Matrix4x4 rotMtx = playerRotMtx * cameraRotMtx;

	// �v���C���[����J�����Ɍ������x�N�g����
	// �v���C���[�̉�]���ɍ��킹�ĉ�]������
	Vector3 toCamera = rotMtx.VecMultiple(kPlayerToCamera);
	Vector3 toTarget = rotMtx.VecMultiple(kPlayerToTarget);

	// �㉺�����̓J������x�点�Ēǔ�
	// �J�������ŏI�I�ɂ��Ăق����ʒu
	Vector3 finalPos = playerPos + toCamera;
	Vector3 finalTargetPos = playerPos + toTarget;

	// ���݈ʒu�ƍŏI�I�ɂ��Ăق����ʒu�̒��_�����߂�
	_pos = _pos * 0.9f + finalPos * 0.1f;
	_targetPos = _targetPos * 0.9f + finalTargetPos * 0.1f;

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
	DrawFormatString(0, y, color, L"Camera:RotAngle (%.3f,%.3f,%.3f)", _rotAngle.x, _rotAngle.y, _rotAngle.z);
#endif
}
