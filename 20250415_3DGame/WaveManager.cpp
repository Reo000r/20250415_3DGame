#include "WaveManager.h"
#include "EnemyFactory.h"
#include "EnemyBase.h"
#include "Calculation.h"

#include <DxLib.h>

namespace {
	constexpr int kTotalWaves = 5;			// ウェーブ数
	constexpr int kBaseEnemyCount = 2;		// 初期出現敵数
	constexpr int kWavesPerIncrease = 2;	// このウェーブ数超えると敵が増える
	constexpr int kIncreaseAmount = 1;		// 敵増加量

	const Position3 kSpawnCenterPos = Vector3(0.0f, 0.0f, 700.0f);	// 敵出現中心位置
	constexpr float kSpawnRadius = 1200.0f;	// 敵出現半径

	// ウェーブ間のインターバル時間(秒)
	constexpr int kWaveTransitionIntervalFrame = 60.0f * 3;
}

WaveManager::WaveManager() :
	_state(State::Spawning),
	_currentWaveIndex(0),
	_waveTransitionFrameCount(0),
	_enemies(),
	_waveSettings(),
	_player(),
	_physics()
{
}

WaveManager::~WaveManager()
{
}

void WaveManager::Init(std::weak_ptr<Player> player, std::weak_ptr<Physics> physics)
{
	_player = player;
	_physics = physics;

	// 全てのウェーブ設定をデータとして読み込む
	InitWaveSettings();

	// 最初のウェーブの敵を生成
	SpawnEnemiesForCurrentWave();
}

void WaveManager::Update()
{
	// 管理下にある全ての敵オブジェクトのUpdateを呼び出す
	for (auto& enemy : _enemies) {
		enemy->Update();
	}

	// 現在のウェーブの状態に応じて全体の流れを制御する
	switch (_state) {
	case State::Spawning:
		// 敵の生成が完了したら即座に戦闘状態へ移行
		_state = State::InProgress;
		break;

	case State::InProgress:
		// 敵が全滅したかどうかを毎フレーム監視
		CheckWaveCompletion();
		break;

	case State::WaitingForCleanup:
		// 次のウェーブが始まるまでの待機時間を処理
		_waveTransitionFrameCount++;
		if (_waveTransitionFrameCount >= kWaveTransitionIntervalFrame) {
			// 待機時間が終了したら、次のウェーブへ移行
			TransitionToNextWave();
		}
		break;

	case State::AllWavesComplete:
		// 全てのウェーブが完了したら何もしない
		// GameScene側でこの状態を検知し
		// クリア処理などを行う
		break;
	}
}

void WaveManager::Draw()
{
	// 管理下の敵をすべて描画
	for (auto& enemy : _enemies) {
		enemy->Draw();
	}
#ifdef _DEBUG
	DrawSphere3D(kSpawnCenterPos, kSpawnRadius, 16, 0xff00ff, 0xff00ff, false);
#endif // _DEBUG
}

void WaveManager::InitWaveSettings()
{
	// 最初のウェーブで出現する敵の数
	int spawnAmount = kBaseEnemyCount;
	// kTotalWaves の数だけウェーブ設定を生成
	for (int i = 0; i < kTotalWaves; ++i) {
		WaveData wave;	// ウェーブデータ
		SpawnInfo info;	// 1グループ分の敵の出現情報を格納する変数
		info.type = EnemyType::Normal;	// 出現させる敵の種類設定
		// 特定のウェーブ数を超えるごとに、出現する敵の数を増やす
		if ((i + 1) % kWavesPerIncrease == 0) spawnAmount += kIncreaseAmount;
		info.count = spawnAmount;				// 出現数
		info.basePosition = kSpawnCenterPos;	// 生成中心位置
		info.spawnRadius = kSpawnRadius;		// この半径内にランダム配置

		// 設定登録
		wave.spawnGroups.push_back(info);
		_waveSettings.push_back(wave);
	}
}

void WaveManager::SpawnEnemiesForCurrentWave()
{
	// 死亡済みの敵をリストから完全に削除
	CleanupDefeatedEnemies();

	if (_currentWaveIndex >= _waveSettings.size()) {
		_state = State::AllWavesComplete;
		return;
	}

	const auto& currentWave = _waveSettings[_currentWaveIndex];
	for (const auto& group : currentWave.spawnGroups) {
		for (int i = 0; i < group.count; ++i) {
			// spawnRadius内にランダムな位置を計算
			float angle = static_cast<float>(Calc::ToRadian(GetRand(360)));						// ランダムな角度
			float radius = static_cast<float>(GetRand(static_cast<int>(group.spawnRadius)));	// 生成範囲内のランダムな半径
			// 具体的な生成位置を求める
			Position3 spawnPos = group.basePosition + Vector3(cos(angle) * radius, 0.0f, sin
			(angle) *radius);

			// Factoryを使って敵を生成し、リストに追加
			auto newEnemy = EnemyFactory::CreateAndRegister(group.type, spawnPos, _player,
				_physics);
			_enemies.push_back(newEnemy);
		}
	}
}

void WaveManager::CheckWaveCompletion()
{
	// 生きている敵（ActiveまたはDying状態）がいるかチェック
	bool isAnyEnemyAlive = false;
	for (const auto& enemy : _enemies) {
		if (enemy->GetState() != EnemyBase::State::Dead) {
			isAnyEnemyAlive = true;
			break;
		}
	}

	// もし生きている敵が一人もいなければ、ウェーブクリア
	if (!isAnyEnemyAlive) {
		_state = State::WaitingForCleanup;
		_waveTransitionFrameCount = 0;
		SpawnEnemiesForCurrentWave();
	}
}

void WaveManager::TransitionToNextWave()
{
	_currentWaveIndex++;
	if (_currentWaveIndex < _waveSettings.size()) {
		// 次のウェーブの敵を生成
		_state = State::Spawning;
	}
	else {
		// 全ウェーブ完了
		_state = State::AllWavesComplete;
	}
}

void WaveManager::CleanupDefeatedEnemies()
{
	// StateがDeadの敵をvectorの末尾に集めてから削除する
	_enemies.erase(
		std::remove_if(_enemies.begin(), _enemies.end(),
			[](const std::shared_ptr<EnemyBase>&
				enemy) {
					return (enemy->GetState() == EnemyBase::State::Dead);
			}),
		_enemies.end()
	);
}
