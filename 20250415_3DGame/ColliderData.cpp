#include "ColliderData.h"

#include <cassert>

void ColliderData::AddThroughTag(PhysicsData::GameObjectTag tag)
{
	// �Ȃ����end���A���Ă���
	auto it = std::find(
		throughTags.begin(), 
		throughTags.end(), 
		tag);
	// end�ł����(�o�^����Ă��Ȃ����)
	if (it == throughTags.end())
	{
		throughTags.emplace_back(tag);
	}
	else
	{
		assert(false && "�w��̃^�O�͓o�^��");
	}
}

void ColliderData::RemoveThroughTag(PhysicsData::GameObjectTag tag)
{
	// �o�^����(eraseif �vC++20)
	auto count = std::erase_if(
		throughTags,
		[tag](PhysicsData::GameObjectTag target) { return target == tag; });
	// �o�^����ĂȂ�������assert
	if (count <= 0)
	{
		assert(false && "�w��̃^�O�͖��o�^");
	}
}

bool ColliderData::IsThroughTarget(const PhysicsData::GameObjectTag target) const
{
	auto it = std::find(throughTags.begin(), throughTags.end(), target);
	// end�łȂ����(�������)true�A�����łȂ����false
	return (it != throughTags.end());
}
