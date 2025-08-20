#pragma once
#include "ItemBase.h"

class ItemHeal : public ItemBase
{
public:
	ItemHeal(int modelHandle);
	~ItemHeal();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(Vector3 modelTransOffset, BuffData::BuffStats stats);

	/// <summary>
	/// プレイヤーが触れた場合
	/// </summary>
	void PlayerCatched(const std::shared_ptr<Player> player) override;
};

