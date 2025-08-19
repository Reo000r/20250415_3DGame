#include "SceneOperationInstruction.h"
#include "SceneGamePlay.h"  // 遷移先のシーン
#include "SceneTitle.h"  // 遷移先のシーン
#include "SceneController.h"

#include "Input.h"
#include "Statistics.h"

#include <DxLib.h>
#include <cassert>

namespace {
	const unsigned int kTextColor = GetColor(255, 255, 255);
	const std::wstring kDefaultFontName = L"Yu Gothic UI";
	const std::wstring kNextSceneTextFontName = L"Impact";
	constexpr int kHeadingFontSize = 92;		// 見出し文字サイズ
	constexpr int kSubheadingFontSize = 64;		// 小見出し文字サイズ
	constexpr int kDescriptionFontSize = 56;	// 説明文字サイズ
	constexpr int kNextSceneFontSize = 92;		// 案内文字サイズ
	constexpr int kFontThickness = 3;
	const std::wstring kHeadingText = L"操作方法";
	const std::wstring kPadNextSceneText = L"Press A to GameStart\n      Press B to Title";
	const std::wstring kKeybdNextSceneText = L"Press Enter to GameStart\n        Press Esc to Title";

	// アニメーション用の定数
	constexpr int kNextSceneFontThicknessIn = 40;	// 文字の出現期間(フレーム)
	constexpr int kNextSceneFontThicknessOut = 20;	// 文字の消滅期間(フレーム)
	constexpr int kNextSceneFontThicknessChange = 4;	// シーン切り替え中の文字の点滅切り替え時間(フレーム)

	// 描画レイアウト用の定数
	constexpr int kSubheadingTextStartX = Statistics::kScreenWidth * 0.4f;		// 小見出し描画開始X座標
	constexpr int kDescriptionTextStartX = Statistics::kScreenWidth * 0.45f;		// 詳細描画開始X座標
	constexpr int kHeadingTextY = Statistics::kScreenHeight * 0.1f;							// 見出し文字のY座標
	constexpr int kSubheadingTextStartY = kHeadingTextY + kHeadingFontSize + Statistics::kScreenHeight * 0.1f;	// 小見出し描画開始Y座標
	constexpr int kNextSceneTextYOffset = Statistics::kScreenHeight * 0.1f;					// 説明と案内の距離

	constexpr int kSubheadingOffset = kSubheadingFontSize * 1.0f;			// 小見出しと説明文の間隔
	constexpr int kDescriptionOffset = kDescriptionFontSize * 1.5f;		// 説明文同士の間隔

}

SceneOperationInstruction::SceneOperationInstruction() :
	_frame(Statistics::kFadeInterval),
	_nowUpdateState(&SceneOperationInstruction::FadeinUpdate),
	_nowDrawState(&SceneOperationInstruction::FadeDraw),
	_nextSceneName(NextSceneName::GamePlay),
	_nextScene(nullptr),
	_headingFontHandle(-1),
	_descriptionFontHandle(-1),
	_nextSceneFontHandle(-1),
	_nextSceneTextTickFrame(0),
	_isNextSceneTextActive(true)
{
	_headingFontHandle = CreateFontToHandle(kDefaultFontName.c_str(), kHeadingFontSize, kFontThickness,
		DX_FONTTYPE_ANTIALIASING_EDGE);
	assert(_headingFontHandle >= 0 && "フォントの作成に失敗");
	_subheadingFontHandle = CreateFontToHandle(kDefaultFontName.c_str(), kSubheadingFontSize, kFontThickness,
		DX_FONTTYPE_ANTIALIASING_EDGE);
	assert(_subheadingFontHandle >= 0 && "フォントの作成に失敗");
	_descriptionFontHandle = CreateFontToHandle(kDefaultFontName.c_str(), kDescriptionFontSize, kFontThickness,
		DX_FONTTYPE_ANTIALIASING_EDGE);
	assert(_descriptionFontHandle >= 0 && "フォントの作成に失敗");
	_nextSceneFontHandle = CreateFontToHandle(kNextSceneTextFontName.c_str(), kNextSceneFontSize, kFontThickness,
		DX_FONTTYPE_ANTIALIASING_EDGE);
	assert(_nextSceneFontHandle >= 0 && "フォントの作成に失敗");

	// 説明文
	_subheadingString.emplace_back	(L"移動");
	_descriptionString.emplace_back	(L"左スティック");
	_subheadingString.emplace_back	(L"カメラ操作");
	_descriptionString.emplace_back	(L"右スティック");
	_subheadingString.emplace_back	(L"攻撃");
	_descriptionString.emplace_back	(L"Yボタン");

	// 最後の入力に応じて文字を変える
	if (Input::GetInstance().GetLastInputType() == Input::PeripheralType::keybd) {
		_descriptionString.clear();
		//_subheadingString.emplace_back(L"移動");
		_descriptionString.emplace_back(L"WASD");
		//_subheadingString.emplace_back(L"カメラ操作");
		_descriptionString.emplace_back(L"マウス");
		//_subheadingString.emplace_back(L"攻撃");
		_descriptionString.emplace_back(L"左クリック");
	}
}

SceneOperationInstruction::~SceneOperationInstruction()
{
	// フォント解放
	if (_headingFontHandle != -1) {
		DeleteFontToHandle(_headingFontHandle);
	}
	if (_descriptionFontHandle != -1) {
		DeleteFontToHandle(_descriptionFontHandle);
	}
}

void SceneOperationInstruction::Init()
{
}

void SceneOperationInstruction::Update()
{
	(this->*_nowUpdateState)();
}

void SceneOperationInstruction::Draw()
{
	(this->*_nowDrawState)();
}

void SceneOperationInstruction::FadeinUpdate()
{
	_frame--;
	if (_frame <= 0) {
		_nowUpdateState = &SceneOperationInstruction::NormalUpdate;
		_nowDrawState = &SceneOperationInstruction::NormalDraw;
	}
}

void SceneOperationInstruction::NormalUpdate()
{
	// 決定を押したら
	if (Input::GetInstance().IsTrigger("Instruction:ChangeGameScene")) {
		_nextSceneName = NextSceneName::GamePlay;
		_nowUpdateState = &SceneOperationInstruction::FadeoutUpdate;
		_nowDrawState = &SceneOperationInstruction::FadeDraw;
		_frame = 0;
	}
	else if (Input::GetInstance().IsTrigger("Instruction:ChangeTitleScene")) {
		_nextSceneName = NextSceneName::Title;
		_nowUpdateState = &SceneOperationInstruction::FadeoutUpdate;
		_nowDrawState = &SceneOperationInstruction::FadeDraw;
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

void SceneOperationInstruction::FadeoutUpdate()
{
	_frame++;

	if (_frame >= Statistics::kFadeInterval) {
		if (_nextSceneName == NextSceneName::GamePlay) {
			_nextScene = std::make_shared<SceneGamePlay>();
		}
		else if (_nextSceneName == NextSceneName::Title) {
			_nextScene = std::make_shared<SceneTitle>();
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

void SceneOperationInstruction::FadeDraw()
{
	// 操作説明を描画
	DrawOperationInstructionString();


#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene OperationInstruction");
#endif


	// フェードイン/アウトの処理
	// フェード割合の計算(0.0-1.0)
	float rate = static_cast<float>(_frame) / static_cast<float>(Statistics::kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
	// BlendModeを使った後はNOBLENDにしておくことを忘れず
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneOperationInstruction::NormalDraw()
{
	// 操作説明を描画
	DrawOperationInstructionString();


#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene OperationInstruction");
#endif
}

void SceneOperationInstruction::DrawOperationInstructionString()
{
	// 描画高度
	int y = kHeadingTextY;

	// 見出しを描画
	// 文字の中央までの長さをはかり
	// 描画開始位置を求め描画する
	int headingTextWidth = GetDrawStringWidthToHandle(
		kHeadingText.c_str(),
		kHeadingText.length(),
		_headingFontHandle);
	int headingDrawX = (Statistics::kScreenWidth - headingTextWidth) * 0.5f;
	DrawStringToHandle(
		headingDrawX, y,
		kHeadingText.c_str(), kTextColor,
		_headingFontHandle);

	// どちらか少ないほうの数だけ
	// 小見出しと詳細を描画する
	int drawNum = _subheadingString.size();
	if (drawNum > _descriptionString.size()) drawNum = _descriptionString.size();

	y = kSubheadingTextStartY;

	for (int i = 0; i < drawNum; ++i) {
		// 小見出し描画
		DrawStringToHandle(
			kSubheadingTextStartX, y,
			_subheadingString[i].c_str(), kTextColor,
			_subheadingFontHandle);

		// 描画位置を下げる
		y += kSubheadingOffset;
		
		// 詳細描画
		DrawStringToHandle(
			kDescriptionTextStartX, y,
			_descriptionString[i].c_str(), kTextColor,
			_descriptionFontHandle);

		// 最後の要素でないなら次の説明と距離を開けておく
		if (i < drawNum + 1)	y += kDescriptionOffset;
	}

	// 案内が消滅状態なら描画しない
	if (!_isNextSceneTextActive) return;

	// 最後にシーン遷移案内を描画
	y += kNextSceneTextYOffset;

	// 最後の入力に応じて文字を変える
	std::wstring drawString = kPadNextSceneText;
	if (Input::GetInstance().GetLastInputType() == Input::PeripheralType::keybd) {
		drawString = kKeybdNextSceneText;
	}

	// 文字の中央までの長さをはかり
	// 描画開始位置を求め描画する
	int nextSceneTextWidth = GetDrawStringWidthToHandle(
		drawString.c_str(),
		drawString.length(),
		_nextSceneFontHandle);
	int nextSceneDrawX = (Statistics::kScreenWidth - nextSceneTextWidth) * 0.5f;
	DrawStringToHandle(
		nextSceneDrawX, y,
		drawString.c_str(), kTextColor,
		_nextSceneFontHandle);

}
