#pragma once
#include "SceneBase.h"

#include <memory>

class SceneTitle : public SceneBase
{
private:
	int _frame;
	int _startFadeFrame;
	
	// _update��_draw���ϐ��ł��邱�Ƃ𕪂���₷�����Ă���
	using UpdateFunc_t = void (SceneTitle::*)();
	using DrawFunc_t = void (SceneTitle::*)();
	UpdateFunc_t _nowUpdateState = nullptr;
	DrawFunc_t _nowDrawState = nullptr;

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

public:
	SceneTitle();
	~SceneTitle();

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
};