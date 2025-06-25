#pragma once
#include "Geometry.h"
#include <memory>

class Player;

/// <summary>
/// �Q�[���V�[���ɂ�����J�������Ǘ�����N���X
/// </summary>
class Camera final {
public:
	Camera();
	~Camera();

	void Init(std::weak_ptr<Player> player);
	void Update();
	void Draw();

	/// <summary>
	/// �����̉�]���
	/// </summary>
	/// <returns></returns>
	float GetRotAngleX() const { return _rotAngle.x; }

private:
	Vector3 _pos;
	Vector3 _vec;
	// Player�̈ʒu����������
	std::weak_ptr<Player> _player;
	Vector3 _targetPos;

	// x�ŉ��Ay�ŏc��]��\��
	Vector3 _rotAngle;
	float _near;
	float _far;
	float _viewAngle;
};

