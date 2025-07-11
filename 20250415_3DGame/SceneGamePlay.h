#pragma once
#include "SceneBase.h"
#include "Geometry.h"

#include <memory>

class Physics;
class Camera;
class Player;
class Ground;
class Skydome;

class SceneGamePlay final : public SceneBase {
public:
	SceneGamePlay();
	~SceneGamePlay();

	/// <summary>
	/// ������
	/// </summary>
	void Init() override;

	/// <summary>
	/// �����ϐ��̍X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��S��
	/// </summary>
	void Draw() override;

private:
	// 
	int _frame;

	// ���̃V�[��
	std::shared_ptr<SceneBase> _nextScene;

	std::shared_ptr<Physics> _physics;

	std::shared_ptr<Camera> _camera;
	std::shared_ptr<Player> _player;
	std::shared_ptr<Ground> _ground;
	std::shared_ptr<Skydome> _skydome;

	// Update��Draw��State�p�^�[��
	// _update��_draw���ϐ��ł��邱�Ƃ𕪂���₷�����Ă���
	using UpdateFunc_t = void(SceneGamePlay::*)();
	using DrawFunc_t = void(SceneGamePlay::*)();
	UpdateFunc_t _nowUpdateState = nullptr;
	DrawFunc_t _nowDrawState = nullptr;

	/// <summary>
	/// �t�F�[�h�C�����̍X�V����
	/// </summary>
	void FadeinUpdate();
	// �ʏ펞�̍X�V����
	void NormalUpdate();
	// �t�F�[�h�A�E�g���̍X�V����
	void FadeoutUpdate();

	// �t�F�[�h���̕`��
	void FadeDraw();
	// �ʏ펞�̕`��
	void NormalDraw();
};

