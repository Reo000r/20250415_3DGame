#pragma once
#include <DxLib.h>

class Application final {
	// �V���O���g����
private:
	Application() {}
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

#ifdef _DEBUG	// �R���\�[��Debug�p�ϐ�
	FILE* _out;
	FILE* _in;
#endif	// _DEBUG

public:
	/// <summary>
	/// �V���O���g���I�u�W�F�N�g��Ԃ�
	/// </summary>
	/// <returns>Application�V���O���g���I�u�W�F�N�g</returns>
	static Application& GetInstance();

	/// <summary>
	/// �A�v���P�[�V�����̏�����
	/// </summary>
	/// <returns>true:���������� / false:���������s</returns>
	bool Init();

	/// <summary>
	/// ���C�����[�v���N��
	/// </summary>
	void Run();

	/// <summary>
	/// �A�v���P�[�V�����̌㏈��
	/// </summary>
	void Terminate();
};

