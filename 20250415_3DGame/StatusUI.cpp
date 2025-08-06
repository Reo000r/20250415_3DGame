#include "StatusUI.h"
#include "Player.h"
#include "WaveManager.h"
#include "EnemyManager.h"
#include "EnemyBase.h"
#include "GameManager.h"
#include <DxLib.h>
#include <string>

namespace {
	// プレイヤーHPバー
	const int kPlayerHpBarPosX = 30;
	constexpr int kPlayerHpBarPosY = 30;
	constexpr int kPlayerHpBarWidth = 400;
	constexpr int kPlayerHpBarHeight = 30;

	const unsigned int kPlayerHpBarBackColor = GetColor(50, 50, 50);
	const unsigned int kPlayerHpBarFrontColor = GetColor(0, 200, 100);
	const unsigned int kPlayerHpBarFrameColor = GetColor(255, 255, 255);


	// 敵HPバー
	constexpr int kEnemyHpBarWidth = 100;
	constexpr int kEnemyHpBarHeight = 10;
	const unsigned int kEnemyHpBarBackColor = GetColor(50, 50, 50);
	const unsigned int kEnemyHpBarFrontColor = GetColor(220, 50, 50);
	const unsigned int kEnemyHpBarFrameColor = GetColor(255, 255, 255);
	const Position3 kEnemyBarOffset = Vector3(0,400,0);


	// スコア表示
	const int kScorePosX = 1280 - 32; // 画面右端から30ピクセル内側
	const int kScorePosY = 32;
	const unsigned int kScoreColor = GetColor(255, 255, 255);
	const std::wstring kScoreFontName = L"Impact"; // フォント名
	const int kScoreFontSize = 48;
}

StatusUI::StatusUI():
	_scoreFontHandle(-1)
{
}

StatusUI::~StatusUI()
{
	// フォントハンドルが有効なら削除
	if (_scoreFontHandle != -1) {
		DeleteFontToHandle(_scoreFontHandle);
	}
}

void StatusUI::Init(std::weak_ptr<Player> player, std::weak_ptr<WaveManager> waveManager, std::weak_ptr<EnemyManager> enemyManager)
{
	_player = player;
	_waveManager = waveManager;
	_enemyManager = enemyManager;

	_scoreFontHandle = CreateFontToHandle(
		kScoreFontName.c_str(),
		kScoreFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_EDGE);
}

void StatusUI::Update()
{
	// hitpoint情報を更新
}

void StatusUI::Draw()
{
	DrawScore();

	DrawPlayerHp();

	// 敵HP描画
	if (auto enemyManager = _enemyManager.lock()) {
		const auto& enemies = enemyManager->GetEnemies();
		for (const auto& enemy : enemies) {
			if (enemy) {
				DrawEnemyHp(enemy);
			}
		}
	}
}

void StatusUI::DrawPlayerHp()
{
	if (auto player = _player.lock()) {
		float maxHp = player->GetMaxHitPoint();
		if (maxHp > 0.0f) {
			float playerHitPoint = _player.lock()->GetHitPoint();
			// HPの割合を計算
			float hpRatio = playerHitPoint / maxHp;

			// 背景バー
			DrawBox(kPlayerHpBarPosX, kPlayerHpBarPosY,
				kPlayerHpBarPosX + kPlayerHpBarWidth, kPlayerHpBarPosY + kPlayerHpBarHeight,
				kPlayerHpBarBackColor, true);

			// 前景バー
			DrawBox(kPlayerHpBarPosX, kPlayerHpBarPosY,
				kPlayerHpBarPosX + static_cast<int>(kPlayerHpBarWidth * hpRatio),
				kPlayerHpBarPosY + kPlayerHpBarHeight,
				kPlayerHpBarFrontColor, true);

			// 枠線
			DrawBox(kPlayerHpBarPosX, kPlayerHpBarPosY,
				kPlayerHpBarPosX + kPlayerHpBarWidth, kPlayerHpBarPosY + kPlayerHpBarHeight,
				kPlayerHpBarFrameColor, false);
		}
	}
}

void StatusUI::DrawEnemyHp(std::shared_ptr<EnemyBase> enemy)
{
	// 敵のHPと最大HPを取得
	float currentHp = enemy->GetHitPoint();
	float maxHp = enemy->GetMaxHitPoint();
	// HPが0以下、または最大HPが0以下の敵は描画しない
	if (currentHp <= 0.0f || maxHp <= 0.0f) return;

	// 敵の頭上座標を計算
	Vector3 enemyPos = enemy->GetPos();
	Vector3 barWorldPos = enemyPos + kEnemyBarOffset;

	// 3D座標を2Dスクリーン座標に変換
	Position3 screenPos = ConvWorldPosToScreenPos(barWorldPos);

	// 画面外の場合は描画しない (Z座標が1.0fより大きいと画面奥)
	if (screenPos.z > 1.0f) return;


	// HPバーの中心がscreenPosに来るように、描画開始位置を計算
	int barStartX = static_cast<int>(screenPos.x) - kEnemyHpBarWidth * 0.5f;
	int barStartY = static_cast<int>(screenPos.y) - kEnemyHpBarHeight * 0.5f;

	// HPの割合を計算
	float hpRatio = currentHp / maxHp;

	// 背景バー
	DrawBox(barStartX, barStartY,
		barStartX + kEnemyHpBarWidth, barStartY + kEnemyHpBarHeight,
		kEnemyHpBarBackColor, true);

	// 前景バー
	DrawBox(barStartX, barStartY,
		barStartX + static_cast<int>(kEnemyHpBarWidth * hpRatio), barStartY + kEnemyHpBarHeight,
		kEnemyHpBarFrontColor, true);

	// 枠線
	DrawBox(barStartX, barStartY,
		barStartX + kEnemyHpBarWidth, barStartY + kEnemyHpBarHeight,
		kEnemyHpBarFrameColor, false);
}

void StatusUI::DrawScore()
{
	// GameManagerからスコアを取得
	int score = GameManager::GetInstance().GetScore();

	// スコアの文字列を作成
	std::wstring scoreText = L"Score : ";

	// 描画するX座標を計算 (右揃え)
	// まず"SCORE: "部分の幅を計算
	int textWidth = GetDrawStringWidthToHandle(scoreText.c_str(), scoreText.length(),
		_scoreFontHandle);
	// 次に5桁の数字部分の幅を計算
	int numWidth = GetDrawStringWidthToHandle(L"00000", 5, _scoreFontHandle);

	int totalWidth = textWidth + numWidth;
	int drawX = kScorePosX - totalWidth;

	// 文字を描画
	DrawStringToHandle(drawX, kScorePosY, scoreText.c_str(), kScoreColor, _scoreFontHandle);

	// スコアを5桁0埋めで描画
	DrawFormatStringToHandle(drawX + textWidth, kScorePosY, kScoreColor, _scoreFontHandle, L"%05d",
		score);
}
