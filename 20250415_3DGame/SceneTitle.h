#pragma once
#include "SceneBase.h"

#include <memory>

class SceneTitle : public SceneBase
{
private:
	int _frame;
	int _startFadeFrame;
	
	// _updateや_drawが変数であることを分かりやすくしている
	using UpdateFunc_t = void (SceneTitle::*)();
	using DrawFunc_t = void (SceneTitle::*)();
	UpdateFunc_t _nowUpdateState = nullptr;
	DrawFunc_t _nowDrawState = nullptr;

	// フェードイン時の更新処理
	void FadeinUpdate();
	// 通常時の更新処理
	void NormalUpdate();
	// フェードアウト時の更新処理
	void FadeoutUpdate();

	// フェード時の描画
	void FadeDraw();
	// 通常時の描画
	void NormalDraw();

public:
	SceneTitle();
	~SceneTitle();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 内部変数の更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画全般
	/// </summary>
	void Draw() override;
};