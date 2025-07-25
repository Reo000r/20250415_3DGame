﻿#include "Application.h"
#include "Input.h"
#include "SceneController.h"
#include "Statistics.h"
#include "DebugDraw.h"

#include <DxLib.h>
#include <cassert>
#include <string>

Application& Application::GetInstance()
{
	// 初実行時にメモリ確保
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

	// スクリーン初期化処理
	SetGraphMode(Statistics::kScreenWidth, Statistics::kScreenHeight, 32);
	ChangeWindowMode(true);
	SetWindowText(L"Window");

	// DxLibの初期化処理
	if (DxLib_Init()) {
		assert(false && "DxLib_Init_Error");
		return false;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	// 3D関連設定
	SetUseZBuffer3D(true);		// Zバッファ有効化
	SetWriteZBuffer3D(true);	// Zバッファへの書き込みを行う
	SetUseBackCulling(true);	// バックカリングを有効にする

	// GetRandシード設定
	auto t = static_cast<unsigned int>(time(nullptr));
	SRand(t);

	return true;
}

void Application::Run()
{
	// シングルトンオブジェクトを取得
	SceneController& sceneController = SceneController::GetInstance();
	Input& input = Input::GetInstance();
	DebugDraw& debugDraw = DebugDraw::GetInstance();

	while (ProcessMessage() != -1) {
		// 今回のループが始まった時間を覚えておく
		LONGLONG time = GetNowHiPerformanceCount();

		ClearDrawScreen();

#ifdef _DEBUG
		// デバッグ描画情報を初期化
		debugDraw.Clear();
#endif // _DEBUG

		// 入力更新
		input.Update();

		// ゲーム部分
		sceneController.Update();
		sceneController.Draw();

#ifdef _DEBUG
		// デバッグ描画
		debugDraw.Draw();
#endif // _DEBUG

		ScreenFlip();

		// escが押されたら
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;	// 処理を抜ける
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