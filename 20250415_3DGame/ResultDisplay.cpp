#include "ResultDisplay.h"
#include "Statistics.h"
#include "GameManager.h"
#include "ResultItemDrawer.h"
#include <DxLib.h>
#include <cassert>
#include <string>

namespace {

    const unsigned int kTextColor = GetColor(255, 255, 255);
    const std::wstring kFontName = L"Impact";
    constexpr int kResultFontSize = 128;     // 上部文字用サイズ
    constexpr int kFontThickness = 3;
    const std::wstring kResultText = L"RESULT";
    const std::wstring kScoreText = L"SCORE : ";
    const std::wstring kTimeText = L"TIME  : ";
    const std::wstring kTimeBonusText = L"TIME BONUS : ";
    const std::wstring kTotalScoreText = L"TOTAL SCORE : ";
    const std::wstring kScoreFormat = L"%05d";      // 5桁0埋め部分

    // アニメーション用の定数
    constexpr int kSlideInDuration = 45;            // スライドインにかかるフレーム

    // 描画レイアウト用の定数
    constexpr int kResultTextY = 100;               // "RESULT"文字のY座標
    constexpr int kItemStartY = kResultTextY + 200;
    constexpr int kItemOffsetY = 80;
}

ResultDisplay::ResultDisplay() :
    _resultFontHandle(-1),
    _animationState(AnimationState::SlidingIn),
    _animationTimer(0),
    _labelDrawX(0.0f),
    _labelTargetDrawX(0.0f),
    _valueDrawX(0.0f),
    _valueTargetDrawX(0.0f),
    _resultItems()
{
    _resultFontHandle = CreateFontToHandle(kFontName.c_str(), kResultFontSize, kFontThickness,
        DX_FONTTYPE_ANTIALIASING_EDGE);
    assert(_resultFontHandle >= 0 && "フォントの作成に失敗");
}

ResultDisplay::~ResultDisplay()
{
    if (_resultFontHandle != -1) {
        DeleteFontToHandle(_resultFontHandle);
    }
}

void ResultDisplay::Init()
{
    // GameManagerから値を取得
    int enemyDefeatScore = GameManager::GetInstance().GetEnemyDefeatScore();
    int timeBonus = GameManager::GetInstance().GetTimeBonusScore();
    float clearTime = GameManager::GetInstance().GetClearTime();
    int totalScore = GameManager::GetInstance().GetTotalScore();

    // 表示項目を生成してリストに追加
    _resultItems.push_back(std::make_unique<ResultItemDrawer>(
        ResultItemDrawer::ValueType::Number, kScoreText, static_cast<float>(enemyDefeatScore)));
    _resultItems.push_back(std::make_unique<ResultItemDrawer>(
        ResultItemDrawer::ValueType::Number, kTimeBonusText, static_cast<float>(timeBonus)));
    _resultItems.push_back(std::make_unique<ResultItemDrawer>(
        ResultItemDrawer::ValueType::Time, kTimeText, clearTime));
    _resultItems.push_back(std::make_unique<ResultItemDrawer>(
        ResultItemDrawer::ValueType::Number, kTotalScoreText, static_cast<float>(totalScore)));

    // 描画レイアウトの計算
    _labelTargetDrawX = Statistics::kScreenWidth * 0.3f;
    _valueTargetDrawX = Statistics::kScreenWidth * 0.7f;
    _labelDrawX = Statistics::kScreenWidth * 1.5f;
    _valueDrawX = Statistics::kScreenWidth * 1.5f;
}

void ResultDisplay::Update()
{
    _animationTimer++;

    switch (_animationState)
    {
    case AnimationState::SlidingIn:
    {
        UpdateSlidingIn();
    }
    break;

    case AnimationState::AnimatingItems:
    {
        bool allFinished = true;
        for (const auto& item : _resultItems) {
            item->Update(_animationTimer);
            if (!item->IsAnimationFinished()) {
                allFinished = false;
            }
        }
        if (allFinished) {
            _animationState = AnimationState::Finished;
        }
    }
    break;

    case AnimationState::Finished:
        // 何もしない
        break;
    }
}

void ResultDisplay::Draw()
{
    // "RESULT" の描画
    int resultTextWidth = GetDrawStringWidthToHandle(
        kResultText.c_str(), 
        kResultText.length(), 
        _resultFontHandle);
    int resultDrawX = (Statistics::kScreenWidth - resultTextWidth) * 0.5f;
    DrawStringToHandle(
        resultDrawX, kResultTextY, 
        kResultText.c_str(), kTextColor,
        _resultFontHandle);

    // 各項目の描画
    for (int i = 0; i < _resultItems.size(); ++i) {
        float drawY = static_cast<float>(kItemStartY + (kItemOffsetY * i));
        _resultItems[i]->Draw(_labelDrawX, _valueDrawX, drawY);
    }
}

void ResultDisplay::UpdateSlidingIn()
{
    float progress = std::min<float>(
        static_cast<float>(_animationTimer) / kSlideInDuration,
        1.0f);
    // イージングで滑らかな動きに
    float easedProgress = 1.0f - pow(1.0f - progress, 3.0f);

    float startX = Statistics::kScreenWidth * 1.5f;
    
    _labelDrawX = startX + (_labelTargetDrawX - startX) * easedProgress;
    _valueDrawX = startX + (_valueTargetDrawX - startX) * easedProgress;

    if (progress >= 1.0f) {
        _animationState = AnimationState::AnimatingItems;
        _animationTimer = 0;
    }
}
