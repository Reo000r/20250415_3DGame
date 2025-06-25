#include "SceneController.h"
//#include "SceneTitle.h"
#include "SceneGamePlay.h"

SceneController::SceneController()
{
	// ��ԍŏ��̃V�[�������͊��蓖�ĂĂ���
	ChangeScene(std::make_shared<SceneGamePlay>());
}

SceneController& SceneController::GetInstance()
{
	// ���̐錾�̎��_�Ń��������m�ۂ���ăA�v���I���܂Ŏc��
	static SceneController controller;
	return controller;
}

void SceneController::Update()
{
	// ��ԏ��Update�̂ݎ��s
	// ���w��Update�͖����Ă����Ă��炤
	_scenes.back()->Update();
}

void SceneController::Draw()
{
	// Draw�͑S�Ď��s
	for (auto& scene : _scenes)
	{
		scene->Draw();
	}
}

void SceneController::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	if (_scenes.empty())
	{
		// �󂾂����ꍇ�͐擪�ɒu��
		_scenes.push_back(scene);
	}
	else
	{
		// �Ȃɂ�����ꍇ�͍Ō�̗v�f��u��������
		_scenes.back() = scene;
	}
	scene->Init();
}

void SceneController::PushScene(std::shared_ptr<SceneBase> scene)
{
	// �Ō���ɒǉ�
	_scenes.push_back(scene);

	scene->Init();
}

void SceneController::PopScene()
{
	// ���s���̃V�[����1�ȉ��Ȃ�
	if (_scenes.size() <= 1)
	{
		// ���s���ׂ��V�[�����Ȃ��Ȃ邽�ߋ����Ȃ�
		return;
	}

	// �Ō������菜��
	_scenes.pop_back();
}
