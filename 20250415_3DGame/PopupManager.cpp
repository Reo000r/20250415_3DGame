#include "PopupManager.h"
#include "PopupBase.h"
#include "Statistics.h"

#include <DxLib.h>
#include <cassert>

namespace {
	constexpr float kFadeAmount = 0.3f;	// 背景の暗さ度合(0-1)
}

PopupManager::PopupManager() :
	_frame(Statistics::kFadeInterval),
	_isPopup(false),
	_nowUpdateState(&PopupManager::PopinUpdate),
	_nowDrawState(&PopupManager::FadeDraw)
{
}

PopupManager::~PopupManager()
{
}

void PopupManager::StartPopup(std::shared_ptr<PopupBase> popupBase)
{
	if (!popupBase) {
		assert(false && "不正なポップアップデータ");
		return;
	}
	_isPopup = true;
	_popup = popupBase;
	_popup->Init();
}

void PopupManager::Update()
{
	if (!_popup) {
		assert(false && "不正なポップアップデータ");
	}
	(this->*_nowUpdateState)();
}

void PopupManager::Draw()
{
	if (!_popup) {
		assert(false && "不正なポップアップデータ");
	}
	(this->*_nowDrawState)();
}

void PopupManager::PopinUpdate()
{
	// ポップアップが始まっていない場合はreturn
	if (!_isPopup) return;

	_frame--;

	// 指定のフレームを超えたら
	if (_frame <= 0) {
		// ポップアップの作業を行う
		_nowUpdateState = &PopupManager::NormalUpdate;
		_nowDrawState = &PopupManager::NormalDraw;
		_frame = 0;
	}
}

void PopupManager::NormalUpdate()
{
	// ポップアップの処理が全て終了したら
	if (_popup->IsProcessingCompleted()) {
		// ポップアップの内容の後処理を行い終了処理に移る
		_popup->Final();
		_nowUpdateState = &PopupManager::PopoutUpdate;
		_nowDrawState = &PopupManager::FadeDraw;
		_frame = 0;
		return;
	}

	_popup->Update();
}

void PopupManager::PopoutUpdate()
{
	_frame++;

	// 指定のフレームを超えたら
	if (_frame >= Statistics::kFadeInterval) {
		_popup.reset();		// 削除
		_isPopup = false;
		// 次回の処理へ向けての準備
		_nowUpdateState = &PopupManager::PopinUpdate;
		_nowDrawState = &PopupManager::FadeDraw;
		_frame = Statistics::kFadeInterval;
		return;
	}
}

void PopupManager::FadeDraw()
{
	// 背景を暗くする処理
	// フェード割合の計算(0.0-定数)
	float rate = static_cast<float>(_frame) / static_cast<float>(Statistics::kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>((255 * kFadeAmount) * rate));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
	// BlendModeを使った後はNOBLENDにしておくことを忘れず
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Popup");
#endif
}

void PopupManager::NormalDraw()
{
	// フェードイン/アウトの処理
	// フェード割合の計算(0.0-定数)
	float rate = static_cast<float>(_frame) / static_cast<float>(Statistics::kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>((255 * kFadeAmount) * rate));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
	// BlendModeを使った後はNOBLENDにしておくことを忘れず
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Popup");
#endif
}
