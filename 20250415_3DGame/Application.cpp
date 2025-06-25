#include "Application.h"
#include "Input.h"
#include "SceneController.h"
#include "Statistics.h"

#include <DxLib.h>
#include <cassert>
#include <string>

Application& Application::GetInstance()
{
	// この宣言の時点でメモリが確保されてアプリ終了まで残る
	static Application app;
	return app;
}

bool Application::Init()
{
#ifdef _DEBUG	// コンソールDebug用
	AllocConsole();							// コンソール
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

	// 3D関連設定
	SetUseZBuffer3D(true);		// Zバッファ有効化
	SetWriteZBuffer3D(true);	// Zバッファへの書き込みを行う
	SetUseBackCulling(true);	// バックカリングを有効にする

	return true;
}

void Application::Run()
{
	SceneController& sceneController = SceneController::GetInstance();
	Input& input = Input::GetInstance();

	while (ProcessMessage() != -1) {
		// 今回のループが始まった時間を覚えておく
		LONGLONG time = GetNowHiPerformanceCount();

		ClearDrawScreen();

		// 入力更新
		input.Update();

		// ゲーム部分
		sceneController.Update();
		sceneController.Draw();

		ScreenFlip();

		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;
		}

		// FPS60に固定する
		while (GetNowHiPerformanceCount() - time < 16667) {
		}
	}
}

void Application::Terminate()
{
	DxLib_End();
#ifdef _DEBUG	// コンソールDebug用
	fclose(_out); fclose(_in); FreeConsole();//コンソール解放
#endif	// _debug
}