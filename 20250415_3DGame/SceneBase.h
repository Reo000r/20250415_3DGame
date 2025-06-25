#pragma once

class SceneController;

/// <summary>
/// �V�[���̊��N���X
/// ���͎̂����Ȃ�
/// </summary>
class SceneBase
{
public:
	SceneBase() {};

	/// <summary>
	/// ������
	/// �R���X�g���N�^�ŏ���������Ƃ���
	/// �^�C�~���O�����������肵���ꍇ�̏���
	/// </summary>
	virtual void Init() abstract;

	/// <summary>
	/// �����ϐ��̍X�V
	/// </summary>
	virtual void Update() abstract;

	/// <summary>
	/// �`��S��
	/// </summary>
	virtual void Draw() abstract;
};