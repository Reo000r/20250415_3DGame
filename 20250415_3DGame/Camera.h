#pragma once
#include "Geometry.h"
#include <memory>

class Player;

/// <summary>
/// ゲームシーンにおけるカメラを管理するクラス
/// </summary>
class Camera final {
public:
	Camera();
	~Camera();

	void Init(std::weak_ptr<Player> player);
	void Update();
	void Draw();

	/// <summary>
	/// 横軸の回転情報
	/// </summary>
	/// <returns></returns>
	float GetRotAngleX() const { return _rotAngle.x; }

private:
	Vector3 _pos;
	Vector3 _vec;
	// Playerの位置だけ見たい
	std::weak_ptr<Player> _player;
	Vector3 _targetPos;

	// xで横、yで縦回転を表す
	Vector3 _rotAngle;
	float _near;
	float _far;
	float _viewAngle;
};

