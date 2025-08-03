#include "WeaponPlayer.h"
#include "EnemyBase.h"

WeaponPlayer::WeaponPlayer() :
	Weapon(PhysicsData::GameObjectTag::PlayerAttack)
{
}

void WeaponPlayer::OnCollide(const std::weak_ptr<Collider> collider)
{
    auto other = collider.lock();
    auto owner = _owner.lock();

    // 相手や所有者が不明な場合は何もしない
    if (collider.expired() || owner == nullptr) {
        return;
    }

    // 敵でないなら攻撃しない
    if (other->GetTag() != PhysicsData::GameObjectTag::Enemy) {
        return;
    }

    // 既に攻撃済みの相手なら何もしない
    for (auto& attacked : _attackedColliders) {
        if (attacked.lock() == other) {
            return;
        }
    }

    auto enemy = std::static_pointer_cast<EnemyBase>(other);
    // 相手にダメージ処理を依頼する
    enemy->TakeDamage(_attackPower, owner);

    // ダメージを与えた相手をリストに追加
    _attackedColliders.push_back(collider);
}

