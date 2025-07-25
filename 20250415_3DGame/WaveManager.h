#pragma once
#include "WaveData.h"
#include <vector>
#include <memory>
#include <unordered_map>

class EnemyBase;
class Player;
class Physics;

class WaveManager {
private:
	// ウェーブの状態
	enum class State {
		Spawning,           // ウェーブ開始、敵を生成中
		InProgress,         // 戦闘中
		WaitingForCleanup,  // ウェーブクリア、クリーンアップ待ち
		AllWavesComplete    // 全ウェーブ完了
	};

public:
	WaveManager();
	~WaveManager();

	void Init(std::weak_ptr<Player> player, std::weak_ptr<Physics> physics);
	void Update();
	void Draw();

	bool IsClear() const 
	{ return (_state == State::AllWavesComplete); }

private:

	/// <summary>
	/// 現在のウェーブ設定に基づいて敵を生成する
	/// </summary>
	void SpawnEnemiesForCurrentWave();

	/// <summary>
	/// ウェーブの完了条件(敵の全滅)をチェックする
	/// </summary>
	void CheckWaveCompletion();

	/// <summary>
	/// 次のウェーブへの移行処理
	/// </summary>
	void TransitionToNextWave();

	/// <summary>
	/// 倒された(死亡が完了した)敵をリストから削除する
	/// </summary>
	void CleanupDefeatedEnemies();

	/// <summary>
	/// 全てのウェーブ設定を初期化する
	/// </summary>
	void InitWaveSettings();

private:
	State _state;
	int _currentWaveIndex; // 現在のウェーブ番号(0 - (データサイズ-1))
	int _waveTransitionFrameCount; // ウェーブ間の待機フレームカウンター

	// このマネージャーが生成し、管理下にある全ての敵
	std::vector<std::shared_ptr<EnemyBase>> _enemies;

	// 全てのウェーブの構成データ
	std::vector<WaveData> _waveSettings;

	// 他のオブジェクトへの参照
	std::weak_ptr<Player> _player;
	std::weak_ptr<Physics> _physics;
};