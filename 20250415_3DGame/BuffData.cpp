#include "BuffData.h"
#include "Player.h"

void PlayerBuffManager::Init(std::weak_ptr<Collider> owner)
{
	_owner = std::static_pointer_cast<Player>(owner.lock());
}

void PlayerBuffManager::Update()
{
	UpdateBuff();
}

void PlayerBuffManager::AttachBuff(BuffData data)
{
	// 対応したバフにステータスを設定する
	;
}

float PlayerBuffManager::IsActiveBuffState(BuffType buff) const
{
	return _buffs.find(buff)->second.isActive;
}

float PlayerBuffManager::GetBuffAmount(BuffType buff) const
{
	// バフが有効でなかったり
	// 残り時間が0より小さいならfalse
	if (!_buffs.find(buff)->second.isActive &&
		_buffs.find(buff)->second.activeFrame <= 0) {
		return 0.0f;
	}
	// 効果量を返す
	return _buffs.find(buff)->second.amount;
}

void PlayerBuffManager::InitBuff()
{
	// 効果を追加して残り時間を0にする
	_buffs.find(BuffType::Heal)->second = {};
	_buffs.find(BuffType::ScoreBoost)->second = {};
	_buffs.find(BuffType::Strength)->second = {};
}

void PlayerBuffManager::UpdateBuff()
{
	for (auto& data : _buffs) {
		data.activeFrame--;			// 残り時間を減らす
		// 残り時間がなければ飛ばす
		if (data.activeFrame <= 0 && data.isActive) {
			data.amount = 0.0f;
			data.isActive = false;
			continue;
		}

		switch (data.type) {
		case BuffType::Heal:		// 継続回復処理
			_owner.lock()->Heal(data.amount);
			break;
		case BuffType::ScoreBoost:	// スコア増加処理
			break;
		case BuffType::Strength:	// 攻撃力増加処理
			break;
		}
	}
}
