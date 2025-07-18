﻿#pragma once
#include "SceneBase.h"

#include <memory>

class SceneResult final : public SceneBase {
public:
	SceneResult();
	~SceneResult();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 内部変数の更新
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// 描画全般
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

	// _updateや_drawが変数であることを分かりやすくしている
	using UpdateFunc_t = void(SceneResult::*)();
	using DrawFunc_t = void(SceneResult::*)();

	UpdateFunc_t _nowUpdateState = nullptr;
	DrawFunc_t   _nowDrawState = nullptr;

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
};

