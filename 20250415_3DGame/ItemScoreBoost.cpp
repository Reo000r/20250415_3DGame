#include "ItemScoreBoost.h"

ItemScoreBoost::ItemScoreBoost(int modelHandle) :
	ItemBase(ItemType::ScoreBoost, modelHandle)
{
}

ItemScoreBoost::~ItemScoreBoost()
{
}

void ItemScoreBoost::Init(Vector3 modelTransOffset)
{
}

void ItemScoreBoost::PlayerCatched(const std::shared_ptr<Player> player)
{
}
