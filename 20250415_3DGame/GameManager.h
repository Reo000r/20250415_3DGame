#pragma once
#include <memory>

class Player;
class WaveManager;

// シングルトンとして実装
class GameManager final {
	// シングルトン化
private:
	GameManager() : 
		_enemyDefeatScore(0),
		_clearTime(0.0f),
		_timeBonusScore(0)
	{}
	GameManager(const GameManager&) = delete;
	void operator=(const GameManager&) = delete;

	~GameManager();

public:
	// タイムボーナス計算用の定数
	static constexpr float kMaxBonusTime = (0 * 60.0f + 0.0f);	// 最大ボーナスがもらえるタイム（秒）
	static constexpr float kMinBonusTime = (2 * 60.0f + 0.0f);	// 最小ボーナスがもらえるタイム（秒）
	static constexpr int kMaxTimeBonusScore = 10000;	// 最大ボーナススコア
	static constexpr int kMinTimeBonusScore = 500;		// 最小ボーナススコア

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
	/// 初回プレイ状態を更新
	/// 操作説明を出した後に更新される想定
	/// </summary>
	void ChangeFirstPlayState();

	/// <summary>
	/// 敵撃破時のスコアを加算する
	/// </summary>
	/// <param name="score">加算するスコア</param>
	void AddEnemyDefeatScore(int score);

	/// <summary>
	/// クリアタイムを更新する
	/// 呼ばれるたび1f経ったとみなす
	/// </summary>
	void UpdateClearTime();

	/// <summary>
	/// クリアタイムに応じてタイムボーナスを計算する
	/// </summary>
	void CalculateTimeBonus();

	// getter
	int GetTotalScore() const { return _enemyDefeatScore + _timeBonusScore; }
	int GetEnemyDefeatScore() const { return _enemyDefeatScore; }
	int GetTimeBonusScore() const { return _timeBonusScore; }
	float GetClearTime() const { return _clearTime; }
	int GetCurrentWaveIndex() const;
	int GetTotalWaves() const;
	bool IsPlayerAlive() const;
	bool IsFirstPlay() const { return _isFirstPlay; };

private:
	
	// 監視対象へのポインタ
	std::weak_ptr<Player> _player;
	std::weak_ptr<WaveManager> _waveManager;

	// 管理するデータ
	int _enemyDefeatScore;
	float _clearTime;
	int _timeBonusScore;

	int _isFirstPlay = true;
};

