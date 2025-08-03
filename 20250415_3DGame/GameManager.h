#pragma once
#include <memory>

class Player;
class WaveManager;

// シングルトンとして実装
class GameManager {
	// シングルトン化
private:
	GameManager() : _score(0) {}
	GameManager(const GameManager&) = delete;
	void operator=(const GameManager&) = delete;

	~GameManager();

public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>GameManagerシングルトンオブジェクト</returns>
	static GameManager& GetInstance();

	/// <summary>
	/// ゲーム開始時やリトライ時に呼ばれる初期化処理
	/// </summary>
	void Init(std::weak_ptr<Player> player, std::weak_ptr<WaveManager> waveManager);

	/// <summary>
	/// 毎フレームの更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// スコアを加算する
	/// </summary>
	/// <param name="score">加算するスコア</param>
	void AddScore(int score);

	// getter
	int GetScore() const { return _score; }
	int GetCurrentWaveIndex() const;
	int GetTotalWaves() const;
	bool IsPlayerAlive() const;

private:
	
	// 監視対象へのポインタ
	std::weak_ptr<Player> _player;
	std::weak_ptr<WaveManager> _waveManager;

	// 管理するデータ
	int _score;
};

