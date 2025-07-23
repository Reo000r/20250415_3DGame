#include "EnemyFactory.h"
#include "EnemyBase.h"
#include "EnemyNormal.h"
#include "Player.h"
#include "Physics.h"
#include <DxLib.h>
#include <cassert>
#include <string>

namespace {
	const std::wstring kNormalPath = L"data/model/character/EnemyNormal.mv1";
}

EnemyFactory::EnemyFactory()
{
}

EnemyFactory::~EnemyFactory()
{
}

std::shared_ptr<EnemyBase> EnemyFactory::CreateAndRegister(
	EnemyType type,
	const Vector3& position,
	std::weak_ptr<Player> player,
	std::weak_ptr<Physics> physics)
{
	std::shared_ptr<EnemyBase> newEnemy = nullptr;

	// 敵の種類に応じて生成するクラスを切り替える
	switch (type) {
	case EnemyType::Normal:
	{
		int handle = MV1LoadModel(kNormalPath.c_str());
		newEnemy = std::make_shared<EnemyNormal>(handle);
		// 生成した敵の初期化
		if (newEnemy) {
			// 位置設定
			newEnemy->SetPos(position);
			// 派生先のInitを呼び出す
			newEnemy->Init(player);
			// physicsに登録
			newEnemy->EntryPhysics(physics);
		}
		break;
	}


	// case EnemyType::Other:
	//     newEnemy = std::make_shared<EnemyRanged>();
	//     break;
	// case EnemyType::Boss:
	//     newEnemy = std::make_shared<EnemyBoss>();
	//     break;

	default:
		assert(false && "不明な敵タイプが指定された");
		return nullptr;
	}

	return newEnemy;
}