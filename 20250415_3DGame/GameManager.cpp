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
	if (auto waveManager = _waveManager.lock()) {
		return waveManager->GetTotalWaveCount();
	}
	return 0;
}

bool GameManager::IsPlayerAlive() const
{
	if (auto player = _player.lock()) {
		return player->IsAlive();
	}
	return false;
}
