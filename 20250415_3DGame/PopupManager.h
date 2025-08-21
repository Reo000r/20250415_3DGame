//#pragma once
//
//class PopupManager
//{
//public:
//	PopupManager();
//	~PopupManager();
//
//	/// <summary>
//	/// 初期化
//	/// </summary>
//	void Init();
//
//	/// <summary>
//	/// 内部変数の更新
//	/// </summary>
//	void Update();
//
//	/// <summary>
//	/// 描画全般
//	/// </summary>
//	void Draw();
//
//private:
//
//	/// <summary>
//	/// ポップアップが入った時の更新処理
//	/// </summary>
//	void PopinUpdate();
//	/// <summary>
//	/// 通常時の更新処理
//	/// </summary>
//	void NormalUpdate();
//	/// <summary>
//	/// フェードアウト時の更新処理
//	/// </summary>
//	void FadeoutUpdate();
//
//	/// <summary>
//	/// フェード時の描画
//	/// </summary>
//	void FadeDraw();
//	/// <summary>
//	/// 通常時の描画
//	/// </summary>
//	void NormalDraw();
//
//
//	int _frame;
//
//	// _updateや_drawが変数であることを分かりやすくしている
//	using UpdateFunc_t = void (SceneTitle::*)();
//	using DrawFunc_t = void (SceneTitle::*)();
//	UpdateFunc_t _nowUpdateState = nullptr;
//	DrawFunc_t _nowDrawState = nullptr;
//
//	enum class NextSceneName {
//		GamePlay,		// ゲームシーン
//		Instruction,	// 操作説明
//	};
//
//	NextSceneName _nextSceneName;
//	std::shared_ptr<SceneBase> _nextScene;
//
//private:
//
//	/// <summary>
//	/// タイトルに描画する文字を描画
//	/// </summary>
//	void DrawTitleString();
//
//	// 文字ハンドル
//	int _titleFontHandle;
//	int _nextSceneFontHandle;
//
//	// 演出用変数
//	int _nextSceneTextTickFrame;	// 文字点滅時間を管理
//	bool _isNextSceneTextActive;	// 文字描画を行うか
//};
//
