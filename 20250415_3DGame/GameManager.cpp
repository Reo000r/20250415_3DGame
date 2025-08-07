#include "GameManager.h"
#include "Player.h"
#include "WaveManager.h"

GameManager::~GameManager()
{
}

GameManager& GameManager::GetInstance()
{
	// 初実行時にメモリ確保
	static GameManager manager;
	return manager;
}

void GameManager::Init(std::weak_ptr<Player> player, std::weak_ptr<WaveManager> waveManager)
{
	_player = player;
	_waveManager = waveManager;
	_score = 0;
	_clearTime = 0.0f;
}

void GameManager::Update()
{
	// 処理なし
}

void GameManager::AddScore(int score)
{
	if (score > 0) {
		_score += score;
	}
}

void GameManager::UpdateClearTime()
{
	_clearTime += 1.0f / 60.0f;
}

int GameManager::GetCurrentWaveIndex() const
{
	auto waveManager = _waveManager.lock();
	if (waveManager) {
		// WaveManagerに現在のウェーブ数を返すgetterが必要
		return waveManager->GetCurrentWaveIndex();
	}
	return 0;
}

int GameManager::GetTotalWaves() const
{
	auto waveManager = _waveManager.lock();
	if (waveManager) {
		return waveManager->GetTotalWaveCount();
	}
	return 0;
}

bool GameManager::IsPlayerAlive() const
{
	auto player = _player.lock();
	if (player) {
		return player->IsAlive();
	}
	return false;
}
