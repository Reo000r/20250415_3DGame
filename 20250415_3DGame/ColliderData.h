#pragma once
#include <list>
#include "ProjectSettings.h"

class ColliderData abstract {
public:
	// コンストラクタ
	ColliderData(PhysicsData::ColliderKind kind_, bool isTrigger_) :
		kind(kind_),
		isTrigger(isTrigger_)
	{
	}

	virtual ~ColliderData() {}

	// getter
	PhysicsData::ColliderKind GetKind() const { return kind; }
	bool IsTrigger() const { return isTrigger; }

	// 当たり判定を無視するタグの追加/削除
	void AddThroughTag(PhysicsData::GameObjectTag tag);
	void RemoveThroughTag(PhysicsData::GameObjectTag tag);

	// 当たり判定を無視するタグかどうか
	bool IsThroughTarget(const PhysicsData::GameObjectTag target) const;

	// 当たり判定を無視するタグのリスト
	std::list<PhysicsData::GameObjectTag>	throughTags;

private:
	PhysicsData::ColliderKind	kind;
	bool	isTrigger;

};

