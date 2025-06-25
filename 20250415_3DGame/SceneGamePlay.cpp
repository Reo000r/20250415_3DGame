#include "SceneGamePlay.h"
#include "SceneResult.h"
#include "SceneController.h"
#include "Camera.h"
#include "Player.h"
#include "Ground.h"
#include "Physics.h"

#include "Statistics.h"
#include "Input.h"

#include <memory>
#include <DxLib.h>
#include <cassert>

SceneGamePlay::SceneGamePlay() :
	_frame(Statistics::kFadeInterval),
	_nextScene(std::make_shared<SceneResult>()),
	_physics(std::make_shared<Physics>()),
	_camera(std::make_shared<Camera>()),
	_player(std::make_shared<Player>()),
	_ground(std::make_shared<Ground>()),
	_nowUpdateState(&SceneGamePlay::FadeinUpdate),
	_nowDrawState(&SceneGamePlay::FadeDraw)
{
}

SceneGamePlay::~SceneGamePlay()
{
	_player->ReleasePhysics();
}

void SceneGamePlay::Init()
{
	// ����������
	//_any->Init();
	_camera->Init(_player);
	_player->Init(_camera);
	_ground->Init();

	_player->EntryPhysics(_physics);
}

void SceneGamePlay::Update()
{
	(this->*_nowUpdateState)();
}

void SceneGamePlay::Draw()
{
	(this->*_nowDrawState)();
}

void SceneGamePlay::FadeinUpdate()
{
	_frame--;

	if (_frame <= 0) {
		_nowUpdateState = &SceneGamePlay::NormalUpdate;
		_nowDrawState = &SceneGamePlay::NormalDraw;
	}
}

void SceneGamePlay::NormalUpdate()
{
	//_any->Update();
	_camera->Update();
	_player->Update();
	_ground->Update();

	// �������Z�X�V
	_physics->Update();

	// �������������
	if (Input::GetInstance().IsTrigger("next")) {
		_nowUpdateState = &SceneGamePlay::FadeoutUpdate;
		_nowDrawState = &SceneGamePlay::FadeDraw;
		_frame = 0;
	}
}

void SceneGamePlay::FadeoutUpdate()
{
	_frame++;

	if (_frame >= Statistics::kFadeInterval) {
		SceneController::GetInstance().ChangeScene(_nextScene);
		return;  // ����������ł���̂ł���
		// �]�v�ȏ����������Ă���Ƃ܂����̂�return;
	}
}

void SceneGamePlay::FadeDraw()
{
	//_any->Draw();
	_ground->Draw();
	_camera->Draw();
	_player->Draw();

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene GamePlay");
#endif

	// �t�F�[�h�C��/�A�E�g�̏���
	// �t�F�[�h�����̌v�Z(0.0-1.0)
	float rate = static_cast<float>(_frame) / static_cast<float>(Statistics::kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
	// BlendMode���g�������NOBLEND�ɂ��Ă������Ƃ�Y�ꂸ
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneGamePlay::NormalDraw()
{
	//_any->Draw();
	_ground->Draw();
	_camera->Draw();
	_player->Draw();

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene GamePlay");
#endif
}