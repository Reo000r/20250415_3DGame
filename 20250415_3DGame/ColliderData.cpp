#include "ColliderData.h"

#include <cassert>

void ColliderData::AddThroughTag(PhysicsData::GameObjectTag tag)
{
	// ‚È‚¯‚ê‚Îend‚ª‹A‚Á‚Ä‚­‚é
	auto it = std::find(
		throughTags.begin(), 
		throughTags.end(), 
		tag);
	// end‚Å‚ ‚ê‚Î(“o˜^‚³‚ê‚Ä‚¢‚È‚¯‚ê‚Î)
	if (it == throughTags.end())
	{
		throughTags.emplace_back(tag);
	}
	else
	{
		assert(false && "w’è‚Ìƒ^ƒO‚Í“o˜^Ï");
	}
}

void ColliderData::RemoveThroughTag(PhysicsData::GameObjectTag tag)
{
	// “o˜^‰ğœ(eraseif —vC++20)
	auto count = std::erase_if(
		throughTags,
		[tag](PhysicsData::GameObjectTag target) { return target == tag; });
	// “o˜^‚³‚ê‚Ä‚È‚©‚Á‚½‚çassert
	if (count <= 0)
	{
		assert(false && "w’è‚Ìƒ^ƒO‚Í–¢“o˜^");
	}
}

bool ColliderData::IsThroughTarget(const PhysicsData::GameObjectTag target) const
{
	auto it = std::find(throughTags.begin(), throughTags.end(), target);
	// end‚Å‚È‚¯‚ê‚Î(Œ©‚Â‚©‚ê‚Î)trueA‚»‚¤‚Å‚È‚¯‚ê‚Îfalse
	return (it != throughTags.end());
}
