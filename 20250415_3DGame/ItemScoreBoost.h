#pragma once
#include "ItemBase.h"

class ItemScoreBoost : public ItemBase
{
public:
	ItemScoreBoost(int modelHandle);
	~ItemScoreBoost();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(Vector3 modelTransOffset);

	/// <summary>
	/// プレイヤーが触れた場合
	/// </summary>
	void PlayerCatched(const std::shared_ptr<Player> player) override;
};

