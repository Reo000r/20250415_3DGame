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
	// 初期位置
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
	// スティックによる平面移動
	Vector3 stick = Input::GetInstance().GetPadRightSitck();
	// Y軸回転
	_rotAngle.y += stick.x * 0.001f * kRotSpeedY;
#else
	//LRでカメラを回転させる
	if (input.IsPress("Rbutton")) {
		_rotAngle.y -= 0.05f;
	}
	if (input.IsPress("Lbutton")) {
		_rotAngle.y += 0.05f;
	}
#endif // USE_STICK

#else

	// 毎フレームの回転量 軸毎に作り合成する
	Quaternion rotDelta = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	Quaternion rotDeltaX = rotDelta;
	Quaternion rotDeltaY = rotDelta;

#ifdef USE_STICK
	// スティックによる平面移動
	Vector3 stick = Input::GetInstance().GetPadRightSitck();
	// Y軸回転
	// 横の入力を充てる
	rotDeltaY = AngleAxis(Vector3Up(), stick.x * 0.001f * kRotSpeedY);
	// X軸回転
	// 縦の入力を充てる
	rotDeltaX = AngleAxis(Vector3Right(), stick.z * 0.001f * kRotSpeedX);

	rotDelta = rotDeltaY * rotDeltaX;	// 
#else
	float rotSpeed = 0.0f;
	//LRでカメラを回転させる
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
	// さらにカメラが持つ回転情報を加える
	Matrix4x4 cameraRotMtx = MatRotateY(_rotAngle.y);
	// プレイヤーが持つ回転情報とカメラが持つ回転情報を合わせ
	// 最終的な回転情報を生成する
	rotMtx = playerRotMtx * cameraRotMtx;
#else
	// クォータニオンから回転行列を生成
	// プレイヤーを考慮していない
	rotMtx = ConvQuaternionToMatrix4x4(_quaternion * rotDelta);
#endif // !USE_QUATERNION

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
#ifdef _DEBUG
	int color = 0xffffff;
	int y = 16 * 6;
	DrawFormatString(0, y, color, L"Camera:Pos (%.3f,%.3f,%.3f)", _pos.x, _pos.y, _pos.z);
	y += 16;
	Vector3 rotAngle = ConvQuaternionToEuler(_quaternion);
	DrawFormatString(0, y, color, L"Camera:RotAngle (%.3f,%.3f,%.3f)", rotAngle.x, rotAngle.y, rotAngle.z);
#endif
}
