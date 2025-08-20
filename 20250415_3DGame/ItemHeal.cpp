#include "ItemHeal.h"
#include "Player.h"

ItemHeal::ItemHeal(int modelHandle) :
	ItemBase(ItemType::Heal, modelHandle)
{
	// 処理なし
}

ItemHeal::~ItemHeal()
{
	// 処理なし
}

void ItemHeal::Init(Vector3 modelTransOffset, BuffData::BuffStats stats)
{
	_stats = stats;
	_modelOffset = modelTransOffset;
}

void ItemHeal::PlayerCatched(const std::shared_ptr<Player> player)
{
	// 回復量分だけ回復させる
	player->AttachBuff(Buff::Heal, _stats);
}
