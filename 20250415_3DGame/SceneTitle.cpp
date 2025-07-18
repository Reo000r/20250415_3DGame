#include "SceneTitle.h"
#include "SceneGamePlay.h"  // 遷移先のシーン
#include "SceneController.h"

#include "Input.h"
#include "Statistics.h"

#include <memory>
#include <DxLib.h>
#include <cassert>

void SceneTitle::FadeinUpdate()
{
	_frame--;
	if (_frame <= 0) {
		_nowUpdateState = &SceneTitle::NormalUpdate;
		_nowDrawState = &SceneTitle::NormalDraw;
	}
}

void SceneTitle::NormalUpdate()
{
	// 決定を押したら
	if (Input::GetInstance().IsTrigger("next")) {
		_nowUpdateState = &SceneTitle::FadeoutUpdate;
		_nowDrawState = &SceneTitle::FadeDraw;
		_frame = 0;
	}
}

void SceneTitle::FadeoutUpdate()
{
	_frame++;

	if (_frame >= Statistics::kFadeInterval) {
		SceneController::GetInstance().ChangeScene(std::make_shared<SceneGamePlay>());
		return;  // 余計な処理が入っているとまずいのでreturn
	}
}

void SceneTitle::FadeDraw()
{
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
#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene Title");
#endif
}

SceneTitle::SceneTitle() :
	_frame(Statistics::kFadeInterval),
	_nowUpdateState(&SceneTitle::FadeinUpdate),
	_nowDrawState(&SceneTitle::FadeDraw)
{
	//_titleGraphHandle = LoadGraph(L"data/img/background/Title.png");
	//assert(_titleGraphHandle >= 0);
}

SceneTitle::~SceneTitle()
{
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