#pragma once
#include "SceneBase.h"

#include <memory>

class SceneResult final : public SceneBase {
public:
	SceneResult();
	~SceneResult();

	/// <summary>
	/// ������
	/// </summary>
	void Init() override;

	/// <summary>
	/// �����ϐ��̍X�V
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// �`��S��
	/// </summary>
	virtual void Draw() override;

private:
	enum class SceneName {
		Title,
		GamePlay,
	};

	int _frame;

	SceneName _nextSceneName;
	std::shared_ptr<SceneBase> _nextScene;

	// _update��_draw���ϐ��ł��邱�Ƃ𕪂���₷�����Ă���
	using UpdateFunc_t = void(SceneResult::*)();
	using DrawFunc_t = void(SceneResult::*)();

	UpdateFunc_t _nowUpdateState = nullptr;
	DrawFunc_t   _nowDrawState = nullptr;

	// �t�F�[�h�C�����̍X�V����
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

