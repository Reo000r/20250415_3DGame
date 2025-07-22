#pragma once

#include <memory>
#include "Vector3.h"

class EnemyBase;
class Player;
class Physics;

enum EnemyType {
	Normal,
};

class EnemyFactory {
public:
	/// <summary>
	/// 敵の種類に応じてインスタンスを生成し、
	/// 初期化とphysicsへの登録を行う
	/// </summary>
	static std::shared_ptr<EnemyBase> CreateAndRegister(
		EnemyType type,
		const Vector3& position,
		std::weak_ptr<Player> player,
		std::weak_ptr<Physics> physics
	);
};