#include "SceneTitle.h"
#include "SceneGamePlay.h"  // 遷移先のシーン
#include "SceneOperationInstruction.h"  // 遷移先のシーン
#include "SceneController.h"

#include "Input.h"
#include "Statistics.h"

#include <DxLib.h>
#include <cassert>

namespace {
	const unsigned int kTextColor = GetColor(255, 255, 255);
	const std::wstring kFontName = L"Impact";
	constexpr int kTitleFontSize = 128;				// タイトル文字サイズ
	constexpr int kNextSceneFontSize = 96;
	constexpr int kFontThickness = 3;
	const std::wstring kTitleText = L"(Title)";
	const std::wstring kNextSceneText = L"       Press A to Start\nPress B to Instruction";

	// アニメーション用の定数
	constexpr int kNextSceneFontThicknessIn		= 40;	// 文字の出現期間(フレーム)
	constexpr int kNextSceneFontThicknessOut	= 20;	// 文字の消滅期間(フレーム)
	constexpr int kNextSceneFontThicknessChange	= 4;	// シーン切り替え中の文字の点滅切り替え時間(フレーム)

	// 描画レイアウト用の定数
	constexpr int kTitleTextY = Statistics::kScreenHeight * 0.1f;						// タイトル文字のY座標
	constexpr int kNextSceneTextY = kTitleTextY + Statistics::kScreenHeight * 0.6f;
}

SceneTitle::SceneTitle() :
	_frame(Statistics::kFadeInterval),
	_nowUpdateState(&SceneTitle::FadeinUpdate),
	_nowDrawState(&SceneTitle::FadeDraw),
	_nextSceneName(NextSceneName::GamePlay),
	_nextScene(nullptr),
	_nextSceneTextTickFrame(0),
	_isNextSceneTextActive(true)
{
	_titleFontHandle = CreateFontToHandle(kFontName.c_str(), kTitleFontSize, kFontThickness,
		DX_FONTTYPE_ANTIALIASING_EDGE);
	assert(_titleFontHandle >= 0 && "フォントの作成に失敗");
	_nextSceneFontHandle = CreateFontToHandle(kFontName.c_str(), kNextSceneFontSize, kFontThickness,
		DX_FONTTYPE_ANTIALIASING_EDGE);
	assert(_nextSceneFontHandle >= 0 && "フォントの作成に失敗");


	//_titleGraphHandle = LoadGraph(L"data/img/background/Title.png");
	//assert(_titleGraphHandle >= 0);
}

SceneTitle::~SceneTitle()
{
	// フォント解放
	if (_titleFontHandle != -1) {
		DeleteFontToHandle(_titleFontHandle);
	}
	if (_nextSceneFontHandle != -1) {
		DeleteFontToHandle(_nextSceneFontHandle);
	}

	//DeleteGraph();
}

void SceneTitle::Init()
{
}

void SceneTitle::Update()
{
	(this->*_nowUpdateState)();
}

void SceneTitle::Draw()
{
	(this->*_nowDrawState)();
}

void SceneTitle::FadeinUpdate()
{
	_frame--;
	if (_frame <= 0) {
		_nowUpdateState = &SceneTitle::NormalUpdate;
		_nowDrawState = &SceneTitle::NormalDraw;
	}


	// フェードイン時は点滅を行わない

}

void SceneTitle::NormalUpdate()
{
	// 決定を押したら
	if (Input::GetInstance().IsTrigger("NextScene")) {
		_nowUpdateState = &SceneTitle::FadeoutUpdate;
		_nowDrawState = &SceneTitle::FadeDraw;
		_frame = 0;

		// FadeUpdateに移る前の処理を行う
		_isNextSceneTextActive = false;
		_nextSceneTextTickFrame = 0;
		return;
	}

	// 決定を押したら
	if (Input::GetInstance().IsTrigger("Title:ChangeGameScene")) {
		_nextSceneName = NextSceneName::GamePlay;
		_nowUpdateState = &SceneTitle::FadeoutUpdate;
		_nowDrawState = &SceneTitle::FadeDraw;
		_frame = 0;
	}
	else if (Input::GetInstance().IsTrigger("Title:ChangeInstructionScene")) {
		_nextSceneName = NextSceneName::Instruction;
		_nowUpdateState = &SceneTitle::FadeoutUpdate;
		_nowDrawState = &SceneTitle::FadeDraw;
		_frame = 0;
	}


	_nextSceneTextTickFrame++;

	// 文字描画状態切り替え
	// 表示されている　　かつ　出現時間を超えている
	// または
	// 表示されていない　かつ　消滅時間を超えているなら
	if ((_isNextSceneTextActive &&
		_nextSceneTextTickFrame >= kNextSceneFontThicknessIn) ||
		(!_isNextSceneTextActive && 
		_nextSceneTextTickFrame >= kNextSceneFontThicknessOut)) {

		// 時間をリセットし描画状態を反転させる
		_nextSceneTextTickFrame = 0;
		_isNextSceneTextActive = !_isNextSceneTextActive;
	}
}

void SceneTitle::FadeoutUpdate()
{
	_frame++;

	if (_frame >= Statistics::kFadeInterval) {
		if (_nextSceneName == NextSceneName::GamePlay) {
			_nextScene = std::make_shared<SceneGamePlay>();
		}
		else if (_nextSceneName == NextSceneName::Instruction) {
			_nextScene = std::make_shared<SceneOperationInstruction>();
		}
		else {
			assert(false && "次のシーンが不明");
		}

		if (_nextScene == nullptr) {
			assert(false && "次のシーンが不明");
		}
		SceneController::GetInstance().ChangeScene(_nextScene);
		return;  // 自分が死んでいるのでもし
		// 余計な処理が入っているとまずいのでreturn;
	}


	_nextSceneTextTickFrame++;

	// 文字描画状態切り替え
	// 点滅切り替え時間を超えているなら
	if (_nextSceneTextTickFrame >= kNextSceneFontThicknessChange) {
		// 時間をリセットし描画状態を反転させる
		_nextSceneTextTickFrame = 0;
		_isNextSceneTextActive = !_isNextSceneTextActive;
	}
}

void SceneTitle::FadeDraw()
{
	// タイトル文字を描画する
	DrawTitleString();


#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene Title");
#endif


	// フェードイン/アウトの処理
	// フェード割合の計算(0.0-1.0)
	float rate = static_cast<float>(_frame) / static_cast<float>(Statistics::kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
	// BlendModeを使った後はNOBLENDにしておくことを忘れず
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneTitle::NormalDraw()
{
	// タイトル文字を描画する
	DrawTitleString();


#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene Title");
#endif
}

void SceneTitle::DrawTitleString()
{
	// 文字描画

	// 文字の中央までの長さをはかり
	// 描画開始位置を求め描画する
	int titleTextWidth = GetDrawStringWidthToHandle(
		kTitleText.c_str(),
		kTitleText.length(),
		_titleFontHandle);
	int titleDrawX = (Statistics::kScreenWidth - titleTextWidth) * 0.5f;
	DrawStringToHandle(
		titleDrawX, kTitleTextY,
		kTitleText.c_str(), kTextColor,
		_titleFontHandle);

	// 消滅中は描画を行わない
	if (!_isNextSceneTextActive) return;

	// 同様に描画する
	int nextSceneTextWidth = GetDrawStringWidthToHandle(
		kNextSceneText.c_str(),
		kNextSceneText.length(),
		_nextSceneFontHandle);
	int NextSceneDrawX = (Statistics::kScreenWidth - nextSceneTextWidth) * 0.5f;
	DrawStringToHandle(
		NextSceneDrawX, kNextSceneTextY,
		kNextSceneText.c_str(), kTextColor,
		_nextSceneFontHandle);
}
