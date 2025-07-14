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

	Position3 GetPos() { return _pos; }

	/// <summary>
	/// Y軸の回転情報
	/// </summary>
	/// <returns></returns>
	float GetRotAngleY() const { return _rotAngle.y; }

private:
	Position3 _pos;
	Vector3 _vel;
	// Playerの位置だけ見たい
	std::weak_ptr<Player> _player;
	Vector3 _targetPos;

	// それぞれの回転量を表す
	Vector3 _rotAngle;

	float _near;
	float _far;
	float _viewAngle;

	int _lightHandle;
};

