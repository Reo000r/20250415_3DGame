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

	/// <summary>
	/// �����蔻��𖳎�����^�O���ǂ���
	/// </summary>
	/// <param name="target">�^�O</param>
	/// <returns>��������ꍇ��true</returns>
	bool IsThroughTarget(const PhysicsData::GameObjectTag target) const;

	// �����蔻��𖳎�����^�O�̃��X�g
	std::list<PhysicsData::GameObjectTag>	throughTags;

	// MEMO:����public�ɂ������肠�邩��
public:

	PhysicsData::ColliderKind	kind;
	bool	isTrigger;

};

