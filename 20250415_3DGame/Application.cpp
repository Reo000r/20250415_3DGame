#include "Application.h"
#include "Input.h"
#include "SceneController.h"
#include "Statistics.h"

#include <DxLib.h>
#include <cassert>
#include <string>

Application& Application::GetInstance()
{
	// ���̐錾�̎��_�Ń��������m�ۂ���ăA�v���I���܂Ŏc��
	static Application app;
	return app;
}

bool Application::Init()
{
#ifdef _DEBUG	// �R���\�[��Debug�p
	AllocConsole();							// �R���\�[��
	_out = 0;
	freopen_s(&_out, "CON", "w", stdout);	// stdout
	_in = 0;
	freopen_s(&_in, "CON", "r", stdin);		// stdin
#endif	// _DEBUG

	SetGraphMode(Statistics::kScreenWidth, Statistics::kScreenHeight, 32);
	ChangeWindowMode(true);
	SetWindowText(L"Skeleton Window");

	if (DxLib_Init()) {
		assert(false && "DxLib_Init_Error");
		return false;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	// 3D�֘A�ݒ�
	SetUseZBuffer3D(true);		// Z�o�b�t�@�L����
	SetWriteZBuffer3D(true);	// Z�o�b�t�@�ւ̏������݂��s��
	SetUseBackCulling(true);	// �o�b�N�J�����O��L���ɂ���

	return true;
}

void Application::Run()
{
	SceneController& sceneController = SceneController::GetInstance();
	Input& input = Input::GetInstance();

	while (ProcessMessage() != -1) {
		// ����̃��[�v���n�܂������Ԃ��o���Ă���
		LONGLONG time = GetNowHiPerformanceCount();

		ClearDrawScreen();

		// ���͍X�V
		input.Update();

		// �Q�[������
		sceneController.Update();
		sceneController.Draw();

		ScreenFlip();

		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;
		}

		// FPS60�ɌŒ肷��
		while (GetNowHiPerformanceCount() - time < 16667) {
		}
	}
}

void Application::Terminate()
{
	DxLib_End();
#ifdef _DEBUG	// �R���\�[��Debug�p
	fclose(_out); fclose(_in); FreeConsole();//�R���\�[�����
#endif	// _debug
}