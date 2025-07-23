#include "Camera.h"
#include "Player.h"
#include "Input.h"
#include "Calculation.h"

#include <DxLib.h>

namespace {
	constexpr float kTargetHormingRad = 1000;

	constexpr float kRotSpeedX = 0.03f;
	constexpr float kRotSpeedY = 0.05f;

	// 
	constexpr float kCameraFollowLerpFactor = 0.1f;

	// 
	const Vector3 kDefaultPosition = { 0.0f, 520.0f, -860.0f };
	const float kDefaultRotation = Calc::ToRadian(180.0f);

	const Vector3 kPlayerToCamera = { 0.0f, 520.0f, 860.0f };
	const Vector3 kPlayerToTarget = { 0.0f, 220.0f, 0.0f };
	// 初期位置
	const Vector3 kDefaultPos = { -300, 500, -700 };
	const Vector3 kTargetOffset = { 0, 200, 0 };
}

Camera::Camera() :
	_pos(kDefaultPosition),
	_player(),
	_targetPos(kPlayerToTarget),
	_rotAngle(Vector3(0.0f, kDefaultRotation, 0.0f)),
	_near(10.0f),
	_far(10000.0f),
	_viewAngle(Calc::ToRadian(60.0f)),
	_lightHandle(-1)
{
}

Camera::~Camera() {
	DeleteLightHandle(_lightHandle);
}

void Camera::Init(std::weak_ptr<Player> player) {
	_player = player;
	_targetPos = _player.lock()->GetPos() + kPlayerToTarget;
	//SetCameraPositionAndTarget_UpVecY(pos, target);
	SetCameraPositionAndTarget_UpVecY(_pos, _targetPos);

	//_lightHandle = CreatePointLightHandle(_pos, _far, 
	//	1.0f, 0.005f, 0.0f);
	_lightHandle = CreateSpotLightHandle(
		_pos,
		Vector3Up(),
		_viewAngle,
		_viewAngle * 0.9f,
		_far,
		1.0f,
		0.005f,
		0.0f);
}

void Camera::Update() {
	auto& input = Input::GetInstance();

	// スティックによる平面移動
	Vector3 stick = Input::GetInstance().GetPadRightSitck();
	// Y軸回転
	_rotAngle.y += stick.x * 0.001f * kRotSpeedY;


	Matrix4x4 rotMtx = MatIdentity();


	Matrix4x4 playerRotMtx = MatIdentity();
	// さらにカメラが持つ回転情報を加える
	Matrix4x4 cameraRotMtx = MatRotateY(_rotAngle.y);
	// プレイヤーが持つ回転情報とカメラが持つ回転情報を合わせ
	// 最終的な回転情報を生成する
	rotMtx = playerRotMtx * cameraRotMtx;

	// プレイヤーの移動に合わせて移動を行う
	Vector3 playerPos = _player.lock()->GetPos();
	// プレイヤーからカメラに向かうベクトルを
	// プレイヤーの回転情報に合わせて回転させる
	Vector3 toCamera = rotMtx.VecMultiple(kPlayerToCamera);
	Vector3 toTarget = rotMtx.VecMultiple(kPlayerToTarget);

	// 上下方向はカメラを遅らせて追尾
	// カメラが最終的にいてほしい位置
	Vector3 finalPos = playerPos + toCamera;
	Vector3 finalTargetPos = playerPos + toTarget;

	// 現在位置と最終的にいてほしい位置の中点を求める
	_pos = _pos * (1.0f - kCameraFollowLerpFactor) + finalPos * kCameraFollowLerpFactor;
	_targetPos = _targetPos * (1.0f - kCameraFollowLerpFactor) + finalTargetPos * kCameraFollowLerpFactor;

	// カメラの位置、描画距離、画角を更新
	SetCameraPositionAndTarget_UpVecY(_pos, _targetPos);
	SetCameraNearFar(_near, _far);
	SetupCamera_Perspective(_viewAngle);
}

void Camera::Draw() {
	// ライトの位置と方向を更新
	if (_lightHandle != -1) {
		// ライトの位置をカメラの位置に設定
		SetLightPositionHandle(_lightHandle, _pos);

		// ライトの方向をカメラの位置から注視点へのベクトルに設定
		VECTOR direction = (_targetPos - _pos).Normalize();
		SetLightDirectionHandle(_lightHandle, direction);

		// ライトを有効にする
		SetLightEnableHandle(_lightHandle, true);
	}

#ifdef _DEBUG
	int color = 0xffffff;
	int y = 16 * 6;
	DrawFormatString(0, y, color, L"Camera:Pos (%.3f,%.3f,%.3f)", _pos.x, _pos.y, _pos.z);
	y += 16;
	Vector3 rotAngle;
	rotAngle = _rotAngle;
	DrawFormatString(0, y, color, L"Camera:RotAngle (%.3f,%.3f,%.3f)", rotAngle.x, rotAngle.y, rotAngle.z);
#endif
}
