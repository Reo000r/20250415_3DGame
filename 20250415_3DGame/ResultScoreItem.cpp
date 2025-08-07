#include "ResultScoreItem.h"
#include "Statistics.h"
#include <DxLib.h>
#include <string>
#include <cassert>

namespace {

    const unsigned int kTextColor = GetColor(255, 255, 255);
    const std::wstring kFontName = L"Impact";
    constexpr int kScoreFontSize = 64;      // スコア文字用サイズ
    constexpr int kFontThickness = 3;
    constexpr int kScorePadding = 20;               // テキストと数値の間の余白
    const std::wstring kResultText = L"RESULT";
    const std::wstring kScoreText = L"SCORE : ";    // テキスト部分
    const std::wstring kScoreFormat = L"%05d";      // 5桁0埋め部分

    constexpr int kDigitFinalizeDuration = 30;      // 1桁が確定するのにかかるフレーム
    constexpr int kScoreDigitCount = 5;             // スコアの表示桁数
}

ResultScoreItem::ResultScoreItem(int finalScore) :
    _finalScore(finalScore),
    _displayScore(0),
    _finalizedDigitCount(0),
    _isAnimationFinished(false)
{
    _fontHandle = CreateFontToHandle(kFontName.c_str(), kScoreFontSize, kFontThickness,
        DX_FONTTYPE_ANTIALIASING_EDGE);
    assert(_fontHandle >= 0 && "フォントの作成に失敗");
}

ResultScoreItem::~ResultScoreItem()
{
    // fontはBaseで開放している
}

void ResultScoreItem::Update(int animationTimer)
{
    if (_isAnimationFinished) return;

    _finalizedDigitCount = animationTimer / kDigitFinalizeDuration;
    if (_finalizedDigitCount >= kScoreDigitCount) {
        _finalizedDigitCount = kScoreDigitCount;
        _isAnimationFinished = true;
    }

    // 表示スコアの計算
    if (_isAnimationFinished) {
        _displayScore = _finalScore;
    }
    else {
        int tempScore = 0;
        int finalScoreCopy = _finalScore;
        int powerOf10 = 1;

        for (int i = 0; i < kScoreDigitCount; ++i) {
            int digit = 0;
            // 確定済みの桁（下位の桁）は本来の数字
            if (i < _finalizedDigitCount) {
                digit = finalScoreCopy % 10;
            }
            // 確定していない桁（上位の桁）はランダムな数字
            else {
                digit = GetRand(9); // 0-9
            }
            tempScore += digit * powerOf10;
            powerOf10 *= 10;
            finalScoreCopy /= 10;
        }
        _displayScore = tempScore;
    }
}

void ResultScoreItem::Draw(float labelX, float valueX, float baseY) const
{
    // ラベルを左揃えで描画
    DrawStringToHandle(static_cast<int>(labelX), static_cast<int>(baseY), kScoreText.c_str(),
        kTextColor, _fontHandle);

    // 数値を右揃えで描画
    wchar_t buffer[16];
    swprintf_s(buffer, kScoreFormat.c_str(), _displayScore);
    std::wstring scoreStr = buffer;

    int numberWidth = GetDrawStringWidthToHandle(scoreStr.c_str(), scoreStr.length(),
        _fontHandle);
    int numberDrawX = static_cast<int>(valueX) - numberWidth;
    DrawStringToHandle(numberDrawX, static_cast<int>(baseY), scoreStr.c_str(), kTextColor,
        _fontHandle);
}

bool ResultScoreItem::IsAnimationFinished() const
{
	return _isAnimationFinished;
}
