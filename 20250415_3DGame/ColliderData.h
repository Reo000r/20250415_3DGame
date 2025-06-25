#pragma once
#include <list>
#include "ProjectSettings.h"

class ColliderData abstract {
public:
	// �R���X�g���N�^
	ColliderData(PhysicsData::ColliderKind kind_, bool isTrigger_) :
		kind(kind_),
		isTrigger(isTrigger_)
	{
	}

	virtual ~ColliderData() {}

	// getter
	PhysicsData::ColliderKind GetKind() const { return kind; }
	bool IsTrigger() const { return isTrigger; }

	// �����蔻��𖳎�����^�O�̒ǉ�/�폜
	void AddThroughTag(PhysicsData::GameObjectTag tag);
	void RemoveThroughTag(PhysicsData::GameObjectTag tag);

	// �����蔻��𖳎�����^�O���ǂ���
	bool IsThroughTarget(const PhysicsData::GameObjectTag target) const;

	// �����蔻��𖳎�����^�O�̃��X�g
	std::list<PhysicsData::GameObjectTag>	throughTags;

private:
	PhysicsData::ColliderKind	kind;
	bool	isTrigger;

};

