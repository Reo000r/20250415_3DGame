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

	Position3 GetPos() { return _pos; }

	/// <summary>
	/// Y���̉�]���
	/// </summary>
	/// <returns></returns>
	float GetRotAngleY() const { return ConvQuaternionToEuler(_quaternion).y; }

private:
	Position3 _pos;
	// Player�̈ʒu����������
	std::weak_ptr<Player> _player;
	Vector3 _targetPos;

	// ���ꂼ��̍��W���ł̉�]�ʂ�\��
	Quaternion _quaternion;
	float _near;
	float _far;
	float _viewAngle;

	int _lightHandle;
};

