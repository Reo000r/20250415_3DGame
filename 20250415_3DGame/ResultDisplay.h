#pragma once
#include <vector>
#include <memory>

class ResultItemBase;

class ResultDisplay
{
private:
	/// <summary>
	/// アニメーション状態
	/// </summary>
	enum class AnimationState {
		SlidingIn,  // スライドイン中
		AnimatingItems,// アニメーション中
		Finished    // アニメーション完了
	};

public:
	ResultDisplay();
	~ResultDisplay();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:

	// 更新処理の補助関数
	void UpdateSlidingIn();

	int _resultFontHandle;

	AnimationState _animationState;	// 現在のアニメーション状態
	int _animationTimer;            // アニメーション進行度タイマー
	
	float _labelDrawX;
	float _labelTargetDrawX;
	float _valueDrawX;
	float _valueTargetDrawX;

	std::vector<std::unique_ptr<ResultItemBase>> _resultItems;
};
