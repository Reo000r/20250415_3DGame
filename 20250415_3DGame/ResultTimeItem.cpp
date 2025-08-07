#include "ResultTimeItem.h"
#include "Statistics.h"
#include <DxLib.h>
#include <string>
#include <cmath>
#include <cassert>

namespace {
	// タイム表示に固有の定数
	const unsigned int kTextColor = GetColor(255, 255, 255);
	const std::wstring kFontName = L"Impact";
	constexpr int kTimeFontSize = 64;
	constexpr int kFontThickness = 3;
	constexpr int kTimePadding = 20;
	const std::wstring kTimeText = L"TIME : ";
	constexpr int kTimeDigitCount = 6;		// MMSSss
	constexpr int kDigitFinalizeDuration = 30;
}

ResultTimeItem::ResultTimeItem(float clearTime) :
	_clearTime(clearTime),
	_displayTime(0),
	_finalizedDigitCount(0),
	_isAnimationFinished(false)
{
	_fontHandle = CreateFontToHandle(kFontName.c_str(), kTimeFontSize, kFontThickness,
		DX_FONTTYPE_ANTIALIASING_EDGE);
	assert(_fontHandle >= 0 && "フォントの作成に失敗");
}

ResultTimeItem::~ResultTimeItem()
{
	// fontはBaseで開放している
}

void ResultTimeItem::Update(int animationTimer)
{
	if (_isAnimationFinished) return;

	_finalizedDigitCount = animationTimer / kDigitFinalizeDuration;
	if (_finalizedDigitCount >= kTimeDigitCount) {
		_finalizedDigitCount = kTimeDigitCount;
		_isAnimationFinished = true;
	}

	int minutes = static_cast<int>(_clearTime) / 60;
	int seconds = static_cast<int>(_clearTime) % 60;
	int milliseconds = static_cast<int>((_clearTime - floor(_clearTime)) * 100.0f);
	int finalTime = minutes * 10000 + seconds * 100 + milliseconds;

	if (_isAnimationFinished) {
		_displayTime = finalTime;
	}
	else {
		int tempTime = 0;
		int powerOf10 = 1;
		for (int i = 0; i < kTimeDigitCount; ++i) {
			int digit = (i < _finalizedDigitCount) ? (finalTime % 10) : GetRand(9);
			tempTime += digit * powerOf10;
			powerOf10 *= 10;
			finalTime /= 10;
		}
		_displayTime = tempTime;
	}
}

void ResultTimeItem::Draw(float labelX, float valueX, float baseY) const
{
	// ラベルを左揃えで描画
	DrawStringToHandle(static_cast<int>(labelX), static_cast<int>(baseY), kTimeText.c_str(),
		kTextColor, _fontHandle);

	// 数値を右揃えで描画
	int minutes = _displayTime / 10000;
	int seconds = (_displayTime / 100) % 100;
	int milliseconds = _displayTime % 100;

	// 各パーツの文字列と幅を計算
	wchar_t buffer[4];
	swprintf_s(buffer, L"%02d:", minutes);
	std::wstring minStr = buffer;
	swprintf_s(buffer, L"%02d.", seconds);
	std::wstring secStr = buffer;
	swprintf_s(buffer, L"%02d", milliseconds);
	std::wstring msStr = buffer;

	int minWidth = GetDrawStringWidthToHandle(minStr.c_str(), minStr.length(), _fontHandle);
	int secWidth = GetDrawStringWidthToHandle(secStr.c_str(), secStr.length(), _fontHandle);
	int msWidth = GetDrawStringWidthToHandle(msStr.c_str(), msStr.length(), _fontHandle);

	// 全体の幅から描画開始位置を計算
	int totalWidth = minWidth + secWidth + msWidth;
	int currentX = static_cast<int>(valueX) - totalWidth;

	// 各パーツを順番に描画
	DrawStringToHandle(currentX, static_cast<int>(baseY), minStr.c_str(), kTextColor,
		_fontHandle);
	currentX += minWidth;
	DrawStringToHandle(currentX, static_cast<int>(baseY), secStr.c_str(), kTextColor,
		_fontHandle);
	currentX += secWidth;
	DrawStringToHandle(currentX, static_cast<int>(baseY), msStr.c_str(), kTextColor,
		_fontHandle);
}

bool ResultTimeItem::IsAnimationFinished() const
{
	return _isAnimationFinished;
}
