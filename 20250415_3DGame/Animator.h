#pragma once

#include <string>
#include <vector>

class Animator
{
private:
	// プロトタイプ宣言
	typedef wchar_t TCHAR;

public:
	struct AnimData
	{
		int animIndex = -1;			// アニメーション番号(元データにおける)
		int attachNo = -1;			// アタッチ番号
		std::wstring animName = L"";	// アニメーションの名前
		float frame = 0.0f;			// アニメーションの再生時間
		float totalFrame = 0.0f;	// アニメーションの総再生時間
		bool  isLoop = false;		// ループするか
		bool  isEnd = false;		// ループしない場合終了しているか
	};

	Animator();
	~Animator();

	void Init(int model);
	void Update();

	/// <summary>
	/// 最初に使用するアニメーションを設定
	/// </summary>
	/// <param name="animName"></param>
	/// <param name="isLoop"></param>
	void SetStartAnim(const std::wstring animName, const bool isLoop = true);
	/// <summary>
	/// ゲーム中で使用するアニメーションデータ
	/// </summary>
	/// <param name="animName"></param>
	/// <param name="isLoop"></param>
	void SetAnimData(const std::wstring animName, const bool isLoop);
	/// <summary>
	/// アニメーション名を指定しアタッチ
	/// (ブレンドの進行状況が止まるため初期化する目的で使用)
	/// </summary>
	/// <param name="animName"></param>
	/// <param name="isLoop"></param>
	void AttachAnim(const std::wstring animName, const bool isLoop = false);
	void UpdateAnim(AnimData& data);
	void UpdateAnimBlend();
	void ChangeAnim(const std::wstring animName, bool isLoop);

	AnimData& FindAnimData(const std::wstring animName);

	int GetModelHandle() const{ return _model; }
	AnimData& GetBlendingAnim();
	AnimData& GetExistAnim();
	bool GetBlendingAnimFinishState();

private:
	// モデルハンドル
	int _model;
	
	std::vector<AnimData> _animDataList;

	// 現在再生中のアニメーション名
	std::wstring _currentAnimName;
	// 次にメインとなるブレンド中のアニメーション名
	std::wstring _blendingAnimName;

	// アニメーションのブレンド比率
	// 0.0->1.0
	float _blendRate;
};

