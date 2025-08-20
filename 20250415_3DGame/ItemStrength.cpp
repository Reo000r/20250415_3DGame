#include "ItemStrength.h"

ItemStrength::ItemStrength(BuffData data, int modelHandle, 
	std::weak_ptr<PlayerBuffManager> manager) :
	ItemBase(data, modelHandle, manager)
{
	// 処理なし
}

ItemStrength::~ItemStrength()
{
	// 処理なし
}

void ItemStrength::Init(Vector3 modelTransOffset)
{
	_modelOffset = modelTransOffset;
}
