#pragma once
#include "SceneBase.h"
#include "Geometry.h"

#include <memory>

class Physics;
class Camera;
class Player;
class Ground;
class Skydome;

class SceneGamePlay final : public SceneBase {
public:
	SceneGamePlay();
	~SceneGamePlay();

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

private:
	// 
	int _frame;

	// 次のシーン
	std::shared_ptr<SceneBase> _nextScene;

	std::shared_ptr<Physics> _physics;

	std::shared_ptr<Camera> _camera;
	std::shared_ptr<Player> _player;
	std::shared_ptr<Ground> _ground;
	std::shared_ptr<Skydome> _skydome;

	// UpdateとDrawのStateパターン
	// _updateや_drawが変数であることを分かりやすくしている
	using UpdateFunc_t = void(SceneGamePlay::*)();
	using DrawFunc_t = void(SceneGamePlay::*)();
	UpdateFunc_t _nowUpdateState = nullptr;
	DrawFunc_t _nowDrawState = nullptr;

	/// <summary>
	/// フェードイン時の更新処理
	/// </summary>
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

