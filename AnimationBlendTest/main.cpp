#include <DxLib.h>
#include <cassert>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (DxLib_Init() == -1) {
        return -1;
    }

#ifdef _DEBUG	// コンソールDebug用
	FILE* _out;
	FILE* _in;
	AllocConsole();							// コンソール
	_out = 0;
	freopen_s(&_out, "CON", "w", stdout);	// stdout
	_in = 0;
	freopen_s(&_in, "CON", "r", stdin);		// stdin
#endif	// _DEBUG

	ChangeWindowMode(true);
    SetGraphMode(640, 480, 32);
	SetWindowText(L"Test Window");

	if (DxLib_Init()) {
		assert(false && "DxLib_Init_Error");
		return false;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	// 3D関連設定
	SetUseZBuffer3D(true);		// Zバッファ有効化
	SetWriteZBuffer3D(true);	// Zバッファへの書き込みを行う
	SetUseBackCulling(true);	// バックカリングを有効にする

    // カメラ設定
    SetCameraPositionAndTarget_UpVecY(VGet(0, 200, -500), VGet(0, 80, 0));



    // モデルの読み込み
    int modelHandle = MV1LoadModel(L"model.mv1"); // あなたのモデルファイル名に置き換えてください

    // アニメーションをモデルにアタッチ
    // 第2引数でモデルファイル内のアニメーション番号を指定
    int walkAttachIndex = MV1AttachAnim(modelHandle, MV1GetAnimIndex(modelHandle, L"CharacterArmature|Idle"), -1, FALSE); // 待機アニメーション
    int runAttachIndex = MV1AttachAnim(modelHandle, MV1GetAnimIndex(modelHandle, L"CharacterArmature|Walk"), -1, FALSE); // 歩きアニメーション

    // 各アニメーションの総時間を取得
    float totalWalkTime = MV1GetAttachAnimTotalTime(modelHandle, walkAttachIndex);
    float totalRunTime = MV1GetAttachAnimTotalTime(modelHandle, runAttachIndex);

    // アニメーションの再生時間
    float walkTime = 0.0f;
    float runTime = 0.0f;

    // ブレンド率 (0.0: 歩き100% -> 1.0: 走り100%)
    float blendRate = 0.0f;
    bool isRunning = false; // 走りに切り替えるフラグ






    // メインループ
    while (ProcessMessage() == 0) {
		// 今回のループが始まった時間を覚えておく
		LONGLONG time = GetNowHiPerformanceCount();

		ClearDrawScreen();


		// 処理

        // スペースキーが押されたら「走り」フラグを立てる
        if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
            isRunning = true;
        }
        else {
            isRunning = false;
        }

        // ブレンド率を目標値(isRunning ? 1.0f : 0.0f)に近づける
        if (isRunning && blendRate < 1.0f) {
            blendRate += 0.05f; // 徐々に走りの割合を増やす
            if (blendRate > 1.0f) blendRate = 1.0f;
        }
        else if (!isRunning && blendRate > 0.0f) {
            blendRate -= 0.05f; // 徐々に歩きの割合を増やす
            if (blendRate < 0.0f) blendRate = 0.0f;
        }

        // 各アニメーションのブレンド率を設定
        // 2つのブレンド率の合計が常に1.0になるようにする
        MV1SetAttachAnimBlendRate(modelHandle, walkAttachIndex, 1.0f - blendRate);
        MV1SetAttachAnimBlendRate(modelHandle, runAttachIndex, blendRate);

        // アニメーション時間の更新 (ループ再生)
        walkTime += 1.0f;
        if (walkTime >= totalWalkTime) {
            walkTime = 0.0f;
        }
        runTime += 1.0f;
        if (runTime >= totalRunTime) {
            runTime = 0.0f;
        }

        // アニメーションの再生時間をモデルに設定
        MV1SetAttachAnimTime(modelHandle, walkAttachIndex, walkTime);
        MV1SetAttachAnimTime(modelHandle, runAttachIndex, runTime);

        // モデルの描画
        MV1DrawModel(modelHandle);

        // デバッグ情報の表示
        DrawFormatString(10, 10, GetColor(255, 255, 255), L"SPACEキーで歩き/走り切り替え");
        DrawFormatString(10, 30, GetColor(255, 255, 255), L"BlendRate: %.2f", blendRate);
        DrawFormatString(10, 50, GetColor(255, 255, 255), L"Walk: %.2f%%, Run: %.2f%%", (1.0f - blendRate) * 100, blendRate * 100);



		ScreenFlip();

		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;
		}

		// FPS60に固定する
		while (GetNowHiPerformanceCount() - time < 16667) {
		}
	}

    DxLib_End();
    return 0;
}