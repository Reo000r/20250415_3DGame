#pragma once

#include <vector>
#include "Vector3.h"

enum class EnemyType;

// 1グループの敵の生成情報
struct SpawnInfo {
	EnemyType type;			// 敵の種類
	int count;				// 生成数
	Position3 basePosition;	// 生成基準位置
	float spawnRadius;		// この半径内にランダムに配置する
};

// 1ウェーブ分のデータ
struct WaveData {
	std::vector<SpawnInfo> spawnGroups; // このウェーブで生成される敵グループのリスト
};