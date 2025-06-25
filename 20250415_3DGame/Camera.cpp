#include "Camera.h"
#include "Player.h"
#include "Input.h"

#include <DxLib.h>

namespace {
	// 追尾範囲
	constexpr float kTargetHormingRad = 1000;
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
	// スティックによる平面移動
	Vector3 stick = Input::GetInstance().GetPadRightSitck();
	_rotAngle.x += stick.x * 0.00005f;
#else
	//LRでカメラを回転させる
	if (input.IsPress("Rbutton")) {
		_rotAngle.x -= 0.05f;
	}
	if (input.IsPress("Lbutton")) {
		_rotAngle.x += 0.05f;
	}
#endif // USE_STICK

	// プレイヤーの移動に合わせて移動を行う
	Vector3 playerPos = _player.lock()->GetPos();		// とりあえず原点
	//// プレイヤーの回転情報から常に後ろから見たカメラにする
	Matrix4x4 playerRotMtx = MatIdentity();//_player.lock()->GetRotMtx();
	// さらにカメラが持つ回転情報を加える
	Matrix4x4 cameraRotMtx = MatRotateY(_rotAngle.x);
	// プレイヤーが持つ回転情報とカメラが持つ回転情報を合わせ
	// 最終的な回転情報を生成する
	Matrix4x4 rotMtx = playerRotMtx * cameraRotMtx;

	// プレイヤーからカメラに向かうベクトルを
	// プレイヤーの回転情報に合わせて回転させる
	Vector3 toCamera = rotMtx.VecMultiple(kPlayerToCamera);
	Vector3 toTarget = rotMtx.VecMultiple(kPlayerToTarget);

	// 上下方向はカメラを遅らせて追尾
	// カメラが最終的にいてほしい位置
	Vector3 finalPos = playerPos + toCamera;
	Vector3 finalTargetPos = playerPos + toTarget;

	// 現在位置と最終的にいてほしい位置の中点を求める
	_pos = _pos * 0.9f + finalPos * 0.1f;
	_targetPos = _targetPos * 0.9f + finalTargetPos * 0.1f;

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
	DrawFormatString(0, y, color, L"Camera:RotAngle (%.3f,%.3f,%.3f)", _rotAngle.x, _rotAngle.y, _rotAngle.z);
#endif
}
